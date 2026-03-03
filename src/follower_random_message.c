#include "global.h"
#include "event_data.h"
#include "event_scripts.h"
#include "follower_friendship.h"
#include "follower_weather.h"
#include "script.h"
#include "random.h"
#include "constants/vars.h"
#include "data/text/follower_generic_messages.h"

#define FOLLOWER_MESSAGE_COOLDOWN_STEPS 120

static const u8 *GetRandomText(const u8 *const *texts, u8 count)
{
    if (texts == NULL || count == 0)
        return NULL;
    return texts[Random() % count];
}

bool8 TryShowFollowerGenericMessage(struct ScriptContext *ctx)
{
    const u8 *text = GetRandomText(sFollowerGenericMessages, ARRAY_COUNT(sFollowerGenericMessages));
    if (text == NULL)
        return FALSE;
    ctx->data[0] = (u32)text;
    ScriptCall(ctx, EventScript_FollowerGeneric);
    return TRUE;
}

static void ShuffleTwo(u8 *a, u8 *b)
{
    if (Random() & 1)
    {
        u8 tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

bool8 TryShowFollowerRandomMessage(struct ScriptContext *ctx)
{
    // 0: weather, 1: friendship, 2: generic
    u8 roll = Random() % 100;
    u8 first = (roll < 40) ? 0 : (roll < 80) ? 1 : 2;
    u8 second, third;
    if (first == 0)
    {
        second = 1;
        third = 2;
    }
    else if (first == 1)
    {
        second = 0;
        third = 2;
    }
    else
    {
        second = 0;
        third = 1;
    }

    // Shuffle remaining two
    ShuffleTwo(&second, &third);

    for (u8 i = 0; i < 3; i++)
    {
        u8 cat = (i == 0) ? first : (i == 1) ? second : third;
        bool8 ok = FALSE;
        if (cat == 0)
            ok = TryShowFollowerWeatherReaction(ctx);
        else if (cat == 1)
            ok = TryShowFollowerFriendshipMessage(ctx);
        else
            ok = TryShowFollowerGenericMessage(ctx);

        if (ok)
            return TRUE;
    }

    return FALSE;
}
