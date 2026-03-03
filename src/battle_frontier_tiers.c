#include "global.h"
#include "random.h"
#include "battle_tower.h"
#include "battle_frontier_tiers.h"
#include "bf_banned_species.h"

// Estos headers DEFINEN los arrays y sus counts (una sola vez aquí)
#include "data/battle_frontier/tiers/mons_ou.h"
#include "data/battle_frontier/tiers/mons_uu.h"
#include "data/battle_frontier/tiers/mons_ru.h"
#include "data/battle_frontier/tiers/mons_nu.h"
#include "data/battle_frontier/tiers/mons_pu.h"
#include "data/battle_frontier/tiers/mons_ubers.h"
#include "data/battle_frontier/tiers/mons_ou_norm.h"
#include "data/battle_frontier/tiers/mons_uu_norm.h"
#include "data/battle_frontier/tiers/mons_ru_norm.h"
#include "data/battle_frontier/tiers/mons_nu_norm.h"
#include "data/battle_frontier/tiers/mons_pu_norm.h"
#include "data/battle_frontier/tiers/mons_ubers_norm.h"

#include "constants/species.h"

#include "constants/moves.h"
#include "move.h"

const struct TrainerMon *const gFactoryTierMons[FACTORY_TIER_COUNT] =
{
    [FACTORY_TIER_OU]    = gBattleFrontierMons_OU,
    [FACTORY_TIER_UU]    = gBattleFrontierMons_UU,
    [FACTORY_TIER_RU]    = gBattleFrontierMons_RU,
    [FACTORY_TIER_NU]    = gBattleFrontierMons_NU,
    [FACTORY_TIER_PU]    = gBattleFrontierMons_PU,
    [FACTORY_TIER_UBERS] = gBattleFrontierMons_UBERS,
};

const u16 gFactoryTierMonsCount[FACTORY_TIER_COUNT] =
{
    [FACTORY_TIER_OU]    = gBattleFrontierMonsCount_OU,
    [FACTORY_TIER_UU]    = gBattleFrontierMonsCount_UU,
    [FACTORY_TIER_RU]    = gBattleFrontierMonsCount_RU,
    [FACTORY_TIER_NU]    = gBattleFrontierMonsCount_NU,
    [FACTORY_TIER_PU]    = gBattleFrontierMonsCount_PU,
    [FACTORY_TIER_UBERS] = gBattleFrontierMonsCount_UBERS,
};

u16 BattleFactory_GetValidSetIdFromTierList(const u16 *list, u16 count);




// Devuelve el tier-list (setIds o species) + count para el Battle Factory
const u16 *BattleFactory_GetTierSetIdList(u16 tier, u16 *count)
{
    const u16 *list = gFactoryTierMonSetIds_OU;
    u16 c = ARRAY_COUNT(gFactoryTierMonSetIds_OU);

    switch (tier)
    {
    case 0: list = gFactoryTierMonSetIds_OU;    c = ARRAY_COUNT(gFactoryTierMonSetIds_OU);    break;
    case 1: list = gFactoryTierMonSetIds_UU;    c = ARRAY_COUNT(gFactoryTierMonSetIds_UU);    break;
    case 2: list = gFactoryTierMonSetIds_RU;    c = ARRAY_COUNT(gFactoryTierMonSetIds_RU);    break;
    case 3: list = gFactoryTierMonSetIds_NU;    c = ARRAY_COUNT(gFactoryTierMonSetIds_NU);    break;
    case 4: list = gFactoryTierMonSetIds_PU;    c = ARRAY_COUNT(gFactoryTierMonSetIds_PU);    break;
    case 5: list = gFactoryTierMonSetIds_UBERS; c = ARRAY_COUNT(gFactoryTierMonSetIds_UBERS); break;
    default: break;
    }

    if (count)
        *count = c;
    return list;
}

// mapping: 0=OU,1=UU,2=RU,3=NU,4=PU,5=UBERS
u16 FactoryTier_GetRandomMonSetId(u8 tier)
{
    const u16 *list = gFactoryTierMonSetIds_OU;
    u16 count = ARRAY_COUNT(gFactoryTierMonSetIds_OU);

    switch (tier)
    {
    case 0: list = gFactoryTierMonSetIds_OU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_OU);    break;
    case 1: list = gFactoryTierMonSetIds_UU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_UU);    break;
    case 2: list = gFactoryTierMonSetIds_RU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_RU);    break;
    case 3: list = gFactoryTierMonSetIds_NU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_NU);    break;
    case 4: list = gFactoryTierMonSetIds_PU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_PU);    break;
    case 5: list = gFactoryTierMonSetIds_UBERS; count = ARRAY_COUNT(gFactoryTierMonSetIds_UBERS); break;
    default: break;
    }

    if (count == 0)
        return 0;

    return BattleFactory_GetValidSetIdFromTierList(list, count);
}
// --- BF tier pick sanitizer (evita SPECIES_NONE / ids fuera de rango / MOVE_NONE / especies baneadas) ---

#if defined(MOVES_COUNT)
#define BF_NUM_MOVES MOVES_COUNT
#elif defined(NUM_MOVES)
#define BF_NUM_MOVES NUM_MOVES
#else
#define BF_NUM_MOVES 0
#endif
static bool8 BF_IsValidSetId(u16 setId)
{
    if (gBattleFrontierMonsCount == 0)
        return FALSE;

    if (setId >= gBattleFrontierMonsCount)
        return FALSE;

    if (gBattleFrontierMons[setId].species == SPECIES_NONE)
        return FALSE;

    if (gBattleFrontierMons[setId].species >= NUM_SPECIES)
        return FALSE;

    if (!IsSpeciesEnabled(gBattleFrontierMons[setId].species))
        return FALSE;
    if (IsBFBannedSpecies(gBattleFrontierMons[setId].species))
        return FALSE;
    if (gSpeciesInfo[gBattleFrontierMons[setId].species].speciesName[0] == 0)
        return FALSE;

    if (gBattleFrontierMons[setId].species == SPECIES_UNOWN)
        return FALSE;

    for (u32 i = 0; i < 4; i++)
    {
        u16 mv = gBattleFrontierMons[setId].moves[i];
        if (mv == MOVE_NONE)
            return FALSE;
#if BF_NUM_MOVES
        if (mv >= BF_NUM_MOVES)
            return FALSE;
#endif
        if (mv >= MOVES_COUNT_ALL)
            return FALSE;
        if (gMovesInfo[mv].name[0] == 0 || gMovesInfo[mv].pp == 0)
            return FALSE;
    }

    return TRUE;
}
// Esta es la función que vamos a usar desde battle_factory.c

u16 BattleFactory_GetValidSetIdFromTierList(const u16 *list, u16 count)
{
    if (list == NULL || count == 0 || gBattleFrontierMonsCount == 0)
        goto fallback_global;

    // 1) Preferir siempre el tier list: entries son setIds globales
    u32 found = 0;
    u16 chosen = 0;
    for (u32 i = 0; i < count; i++)
    {
        u16 entry = list[i];
        if (BF_IsValidSetId(entry))
        {
            found++;
            if ((Random() % found) == 0)
                chosen = entry;
        }
    }

    if (found > 0)
        return chosen;

fallback_global:
    // 2) Fallback: cualquier set válido global (para evitar "--" / basura)
    for (u32 tries = 0; tries < 8192; tries++)
    {
        u16 id = Random() % gBattleFrontierMonsCount;
        if (BF_IsValidSetId(id))
            return id;
    }

    return 0;
}

u16 BattleFactory_GetValidSetIdFromTier(u16 tier)
{
    // mapping: 0=OU,1=UU,2=RU,3=NU,4=PU,5=UBERS
    const u16 *list = gFactoryTierMonSetIds_OU;
    u16 count = ARRAY_COUNT(gFactoryTierMonSetIds_OU);

    switch (tier)
    {
    default:
    case 0: list = gFactoryTierMonSetIds_OU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_OU);    break;
    case 1: list = gFactoryTierMonSetIds_UU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_UU);    break;
    case 2: list = gFactoryTierMonSetIds_RU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_RU);    break;
    case 3: list = gFactoryTierMonSetIds_NU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_NU);    break;
    case 4: list = gFactoryTierMonSetIds_PU;    count = ARRAY_COUNT(gFactoryTierMonSetIds_PU);    break;
    case 5: list = gFactoryTierMonSetIds_UBERS; count = ARRAY_COUNT(gFactoryTierMonSetIds_UBERS); break;
    }

    return BattleFactory_GetValidSetIdFromTierList(list, count);
}
