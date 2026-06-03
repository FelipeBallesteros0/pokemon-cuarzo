#include "global.h"
#include "global.fieldmap.h"
#include "ui_start_menu.h"
#include "bg.h"
#include "international_string_util.h"
#include "menu.h"
#include "palette.h"
#include "sprite.h"
#include "text.h"
#include "window.h"
#include "constants/rgb.h"

// ─── Grid layout constants ─────────────────────────────────────────────────
// Button size: 64×32 px — gap between buttons: 8px (1 tile)
#define BTN_W       64
#define BTN_H       32
#define BTN_GAP     8
#define BTN_STEP_X  (BTN_W + BTN_GAP)  // 72 px per column step
#define BTN_STEP_Y  (BTN_H + BTN_GAP)  // 40 px per row step
// Horizontal centre for 3 columns: (240 - 3*72 + 8) / 2 = 16 px
#define GRID_X      16
// Vertical origin is computed dynamically in UIStartMenu_Load (sGridY)

// Text window (tile units, 1 tile = 8 px)
#define WIN_LEFT      2    // GRID_X / 8 = 2 tiles
#define WIN_WIDTH     26   // (3*BTN_STEP_X - BTN_GAP) / 8 = 26 tiles
#define WIN_BASEBLOCK 0x60 // Safe tile region below DLG_WINDOW_BASE_TILE_NUM

// ─── Sprite tags ───────────────────────────────────────────────────────────
#define TAG_BTN     0x9900
#define TAG_CURSOR  0x9901

// ─── Scroll BG constants ───────────────────────────────────────────────────
#define SCROLL_PAL_SLOT 13  // BG palette slot for scroll pattern
#define SCROLL_TILES    16  // tiles in 32×32 repeat unit (4×4 = 16)

// ─── EWRAM state ──────────────────────────────────────────────────────────
EWRAM_DATA static u8    sButtonSpriteIds[9];
EWRAM_DATA static u8    sCursorSpriteId;
EWRAM_DATA static u8    sTextWindowId;
EWRAM_DATA static bool8 sLoaded;
EWRAM_DATA static void *sSavedBg3TilemapBuf;
EWRAM_DATA static u8    sGridY;      // vertical origin, tile-aligned, computed from count
EWRAM_DATA static u8    sGridRows;   // number of rows in the active grid
EWRAM_DATA static u16   sPrevVisibleMask; // bit i set = object event i was visible before the menu hid it

// ─── Graphics data ─────────────────────────────────────────────────────────
static const u32 sScroll_Gfx[] = INCGFX_U32("graphics/start_menu/scroll_tile.png", ".4bpp");
static const u16 sScroll_Pal[] = INCGFX_U16("graphics/start_menu/scroll_tile.png", ".gbapal");

static const u16 sBoton_Pal[]  = INCGFX_U16("graphics/start_menu/boton.png",  ".gbapal");
static const u32 sBoton_Gfx[]  = INCGFX_U32("graphics/start_menu/boton.png",  ".4bpp");
static const u16 sCursor_Pal[] = INCGFX_U16("graphics/start_menu/cursor.png", ".gbapal");
static const u32 sCursor_Gfx[] = INCGFX_U32("graphics/start_menu/cursor.png", ".4bpp");

// ─── Sprite definitions ────────────────────────────────────────────────────
static const struct OamData sOamData_Btn =
{
    .size = SPRITE_SIZE(64x32), .shape = SPRITE_SHAPE(64x32), .priority = 2,
};
static const struct OamData sOamData_Cursor =
{
    .size = SPRITE_SIZE(64x32), .shape = SPRITE_SHAPE(64x32), .priority = 1,
};

static const struct SpriteSheet sSpriteSheet_Btn    = { sBoton_Gfx,  BTN_W * BTN_H / 2, TAG_BTN };
static const struct SpritePalette sSpritePal_Btn    = { sBoton_Pal,  TAG_BTN };
static const struct SpriteSheet sSpriteSheet_Cursor = { sCursor_Gfx, BTN_W * BTN_H / 2, TAG_CURSOR };
static const struct SpritePalette sSpritePal_Cursor = { sCursor_Pal, TAG_CURSOR };

static const union AnimCmd sSpriteAnim[] = { ANIMCMD_FRAME(0, 32), ANIMCMD_JUMP(0) };
static const union AnimCmd *const sSpriteAnimTable[] = { sSpriteAnim };

static const struct SpriteTemplate sSpriteTemplate_Btn =
{
    .tileTag = TAG_BTN, .paletteTag = TAG_BTN,
    .oam = &sOamData_Btn, .anims = sSpriteAnimTable,
    .images = NULL, .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};
static const struct SpriteTemplate sSpriteTemplate_Cursor =
{
    .tileTag = TAG_CURSOR, .paletteTag = TAG_CURSOR,
    .oam = &sOamData_Cursor, .anims = sSpriteAnimTable,
    .images = NULL, .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// ─── Overworld sprite helpers ──────────────────────────────────────────────

// Hides every overworld sprite while the menu is open, then restores ONLY the
// ones that were visible before. Forcing everything visible on restore would
// reveal objects that were intentionally invisible — e.g. a follower Pokémon
// hidden on the player's tile waiting for the first step, or NPCs hidden by a
// script — making them pop onto the player's square when closing the menu.
static void SetOverworldSpritesInvisible(bool8 invisible)
{
    u8 i;

    if (invisible)
        sPrevVisibleMask = 0;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (!gObjectEvents[i].active)
            continue;

        if (invisible)
        {
            if (!gObjectEvents[i].invisible)
                sPrevVisibleMask |= (1 << i);
        }
        else if (!(sPrevVisibleMask & (1 << i)))
        {
            // Was already invisible before the menu opened: leave it hidden.
            continue;
        }

        // Must set objectEvent->invisible too: UpdateObjectEventSpriteVisibility
        // runs each frame and first resets sprite->invisible=FALSE, then reads
        // objectEvent->invisible to decide if it should stay invisible.
        gObjectEvents[i].invisible = invisible;
        gSprites[gObjectEvents[i].spriteId].invisible = invisible;
    }
}

// ─── Scroll BG helpers ─────────────────────────────────────────────────────

static void SetupScrollBg(void)
{
    u16 *mapVram;
    u16 x, y;

    sSavedBg3TilemapBuf = GetBgTilemapBuffer(3);
    UnsetBgTilemapBuffer(3);

    SetBgAttribute(3, BG_ATTR_CHARBASEINDEX, 1);
    CpuCopy16(sScroll_Gfx, (void *)(BG_VRAM + BG_CHAR_SIZE), SCROLL_TILES * 32);
    LoadPalette(sScroll_Pal, BG_PLTT_ID(SCROLL_PAL_SLOT), PLTT_SIZE_4BPP);

    // Fill full 32×32 BG map (prevents garbage in rows the field scroll exposed)
    mapVram = (u16 *)(BG_VRAM + 30 * BG_SCREEN_SIZE);
    for (y = 0; y < 32; y++)
        for (x = 0; x < 32; x++)
            mapVram[y * 32 + x] = (u16)((y % 4) * 4 + (x % 4))
                                  | ((u16)SCROLL_PAL_SLOT << 12);

    ChangeBgX(3, 0, BG_COORD_SET);
    ChangeBgY(3, 0, BG_COORD_SET);
    ShowBg(3);
}

static void ClearScrollBg(void)
{
    SetBgAttribute(3, BG_ATTR_CHARBASEINDEX, 0);
    ShowBg(3);
    SetBgTilemapBuffer(3, sSavedBg3TilemapBuf);
    if (sSavedBg3TilemapBuf != NULL)
        CopyBgTilemapBufferToVram(3);
}

// ─── Public API ────────────────────────────────────────────────────────────

// count: number of menu items — used to vertically centre the grid.
void UIStartMenu_Load(u8 count)
{
    u8 i;
    u8 numRows;
    u16 totalHeight;
    struct WindowTemplate tmpl;

    for (i = 0; i < ARRAY_COUNT(sButtonSpriteIds); i++)
        sButtonSpriteIds[i] = MAX_SPRITES;
    sCursorSpriteId = MAX_SPRITES;
    sTextWindowId   = WINDOW_NONE;

    // Compute vertical centre (tile-aligned) based on active row count
    numRows    = (u8)((count + 2) / 3);
    sGridRows  = numRows;
    totalHeight = numRows * BTN_STEP_Y - BTN_GAP;   // px
    sGridY     = (u8)(((160 - totalHeight) / 2) & ~7u); // align to 8 px

    HideBg(1);
    HideBg(2);
    SetupScrollBg();
    SetOverworldSpritesInvisible(TRUE);
    LoadMessageBoxAndBorderGfx();

    LoadSpriteSheet(&sSpriteSheet_Btn);
    LoadSpritePalette(&sSpritePal_Btn);
    LoadSpriteSheet(&sSpriteSheet_Cursor);
    LoadSpritePalette(&sSpritePal_Cursor);

    // Build text window dynamically to match the computed grid position
    tmpl.bg         = 0;
    tmpl.tilemapLeft = WIN_LEFT;
    tmpl.tilemapTop  = sGridY / 8;
    tmpl.width       = WIN_WIDTH;
    tmpl.height      = (u8)(numRows * 5 - 1); // tiles: rows×5 minus last gap
    tmpl.paletteNum  = 15;
    tmpl.baseBlock   = WIN_BASEBLOCK;

    sTextWindowId = AddWindow(&tmpl);
    FillWindowPixelBuffer(sTextWindowId, PIXEL_FILL(0));
    sLoaded = TRUE;
}

void UIStartMenu_AddButton(u8 slot, const u8 *text)
{
    static const u8 colors[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};
    u8  col, row;
    u16 px, py, fontH;

    if (slot >= 9)
        return;

    col = slot % 3;
    row = slot / 3;

    if (sButtonSpriteIds[slot] == MAX_SPRITES)
        // CreateSprite takes center coords: add half-sprite to get correct screen top-left
        sButtonSpriteIds[slot] = CreateSprite(&sSpriteTemplate_Btn,
                                              GRID_X + col * BTN_STEP_X + BTN_W / 2,
                                              sGridY + row * BTN_STEP_Y + BTN_H / 2, 3);

    fontH = GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT);
    px = col * BTN_STEP_X + GetStringCenterAlignXOffset(FONT_NORMAL, text, BTN_W);
    // 26 = boton.png visible height after trimming 3px transparent top border
    py = row * BTN_STEP_Y + (26 - fontH) / 2;
    AddTextPrinterParameterized4(sTextWindowId, FONT_NORMAL,
                                 (u8)px, (u8)py, 0, 0,
                                 colors, TEXT_SKIP_DRAW, text);
}

void UIStartMenu_Finalize(void)
{
    PutWindowTilemap(sTextWindowId);
    CopyWindowToVram(sTextWindowId, COPYWIN_FULL);
}

void UIStartMenu_MoveCursor(u8 pos)
{
    u8  col = pos % 3;
    u8  row = pos / 3;
    // Center coords: +1 offset centers the cursor's 62x24 visible area within the 64x26 button
    s16 x   = (s16)(GRID_X + col * BTN_STEP_X + BTN_W / 2 + 1);
    s16 y   = (s16)(sGridY + row * BTN_STEP_Y + BTN_H / 2 + 1);

    if (sCursorSpriteId == MAX_SPRITES)
        sCursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, x, y, 2);
    else
    {
        gSprites[sCursorSpriteId].x = x;
        gSprites[sCursorSpriteId].y = y;
    }
}

// Animates the scroll background (call once per frame from the menu callback).
// Scrolls only vertically, bottom-to-top.
void UIStartMenu_ScrollBg(void)
{
    if (!sLoaded)
        return;
    ChangeBgY(3, 128, BG_COORD_ADD);
}

void UIStartMenu_Free(void)
{
    u8 i;

    if (!sLoaded)
        return;
    sLoaded = FALSE;

    if (sTextWindowId != WINDOW_NONE)
    {
        ClearWindowTilemap(sTextWindowId);
        CopyWindowToVram(sTextWindowId, COPYWIN_MAP);
        RemoveWindow(sTextWindowId);
        sTextWindowId = WINDOW_NONE;
    }
    for (i = 0; i < ARRAY_COUNT(sButtonSpriteIds); i++)
    {
        if (sButtonSpriteIds[i] != MAX_SPRITES)
        {
            DestroySprite(&gSprites[sButtonSpriteIds[i]]);
            sButtonSpriteIds[i] = MAX_SPRITES;
        }
    }
    if (sCursorSpriteId != MAX_SPRITES)
    {
        DestroySprite(&gSprites[sCursorSpriteId]);
        sCursorSpriteId = MAX_SPRITES;
    }
    FreeSpriteTilesByTag(TAG_BTN);
    FreeSpritePaletteByTag(TAG_BTN);
    FreeSpriteTilesByTag(TAG_CURSOR);
    FreeSpritePaletteByTag(TAG_CURSOR);

    SetOverworldSpritesInvisible(FALSE);
    ClearScrollBg();
    ShowBg(1);
    ShowBg(2);
}

// 2D grid navigation: moves pos by (dx,dy) skipping empty slots, wraps at edges.
u8 UIStartMenu_Navigate(u8 pos, s8 dx, s8 dy, u8 count)
{
    u8 col = pos % 3;
    u8 row = pos / 3;
    u8 i, new_pos;

    if (dx != 0)
    {
        for (i = 1; i <= 3; i++)
        {
            u8 new_col = (u8)((col + (s16)dx * i + 9) % 3);
            new_pos = row * 3 + new_col;
            if (new_pos < count)
                return new_pos;
        }
    }
    if (dy != 0)
    {
        for (i = 1; i <= 3; i++)
        {
            u8 new_row = (u8)((row + (s16)dy * i + 9) % 3);
            new_pos = new_row * 3 + col;
            if (new_pos < count)
                return new_pos;
        }
        return dy > 0 ? count - 1 : 0;
    }
    return pos;
}
