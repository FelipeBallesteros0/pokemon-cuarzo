#include "global.h"
#include "event_data.h"
#include "field_move.h"
#include "fldeff.h"
#include "fldeff_misc.h"
#include "party_menu.h"
#include "constants/field_move.h"
#include "constants/moves.h"
#include "constants/party_menu.h"

static bool32 IsFieldMoveUnlocked_Cut(void)
{
    if (IS_FRLG)
        return FlagGet(FLAG_BADGE02_GET);

    return FlagGet(FLAG_BADGE01_GET);
}

static bool32 IsFieldMoveUnlocked_Flash(void)
{
    if (IS_FRLG)
        return FlagGet(FLAG_BADGE01_GET);

    return FlagGet(FLAG_BADGE02_GET);
}

static bool32 IsFieldMoveUnlocked_RockSmash(void)
{
    if (IS_FRLG)
        return FlagGet(FLAG_BADGE06_GET);

    return FlagGet(FLAG_BADGE03_GET);
}

static bool32 IsFieldMoveUnlocked_Strength(void)
{
    return FlagGet(FLAG_BADGE04_GET);
}

static bool32 IsFieldMoveUnlocked_Surf(void)
{
    return FlagGet(FLAG_BADGE05_GET);
}

static bool32 IsFieldMoveUnlocked_Fly(void)
{
    if (IS_FRLG)
        return FlagGet(FLAG_BADGE03_GET);

    return FlagGet(FLAG_BADGE06_GET);
}

static bool32 IsFieldMoveUnlocked_Dive(void)
{
    return FlagGet(FLAG_BADGE07_GET);
}

static bool32 IsFieldMoveUnlocked_Waterfall(void)
{
    if (IS_FRLG)
        return FlagGet(FLAG_BADGE07_GET);

    return FlagGet(FLAG_BADGE08_GET);
}

#if OW_ROCK_CLIMB_FIELD_MOVE == TRUE
static bool32 IsFieldMoveUnlocked_RockClimb(void)
{
    return TRUE;
}
#endif

static bool32 IsFieldMoveUnlocked_Teleport(void)
{
    return TRUE;
}

static bool32 IsFieldMoveUnlocked_Dig(void)
{
    return TRUE;
}

static bool32 IsFieldMoveUnlocked_SecretPower(void)
{
    return TRUE;
}

static bool32 IsFieldMoveUnlocked_MilkDrink(void)
{
    return TRUE;
}

static bool32 IsFieldMoveUnlocked_SoftBoiled(void)
{
    return TRUE;
}

static bool32 IsFieldMoveUnlocked_SweetScent(void)
{
    return TRUE;
}

#if OW_DEFOG_FIELD_MOVE == TRUE
static bool32 IsFieldMoveUnlocked_Defog(void)
{
    return TRUE;
}
#endif

#if OW_HAZE_FIELD_MOVE == TRUE
static bool32 IsFieldMoveUnlocked_Haze(void)
{
    return TRUE;
}
#endif

#if OW_SUNNY_DAY_FIELD_MOVE == TRUE
static bool32 IsFieldMoveUnlocked_SunnyDay(void)
{
    return TRUE;
}
#endif

#if OW_RAIN_DANCE_FIELD_MOVE == TRUE
static bool32 IsFieldMoveUnlocked_RainDance(void)
{
    return TRUE;
}
#endif

#if OW_HAIL_FIELD_MOVE == TRUE
static bool32 IsFieldMoveUnlocked_Hail(void)
{
    return TRUE;
}
#endif

#if OW_SANDSTORM_FIELD_MOVE == TRUE
static bool32 IsFieldMoveUnlocked_Sandstorm(void)
{
    return TRUE;
}
#endif

#if OW_BOUNCE_FIELD_MOVE == TRUE
static bool32 IsFieldMoveUnlocked_Bounce(void)
{
    return TRUE;
}
#endif

const struct FieldMoveInfo gFieldMoveInfo[FIELD_MOVES_COUNT] =
{
    [FIELD_MOVE_CUT] =
    {
        .fieldMoveFunc = SetUpFieldMove_Cut,
        .isUnlockedFunc = IsFieldMoveUnlocked_Cut,
        .moveID = MOVE_CUT,
        .partyMsgID = PARTY_MSG_NOTHING_TO_CUT,
    },

    [FIELD_MOVE_FLASH] =
    {
        .fieldMoveFunc = SetUpFieldMove_Flash,
        .isUnlockedFunc = IsFieldMoveUnlocked_Flash,
        .moveID = MOVE_FLASH,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_ROCK_SMASH] =
    {
        .fieldMoveFunc = SetUpFieldMove_RockSmash,
        .isUnlockedFunc = IsFieldMoveUnlocked_RockSmash,
        .moveID = MOVE_ROCK_SMASH,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_STRENGTH] =
    {
        .fieldMoveFunc = SetUpFieldMove_Strength,
        .isUnlockedFunc = IsFieldMoveUnlocked_Strength,
        .moveID = MOVE_STRENGTH,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_SURF] =
    {
        .fieldMoveFunc = SetUpFieldMove_Surf,
        .isUnlockedFunc = IsFieldMoveUnlocked_Surf,
        .moveID = MOVE_SURF,
        .partyMsgID = PARTY_MSG_CANT_SURF_HERE,
    },

    [FIELD_MOVE_FLY] =
    {
        .fieldMoveFunc = SetUpFieldMove_Fly,
        .isUnlockedFunc = IsFieldMoveUnlocked_Fly,
        .moveID = MOVE_FLY,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_DIVE] =
    {
        .fieldMoveFunc = SetUpFieldMove_Dive,
        .isUnlockedFunc = IsFieldMoveUnlocked_Dive,
        .moveID = MOVE_DIVE,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_WATERFALL] =
    {
        .fieldMoveFunc = SetUpFieldMove_Waterfall,
        .isUnlockedFunc = IsFieldMoveUnlocked_Waterfall,
        .moveID = MOVE_WATERFALL,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_TELEPORT] =
    {
        .fieldMoveFunc = SetUpFieldMove_Teleport,
        .isUnlockedFunc = IsFieldMoveUnlocked_Teleport,
        .moveID = MOVE_TELEPORT,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_DIG] =
    {
        .fieldMoveFunc = SetUpFieldMove_Dig,
        .isUnlockedFunc = IsFieldMoveUnlocked_Dig,
        .moveID = MOVE_DIG,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_SECRET_POWER] =
    {
        .fieldMoveFunc = SetUpFieldMove_SecretPower,
        .isUnlockedFunc = IsFieldMoveUnlocked_SecretPower,
        .moveID = MOVE_SECRET_POWER,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },

    [FIELD_MOVE_MILK_DRINK] =
    {
        .fieldMoveFunc = SetUpFieldMove_SoftBoiled,
        .isUnlockedFunc = IsFieldMoveUnlocked_MilkDrink,
        .moveID = MOVE_MILK_DRINK,
        .partyMsgID = PARTY_MSG_NOT_ENOUGH_HP,
    },

    [FIELD_MOVE_SOFT_BOILED] =
    {
        .fieldMoveFunc = SetUpFieldMove_SoftBoiled,
        .isUnlockedFunc = IsFieldMoveUnlocked_SoftBoiled,
        .moveID = MOVE_SOFT_BOILED,
        .partyMsgID = PARTY_MSG_NOT_ENOUGH_HP,
    },

    [FIELD_MOVE_SWEET_SCENT] =
    {
        .fieldMoveFunc = SetUpFieldMove_SweetScent,
        .isUnlockedFunc = IsFieldMoveUnlocked_SweetScent,
        .moveID = MOVE_SWEET_SCENT,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#if OW_ROCK_CLIMB_FIELD_MOVE == TRUE
    [FIELD_MOVE_ROCK_CLIMB] =
    {
        .fieldMoveFunc = SetUpFieldMove_RockClimb,
        .isUnlockedFunc = IsFieldMoveUnlocked_RockClimb,
        .moveID = MOVE_ROCK_CLIMB,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#endif
#if OW_DEFOG_FIELD_MOVE == TRUE
    [FIELD_MOVE_DEFOG] =
    {
        .fieldMoveFunc = SetUpFieldMove_Defog,
        .isUnlockedFunc = IsFieldMoveUnlocked_Defog,
        .moveID = MOVE_DEFOG,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#endif
#if OW_HAZE_FIELD_MOVE == TRUE
    [FIELD_MOVE_HAZE] =
    {
        .fieldMoveFunc = SetUpFieldMove_Haze,
        .isUnlockedFunc = IsFieldMoveUnlocked_Haze,
        .moveID = MOVE_HAZE,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#endif
#if OW_SUNNY_DAY_FIELD_MOVE == TRUE
    [FIELD_MOVE_SUNNY_DAY] =
    {
        .fieldMoveFunc = SetUpFieldMove_SunnyDay,
        .isUnlockedFunc = IsFieldMoveUnlocked_SunnyDay,
        .moveID = MOVE_SUNNY_DAY,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#endif
#if OW_RAIN_DANCE_FIELD_MOVE == TRUE
    [FIELD_MOVE_RAIN_DANCE] =
    {
        .fieldMoveFunc = SetUpFieldMove_RainDance,
        .isUnlockedFunc = IsFieldMoveUnlocked_RainDance,
        .moveID = MOVE_RAIN_DANCE,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#endif
#if OW_HAIL_FIELD_MOVE == TRUE
    [FIELD_MOVE_HAIL] =
    {
        .fieldMoveFunc = SetUpFieldMove_Hail,
        .isUnlockedFunc = IsFieldMoveUnlocked_Hail,
        .moveID = MOVE_HAIL,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#endif
#if OW_SANDSTORM_FIELD_MOVE == TRUE
    [FIELD_MOVE_SANDSTORM] =
    {
        .fieldMoveFunc = SetUpFieldMove_Sandstorm,
        .isUnlockedFunc = IsFieldMoveUnlocked_Sandstorm,
        .moveID = MOVE_SANDSTORM,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#endif
#if OW_BOUNCE_FIELD_MOVE == TRUE
    [FIELD_MOVE_BOUNCE] =
    {
        .fieldMoveFunc = SetUpFieldMove_Bounce,
        .isUnlockedFunc = IsFieldMoveUnlocked_Bounce,
        .moveID = MOVE_BOUNCE,
        .partyMsgID = PARTY_MSG_CANT_USE_HERE,
    },
#endif
};
