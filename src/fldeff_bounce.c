#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "fldeff.h"
#include "overworld.h"
#include "party_menu.h"
#include "pokemon.h"
#include "script.h"
#include "script_movement.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/field_move.h"
#include "constants/flags.h"
#include "constants/songs.h"

// Out-of-battle "Bounce" (Rebote): the player jumps over a formation of tall
// "high rocks" in one continuous arc, landing on the far side. Two activators:
//   A) interacting with a rock (script specials), and
//   B) the Bounce field move from the party menu (SetUpFieldMove_Bounce).
// Both share the same generic landing detection and jump task below, so this is
// not tied to any specific map coordinates.

// How far ahead (tiles) to look for the far edge of a rock formation.
#define BOUNCE_SCAN_RANGE 8

// Per-direction step, indexed by enum Direction (NONE=0, SOUTH, NORTH, WEST, EAST).
static const s8 sBounceDeltaX[] = {
    [DIR_SOUTH] = 0, [DIR_NORTH] = 0, [DIR_WEST] = -1, [DIR_EAST] = 1,
};
static const s8 sBounceDeltaY[] = {
    [DIR_SOUTH] = 1, [DIR_NORTH] = -1, [DIR_WEST] = 0, [DIR_EAST] = 0,
};

static const u8 sMovement_BounceJump[] = {
    MOVEMENT_ACTION_BOUNCE_JUMP,
    MOVEMENT_ACTION_STEP_END,
};

static void Task_BounceJump(u8 taskId);
static void FieldCallback_Bounce(void);

static bool8 IsHighRockAt(s16 x, s16 y)
{
    u8 id = GetObjectEventIdByXY(x, y);

    return (id != OBJECT_EVENTS_COUNT
         && gObjectEvents[id].active
         && gObjectEvents[id].graphicsId == OBJ_EVENT_GFX_HIGH_BOULDER);
}

// If the player faces a high rock, find the landing tile just past the far edge
// of the whole formation (scanning over consecutive rocks and any gaps between
// them). Returns FALSE if there is no rock in front or no valid landing tile.
// Coords are in object-event space (map grid + MAP_OFFSET), same as the helpers.
static bool8 GetBounceDestination(s16 *destX, s16 *destY)
{
    enum Direction dir = GetPlayerFacingDirection();
    s16 x, y, lastRockX, lastRockY;
    s8 dx, dy;
    bool8 sawRock = FALSE;
    u8 i;

    if (dir != DIR_SOUTH && dir != DIR_NORTH && dir != DIR_WEST && dir != DIR_EAST)
        return FALSE;

    dx = sBounceDeltaX[dir];
    dy = sBounceDeltaY[dir];

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    if (!IsHighRockAt(x, y))
        return FALSE;

    lastRockX = x;
    lastRockY = y;
    for (i = 0; i < BOUNCE_SCAN_RANGE; i++, x += dx, y += dy)
    {
        if (IsHighRockAt(x, y))
        {
            sawRock = TRUE;
            lastRockX = x;
            lastRockY = y;
        }
    }
    if (!sawRock)
        return FALSE;

    // Landing is the first tile past the last rock; it must be walkable and empty.
    x = lastRockX + dx;
    y = lastRockY + dy;
    if (MapGridGetCollisionAt(x, y) != 0)
        return FALSE;
    if (GetObjectEventIdByXY(x, y) != OBJECT_EVENTS_COUNT)
        return FALSE;

    *destX = x;
    *destY = y;
    return TRUE;
}

#define tState      data[0]
#define tTiles      data[1]
#define tFromScript data[2]

// Drives one bounce: hide the follower, run the BOUNCE_JUMP movement action on the
// player via the script-movement system (works even while frozen by lockall), then
// restore the follower and unlock on landing.
static void StartBounceJump(s16 destX, s16 destY, bool8 fromScript)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    s16 dx = destX - player->currentCoords.x;
    s16 dy = destY - player->currentCoords.y;
    s16 tiles = (dx != 0) ? (dx < 0 ? -dx : dx) : (dy < 0 ? -dy : dy);
    u8 taskId = CreateTask(Task_BounceJump, 0);

    gTasks[taskId].tState = 0;
    gTasks[taskId].tTiles = tiles;
    gTasks[taskId].tFromScript = fromScript;
}

static void Task_BounceJump(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 mapNum = gSaveBlock1Ptr->location.mapNum;
    u8 mapGroup = gSaveBlock1Ptr->location.mapGroup;

    switch (tState)
    {
    case 0:
        LockPlayerFieldControls();
        // Hide the follower for the flight; it would copy a broken short jump and
        // could end up stranded on the wrong side of the rocks.
        FlagSet(FLAG_TEMP_HIDE_FOLLOWER);
        RemoveFollowingPokemon();
        PlaySE(SE_LEDGE);
        SetBounceJumpTiles(tTiles);
        ScriptMovement_StartObjectMovementScript(LOCALID_PLAYER, mapNum, mapGroup, sMovement_BounceJump);
        tState = 1;
        break;
    case 1:
        if (ScriptMovement_IsObjectMovementFinished(LOCALID_PLAYER, mapNum, mapGroup))
            tState = 2;
        break;
    case 2:
        // Respawn the follower next to the player at the landing tile.
        FlagClear(FLAG_TEMP_HIDE_FOLLOWER);
        UpdateFollowingPokemon();
        UnlockPlayerFieldControls();
        if (tFromScript)
            ScriptContext_Enable(); // resume the rock script past `waitstate`
        DestroyTask(taskId);
        break;
    }
}

#undef tState
#undef tTiles
#undef tFromScript

// ---- Activator B: party-menu field move (mirrors Surf / Rock Climb) ----
static void FieldCallback_Bounce(void)
{
    s16 destX, destY;

    if (GetBounceDestination(&destX, &destY))
        StartBounceJump(destX, destY, FALSE);
    else
        ScriptContext_Enable();
}

bool32 SetUpFieldMove_Bounce(void)
{
    s16 destX, destY;

    if (GetBounceDestination(&destX, &destY))
    {
        gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_Bounce;
        return TRUE;
    }
    return FALSE;
}

// ---- Activator A: rock interaction (script specials) ----
u16 Special_GetFirstBounceMonIndex(void)
{
    u8 i;
    u8 partyCount = CalculatePlayerPartyCount();

    for (i = 0; i < partyCount; i++)
    {
        if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG, NULL)
         && MonKnowsMove(&gPlayerParty[i], MOVE_BOUNCE))
            return i;
    }
    return PARTY_SIZE;
}

bool8 Special_CanBounceHere(void)
{
    s16 destX, destY;

    return (Special_GetFirstBounceMonIndex() != PARTY_SIZE)
        && GetBounceDestination(&destX, &destY);
}

void Special_StartBounceJump(void)
{
    s16 destX, destY;

    if (GetBounceDestination(&destX, &destY))
        StartBounceJump(destX, destY, TRUE);
    else
        ScriptContext_Enable(); // don't hang the script's waitstate
}

// Teaches Bounce (last move slot) to the most-recently-added party mon. Used by the
// test NPC right after `givemon SPECIES_PONYTA` so the Ponyta can use Bounce.
void Special_TeachLastMonBounce(void)
{
    u8 count = CalculatePlayerPartyCount();

    if (count > 0)
        SetMonMoveSlot(&gPlayerParty[count - 1], MOVE_BOUNCE, MAX_MON_MOVES - 1);
}
