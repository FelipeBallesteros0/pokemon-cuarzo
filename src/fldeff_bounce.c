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

// Out-of-battle "Bounce" (Rebote): the player climbs onto the high rock in
// front, arcs over to the far rock of the formation and hops down one tile
// past it. If the player faces the rock from a direction perpendicular to the
// formation's axis, the crossing still runs along the axis. The follower
// Pokémon is recalled into its ball (animated) before the jump.
// Two activators:
//   A) interacting with a rock (script specials), and
//   B) the Bounce field move from the party menu (SetUpFieldMove_Bounce).

// How far ahead (tiles) to look for the far edge of a rock formation.
#define BOUNCE_SCAN_RANGE 8

// Route of one bounce: hop onto the first rock (climbDir), arc crossTiles to
// the formation's last rock (jumpDir), then hop down 1 more tile in jumpDir.
struct BounceRoute
{
    u8 climbDir;
    u8 jumpDir;
    u8 crossTiles; // 0 = isolated rock, no crossing
};

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

static const u8 sMovement_FollowerRecall[] = {
    MOVEMENT_ACTION_ENTER_POKEBALL,
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

// Try to resolve the crossing from the climbed rock in direction dir: find the
// farthest rock within range (scanning over consecutive rocks and any gaps),
// then validate the landing tile one step past it. With requireCross, fail if
// there is no second rock to cross to in this direction.
// Coords are in object-event space (map grid + MAP_OFFSET), same as the helpers.
static bool8 TryBounceAxis(s16 rockX, s16 rockY, u8 dir, bool8 requireCross, struct BounceRoute *route)
{
    s8 dx = sBounceDeltaX[dir];
    s8 dy = sBounceDeltaY[dir];
    s16 x = rockX + dx;
    s16 y = rockY + dy;
    s16 lastX = rockX, lastY = rockY;
    u8 tiles = 0;
    u8 i;

    for (i = 1; i <= BOUNCE_SCAN_RANGE; i++, x += dx, y += dy)
    {
        if (IsHighRockAt(x, y))
        {
            lastX = x;
            lastY = y;
            tiles = i;
        }
    }
    if (requireCross && tiles == 0)
        return FALSE;

    // Landing is the first tile past the last rock; it must be walkable and empty.
    x = lastX + dx;
    y = lastY + dy;
    if (MapGridGetCollisionAt(x, y) != 0)
        return FALSE;
    if (GetObjectEventIdByXY(x, y) != OBJECT_EVENTS_COUNT)
        return FALSE;

    route->jumpDir = dir;
    route->crossTiles = tiles;
    return TRUE;
}

// If the player faces a high rock, work out the full route. The crossing
// direction is searched facing-first, then back over the player's head (so
// using Bounce from inside a gap of the formation still crosses the whole
// formation), then sideways (so approaching perpendicular to the axis works
// too). Returns FALSE if there is no rock in front or no valid landing tile
// in any candidate direction.
static bool8 GetBounceRoute(struct BounceRoute *route)
{
    enum Direction dir = GetPlayerFacingDirection();
    s16 x, y;
    u8 i;
    u8 candidates[4];

    if (dir != DIR_SOUTH && dir != DIR_NORTH && dir != DIR_WEST && dir != DIR_EAST)
        return FALSE;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    if (!IsHighRockAt(x, y))
        return FALSE;

    route->climbDir = dir;
    candidates[0] = dir;
    candidates[1] = GetOppositeDirection(dir);
    if (dir == DIR_EAST || dir == DIR_WEST)
    {
        candidates[2] = DIR_NORTH;
        candidates[3] = DIR_SOUTH;
    }
    else
    {
        candidates[2] = DIR_WEST;
        candidates[3] = DIR_EAST;
    }

    // Prefer an axis with a far rock to cross to...
    for (i = 0; i < ARRAY_COUNT(candidates); i++)
    {
        if (TryBounceAxis(x, y, candidates[i], TRUE, route))
            return TRUE;
    }
    // ...otherwise it's an isolated rock: a simple hop over it, facing forward.
    return TryBounceAxis(x, y, dir, FALSE, route);
}

#define tState      data[0]
#define tClimbDir   data[1]
#define tJumpDir    data[2]
#define tCrossTiles data[3]
#define tFromScript data[4]

static void StartBounceJump(const struct BounceRoute *route, bool8 fromScript)
{
    u8 taskId = CreateTask(Task_BounceJump, 0);

    gTasks[taskId].tState = 0;
    gTasks[taskId].tClimbDir = route->climbDir;
    gTasks[taskId].tJumpDir = route->jumpDir;
    gTasks[taskId].tCrossTiles = route->crossTiles;
    gTasks[taskId].tFromScript = fromScript;
}

// Drives one bounce: recall the follower into its ball (animated), run the
// multi-phase BOUNCE_JUMP movement action on the player via the script-movement
// system (works even while frozen by lockall), then restore the follower and
// unlock on landing.
static void Task_BounceJump(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 mapNum = gSaveBlock1Ptr->location.mapNum;
    u8 mapGroup = gSaveBlock1Ptr->location.mapGroup;

    switch (tState)
    {
    case 0: // if the follower is out, play its enter-ball animation first
    {
        struct ObjectEvent *follower = GetFollowerObject();

        LockPlayerFieldControls();
        if (follower != NULL && !follower->invisible)
        {
            PlaySE(SE_BALL);
            ScriptMovement_StartObjectMovementScript(OBJ_EVENT_ID_FOLLOWER, mapNum, mapGroup, sMovement_FollowerRecall);
            tState = 1;
        }
        else
        {
            tState = 2;
        }
        break;
    }
    case 1:
        if (ScriptMovement_IsObjectMovementFinished(OBJ_EVENT_ID_FOLLOWER, mapNum, mapGroup))
            tState = 2;
        break;
    case 2: // follower stored away; launch the climb-cross-descend sequence
        FlagSet(FLAG_TEMP_HIDE_FOLLOWER);
        RemoveFollowingPokemon();
        SetBounceJumpParams(tClimbDir, tJumpDir, tCrossTiles);
        ScriptMovement_StartObjectMovementScript(LOCALID_PLAYER, mapNum, mapGroup, sMovement_BounceJump);
        tState = 3;
        break;
    case 3:
        if (ScriptMovement_IsObjectMovementFinished(LOCALID_PLAYER, mapNum, mapGroup))
            tState = 4;
        break;
    case 4:
        // Respawn the follower next to the player at the landing tile; it pops
        // back out of its ball with the standard animation on the next step.
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
#undef tClimbDir
#undef tJumpDir
#undef tCrossTiles
#undef tFromScript

// ---- Activator B: party-menu field move (mirrors Surf / Rock Climb) ----
static void FieldCallback_Bounce(void)
{
    struct BounceRoute route;

    if (GetBounceRoute(&route))
        StartBounceJump(&route, FALSE);
    else
        ScriptContext_Enable();
}

bool32 SetUpFieldMove_Bounce(void)
{
    struct BounceRoute route;

    if (GetBounceRoute(&route))
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
    struct BounceRoute route;

    return (Special_GetFirstBounceMonIndex() != PARTY_SIZE)
        && GetBounceRoute(&route);
}

void Special_StartBounceJump(void)
{
    struct BounceRoute route;

    if (GetBounceRoute(&route))
        StartBounceJump(&route, TRUE);
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
