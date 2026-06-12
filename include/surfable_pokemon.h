#ifndef GUARD_SURFABLE_POKEMON_H
#define GUARD_SURFABLE_POKEMON_H

#include "global.h"

bool8 TryCreateSurfablePokemonSprite(u8 partyIndex, u8 playerObjId, s16 x, s16 y, u8 subpriority, u8 *spriteIdOut);
bool8 TryCreateSurfableSpeciesSprite(u16 species, bool8 shiny, u8 objId, s16 x, s16 y, u8 subpriority, u8 *spriteIdOut);

#endif // GUARD_SURFABLE_POKEMON_H
