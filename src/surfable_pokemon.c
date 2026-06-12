#include "global.h"
#include "event_object_movement.h"
#include "field_effect_helpers.h"
#include "field_weather.h"
#include "field_player_avatar.h"
#include "pokemon.h"
#include "sprite.h"
#include "constants/species.h"
#include "surfable_pokemon.h"

struct RideablePokemon
{
    u16 species;
    u8 trainerPose;
};

static void UpdateSurfMonOverlay(struct Sprite *sprite);

#include "data/surfable_pokemon/surfable_pokemon.h"
#include "data/surfable_pokemon/surfable_pokemon_graphics.h"
#include "data/surfable_pokemon/surfable_pokemon_pic_tables.h"
#include "data/surfable_pokemon/surfable_pokemon_templates.h"

static u16 GetSurfableSpeciesIndex(u16 species)
{
    u16 i;

    for (i = 0; i < NELEMS(gSurfablePokemon); i++)
    {
        if (gSurfablePokemon[i].species == species)
            return i;
    }

    return 0xFFFF;
}

static void LoadSurfablePalette(bool8 shiny, u16 surfIdx)
{
    u8 palSlot;

    if (shiny)
        palSlot = LoadSpritePalette(&sSurfablePokemonShinyPalettes[surfIdx]);
    else
        palSlot = LoadSpritePalette(&sSurfablePokemonPalettes[surfIdx]);

    if (palSlot != 0xFF)
    {
        UpdateSpritePaletteWithWeather(palSlot, FALSE);
        UpdateSpritePaletteWithTime(palSlot);
    }
}

// Required by gSurfablePokemonOverlaySprites table, but we don't use overlays.
static void UpdateSurfMonOverlay(struct Sprite *sprite)
{
    DestroySprite(sprite);
}

// Creates a surfable mount sprite for any species, bound to the object event
// (player or NPC) whose id is `objId`. Returns FALSE if the species has no
// dedicated surf sprite.
bool8 TryCreateSurfableSpeciesSprite(u16 species, bool8 shiny, u8 objId, s16 x, s16 y, u8 subpriority, u8 *spriteIdOut)
{
    u16 surfIdx;
    u8 spriteId;

    if (species == SPECIES_NONE || species == SPECIES_EGG)
        return FALSE;

    surfIdx = GetSurfableSpeciesIndex(species);
    if (surfIdx == 0xFFFF)
        return FALSE;

    LoadSurfablePalette(shiny, surfIdx);
    spriteId = CreateSpriteAtEnd(&gSurfablePokemonOverworldSprites[surfIdx], x, y, subpriority);
    if (spriteId == MAX_SPRITES)
        return FALSE;

    gSprites[spriteId].coordOffsetEnabled = TRUE;
    gSprites[spriteId].data[2] = objId;
    gSprites[spriteId].data[3] = -1;
    gSprites[spriteId].data[6] = -1;
    gSprites[spriteId].data[7] = -1;

    if (spriteIdOut != NULL)
        *spriteIdOut = spriteId;

    return TRUE;
}

bool8 TryCreateSurfablePokemonSprite(u8 partyIndex, u8 playerObjId, s16 x, s16 y, u8 subpriority, u8 *spriteIdOut)
{
    struct Pokemon *mon;

    if (partyIndex >= PARTY_SIZE)
        return FALSE;

    mon = &gPlayerParty[partyIndex];
    return TryCreateSurfableSpeciesSprite(GetMonData(mon, MON_DATA_SPECIES), IsMonShiny(mon),
                                          playerObjId, x, y, subpriority, spriteIdOut);
}
