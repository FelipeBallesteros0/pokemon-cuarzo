#include "global.h"
#include "event_data.h"
#include "event_scripts.h"
#include "follower_friendship.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "script.h"
#include "constants/vars.h"
#include "data/text/follower_friendship_messages.h"

#define FOLLOWER_FRIENDSHIP_COOLDOWN_STEPS 120

static struct Pokemon *GetFollowerPartyMonForFriendship(void)
{
    struct Pokemon *mon = &gPlayerParty[0];
    u32 species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
    if (species == SPECIES_NONE)
        return NULL;
    if (mon->box.isEgg || mon->box.isBadEgg)
        return NULL;
    return mon;
}

static const u8 *GetRandomText(const u8 *const *texts, u8 count)
{
    if (count == 0 || texts == NULL)
        return NULL;
    return texts[Random() % count];
}

static const u8 *GetFriendshipText(u8 friendship)
{
    if (friendship <= 99)
        return GetRandomText(sFollowerFriendshipLow, ARRAY_COUNT(sFollowerFriendshipLow));
    else if (friendship <= 199)
        return GetRandomText(sFollowerFriendshipMid, ARRAY_COUNT(sFollowerFriendshipMid));
    else
        return GetRandomText(sFollowerFriendshipHigh, ARRAY_COUNT(sFollowerFriendshipHigh));
}

bool8 TryShowFollowerFriendshipMessage(struct ScriptContext *ctx)
{
    struct Pokemon *mon = GetFollowerPartyMonForFriendship();
    if (mon == NULL)
        return FALSE;

    u8 friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    const u8 *text = GetFriendshipText(friendship);
    if (text == NULL)
        return FALSE;
    ctx->data[0] = (u32)text;
    ScriptCall(ctx, EventScript_FollowerGeneric);
    return TRUE;
}
