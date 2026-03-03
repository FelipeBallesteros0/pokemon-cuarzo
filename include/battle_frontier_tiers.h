#ifndef GUARD_BATTLE_FRONTIER_TIERS_H
#define GUARD_BATTLE_FRONTIER_TIERS_H

#include "global.h"
#include "battle_tower.h"

enum
{
    FACTORY_TIER_OU,
    FACTORY_TIER_UU,
    FACTORY_TIER_RU,
    FACTORY_TIER_NU,
    FACTORY_TIER_PU,
    FACTORY_TIER_UBERS,
    FACTORY_TIER_COUNT
};

extern const struct TrainerMon *const gFactoryTierMons[FACTORY_TIER_COUNT];
extern const u16 gFactoryTierMonsCount[FACTORY_TIER_COUNT];

u16 FactoryTier_GetRandomMonSetId(u8 tier);
u16 BattleFactory_GetValidSetIdFromTier(u16 tier);
const u16 *BattleFactory_GetTierSetIdList(u16 tier, u16 *count);
u16 BattleFactory_GetValidSetIdFromTierList(const u16 *list, u16 count);

#endif
