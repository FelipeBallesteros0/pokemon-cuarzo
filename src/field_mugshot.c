#include "global.h"
#include "decompress.h"
#include "sprite.h"
#include "script.h"
#include "event_data.h"
#include "field_weather.h"
#include "field_message_box.h"
#include "field_mugshot.h"
#include "constants/field_mugshots.h"
#include "data/field_mugshots.h"

// A field mugshot is 104x96, which is larger than the GBA's maximum 64x64
// sprite. It is therefore drawn as NUM_MUGSHOT_PARTS sub-sprites that read from
// a single packed 64x192 sheet (4 cells stacked vertically, in sprite-tile
// order so 1D OAM mapping keeps each part's tiles contiguous).
#define NUM_MUGSHOT_PARTS   4
#define MUGSHOT_SHEET_TILES 192
#define MUGSHOT_SHEET_SIZE  (MUGSHOT_SHEET_TILES * TILE_SIZE_4BPP) // 0x1800

static EWRAM_DATA u8 sFieldMugshotSpriteIds[2][NUM_MUGSHOT_PARTS] = {};
static EWRAM_DATA u8 sIsFieldMugshotActive = 0;
static EWRAM_DATA u8 sFieldMugshotSlot = 0;

#define TAG_MUGSHOT 0x9000
#define TAG_MUGSHOT2 0x9001

// Top-left anchor of the 104x96 portrait: right side, just above the message box.
#define MUGSHOT_X 136
#define MUGSHOT_Y 19

// Referenced from scripts via 'callnative CreateFieldMugshot'.
void CreateFieldMugshot(struct ScriptContext *ctx);

static void SpriteCB_FieldMugshot(struct Sprite *s);
static void _RemoveFieldMugshot(u8 slot);

static const struct OamData sFieldMugshotOam_64x64 = {
    .size = SPRITE_SIZE(64x64),
    .shape = SPRITE_SHAPE(64x64),
    .priority = 0,
};

static const struct OamData sFieldMugshotOam_64x32 = {
    .size = SPRITE_SIZE(64x32),
    .shape = SPRITE_SHAPE(64x32),
    .priority = 0,
};

// dx/dy: offset (px) of this part from the portrait's top-left anchor.
// w/h: part size in px (to convert the top-left anchor into a sprite center).
// tileOffset: first tile of this part within the packed sheet.
struct MugshotPart
{
    s16 dx;
    s16 dy;
    u8 w;
    u8 h;
    const struct OamData *oam;
    u16 tileOffset;
};

static const struct MugshotPart sMugshotParts[NUM_MUGSHOT_PARTS] =
{
    {  0,  0, 64, 64, &sFieldMugshotOam_64x64,   0 },
    { 64,  0, 64, 64, &sFieldMugshotOam_64x64,  64 },
    {  0, 64, 64, 32, &sFieldMugshotOam_64x32, 128 },
    { 64, 64, 64, 32, &sFieldMugshotOam_64x32, 160 },
};

static const struct SpriteTemplate sFieldMugshot_SpriteTemplate = {
    .tileTag = TAG_MUGSHOT,
    .paletteTag = TAG_MUGSHOT,
    .oam = &sFieldMugshotOam_64x64,
    .callback = SpriteCB_FieldMugshot,
    .anims = gDummySpriteAnimTable,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static void SpriteCB_FieldMugshot(struct Sprite *s)
{
    s->invisible = (s->data[0] == TRUE) ? FALSE : TRUE;
}

void RemoveFieldMugshot(void)
{
    u32 slot, i;

    ResetPreservedPalettesInWeather();
    for (slot = 0; slot < 2; slot++)
    {
        if (sFieldMugshotSpriteIds[slot][0] != SPRITE_NONE)
        {
            FreeSpriteTilesByTag(slot + TAG_MUGSHOT);
            FreeSpritePaletteByTag(slot + TAG_MUGSHOT);
        }
        for (i = 0; i < NUM_MUGSHOT_PARTS; i++)
        {
            if (sFieldMugshotSpriteIds[slot][i] != SPRITE_NONE)
            {
                DestroySprite(&gSprites[sFieldMugshotSpriteIds[slot][i]]);
                sFieldMugshotSpriteIds[slot][i] = SPRITE_NONE;
            }
        }
    }
    sIsFieldMugshotActive = FALSE;
}

void CreateFieldMugshot(struct ScriptContext *ctx)
{
    u16 id = VarGet(ScriptReadHalfword(ctx));
    u16 emote = VarGet(ScriptReadHalfword(ctx));

    _CreateFieldMugshot(id, emote);
}

static void _RemoveFieldMugshot(u8 slot)
{
    u32 i;

    ResetPreservedPalettesInWeather();
    // Hide the other slot's portrait (mirrors the original transition behavior).
    for (i = 0; i < NUM_MUGSHOT_PARTS; i++)
    {
        if (sFieldMugshotSpriteIds[slot ^ 1][i] != SPRITE_NONE)
            gSprites[sFieldMugshotSpriteIds[slot ^ 1][i]].data[0] = FALSE;
    }

    if (sFieldMugshotSpriteIds[slot][0] != SPRITE_NONE)
    {
        FreeSpriteTilesByTag(slot + TAG_MUGSHOT);
        FreeSpritePaletteByTag(slot + TAG_MUGSHOT);
        for (i = 0; i < NUM_MUGSHOT_PARTS; i++)
        {
            if (sFieldMugshotSpriteIds[slot][i] != SPRITE_NONE)
            {
                DestroySprite(&gSprites[sFieldMugshotSpriteIds[slot][i]]);
                sFieldMugshotSpriteIds[slot][i] = SPRITE_NONE;
            }
        }
    }
}

void _CreateFieldMugshot(u32 id, u32 emote)
{
    u32 slot = sFieldMugshotSlot, i;
    struct CompressedSpriteSheet sheet = { .size = MUGSHOT_SHEET_SIZE, .tag = slot + TAG_MUGSHOT };
    struct SpritePalette pal = { .tag = sheet.tag };

    if (sIsFieldMugshotActive)
        _RemoveFieldMugshot(slot);

    if (id >= NELEMS(sFieldMugshots))
        return;

    sheet.data = (sFieldMugshots[id][emote].gfx != NULL ? sFieldMugshots[id][emote].gfx : sFieldMugshotGfx_TestNormal);
    pal.data = (sFieldMugshots[id][emote].pal != NULL ? sFieldMugshots[id][emote].pal : sFieldMugshotPal_TestNormal);

    LoadSpritePalette(&pal);
    LoadCompressedSpriteSheet(&sheet);

    for (i = 0; i < NUM_MUGSHOT_PARTS; i++)
    {
        const struct MugshotPart *part = &sMugshotParts[i];
        struct SpriteTemplate temp = sFieldMugshot_SpriteTemplate;
        u8 spriteId;

        temp.tileTag = sheet.tag;
        temp.paletteTag = sheet.tag;
        temp.oam = part->oam;
        // CreateSprite positions by center, so offset the top-left anchor by half the part size.
        spriteId = CreateSprite(&temp, MUGSHOT_X + part->dx + part->w / 2, MUGSHOT_Y + part->dy + part->h / 2, 0);
        sFieldMugshotSpriteIds[slot][i] = spriteId;
        if (spriteId == SPRITE_NONE)
            continue;

        // CreateSprite already set tileNum to the sheet's start; point this part at its cell.
        gSprites[spriteId].oam.tileNum += part->tileOffset;
        gSprites[spriteId].data[0] = FALSE; // stays hidden until the message box reveals it
        if (i == 0)
            PreservePaletteInWeather(gSprites[spriteId].oam.paletteNum + 0x10);
    }

    sIsFieldMugshotActive = TRUE;
    sFieldMugshotSlot ^= 1;
}

void SetFieldMugshotVisible(bool32 visible)
{
    u8 slot = sFieldMugshotSlot ^ 1; // the most recently created portrait
    u32 i;

    for (i = 0; i < NUM_MUGSHOT_PARTS; i++)
    {
        if (sFieldMugshotSpriteIds[slot][i] != SPRITE_NONE)
            gSprites[sFieldMugshotSpriteIds[slot][i]].data[0] = visible;
    }
}

u8 IsFieldMugshotActive(void)
{
    return sIsFieldMugshotActive;
}

void SetFieldMugshotSpriteId(u32 value)
{
    u32 slot, i;

    for (slot = 0; slot < 2; slot++)
        for (i = 0; i < NUM_MUGSHOT_PARTS; i++)
            sFieldMugshotSpriteIds[slot][i] = value;
}
