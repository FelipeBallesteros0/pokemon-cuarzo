#include "global.h"
#include "event_object_movement.h"
#include "field_effect_helpers.h"
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

static void LoadSurfablePalette(struct Pokemon *mon, u16 surfIdx)
{
    if (IsMonShiny(mon))
        LoadSpritePalette(&sSurfablePokemonShinyPalettes[surfIdx]);
    else
        LoadSpritePalette(&sSurfablePokemonPalettes[surfIdx]);
}

// Required by gSurfablePokemonOverlaySprites table, but we don't use overlays.
static void UpdateSurfMonOverlay(struct Sprite *sprite)
{
    DestroySprite(sprite);
}

bool8 TryCreateSurfablePokemonSprite(u8 partyIndex, u8 playerObjId, s16 x, s16 y, u8 subpriority, u8 *spriteIdOut)
{
    struct Pokemon *mon;
    u16 species;
    u16 surfIdx;
    u8 spriteId;

    if (partyIndex >= PARTY_SIZE)
        return FALSE;

    mon = &gPlayerParty[partyIndex];
    species = GetMonData(mon, MON_DATA_SPECIES);
    if (species == SPECIES_NONE || species == SPECIES_EGG)
        return FALSE;

    surfIdx = GetSurfableSpeciesIndex(species);
    if (surfIdx == 0xFFFF)
        return FALSE;

    LoadSurfablePalette(mon, surfIdx);
    spriteId = CreateSpriteAtEnd(&gSurfablePokemonOverworldSprites[surfIdx], x, y, subpriority);
    if (spriteId == MAX_SPRITES)
        return FALSE;

    gSprites[spriteId].coordOffsetEnabled = TRUE;
    gSprites[spriteId].data[2] = playerObjId;
    gSprites[spriteId].data[3] = -1;
    gSprites[spriteId].data[6] = -1;
    gSprites[spriteId].data[7] = -1;

    if (spriteIdOut != NULL)
        *spriteIdOut = spriteId;

    return TRUE;
}
