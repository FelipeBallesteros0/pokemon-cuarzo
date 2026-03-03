#include "global.h"
#include "dynamic_weather.h"
#include "event_data.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "field_weather.h"
#include "fldeff.h"
#include "palette.h"
#include "party_menu.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "constants/battle_anim.h"
#include "constants/field_effects.h"
#include "constants/songs.h"
#include "constants/vars.h"
#include "constants/weather.h"

static void FieldCallback_Haze(void);
static void FieldMove_Haze(void);
static void EndHazeTask(u8 taskId);

#define tFrameCount data[0]

bool32 SetUpFieldMove_Haze(void)
{
    u16 weatherOverride = VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE);

    if (gWeather.currWeather == WEATHER_FOG_HORIZONTAL
     || gWeather.currWeather == WEATHER_FOG_DIAGONAL
     || weatherOverride == WEATHER_FOG_HORIZONTAL
     || weatherOverride == WEATHER_FOG_DIAGONAL
     || weatherOverride == WEATHER_FOG)
        return FALSE;

    gSpecialVar_Result = GetCursorSelectionMonId();
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_Haze;
    return TRUE;
}

static void FieldCallback_Haze(void)
{
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    ScriptContext_SetupScript(EventScript_UseHaze);
}

bool8 FldEff_Haze(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (uintptr_t)FieldMove_Haze >> 16;
    gTasks[taskId].data[9] = (uintptr_t)FieldMove_Haze;
    return FALSE;
}

static void FieldMove_Haze(void)
{
    PlaySE12WithPanning(SE_M_HAZE, SOUND_PAN_ATTACKER);
    FieldEffectActiveListRemove(FLDEFF_HAZE);

    // Mirror Defog: instead of clearing fog, force a fog override.
    // Use SetWeather (not DynamicWeather_ApplyOverride) so weather transition
    // runs through the normal gradual fade path instead of popping instantly.
    VarSet(VAR_DYNAMIC_WEATHER_OVERRIDE, WEATHER_FOG_HORIZONTAL);
    VarSet(VAR_DYNAMIC_WEATHER, WEATHER_FOG_HORIZONTAL);
    VarSet(VAR_DYNAMIC_WEATHER_TIMER, 1);
    VarSet(VAR_DYNAMIC_WEATHER_MAPSEC, gMapHeader.regionMapSectionId);
    SetFogEntryFadeMode(TRUE);
    SetWeather(WEATHER_FOG_HORIZONTAL);

    u8 taskId = CreateTask(EndHazeTask, 0);
    gTasks[taskId].tFrameCount = 0;
}

static void EndHazeTask(u8 taskId)
{
    if (gPaletteFade.active)
        return;

    gTasks[taskId].tFrameCount++;
    if (gTasks[taskId].tFrameCount != 120)
        return;

    SetWeatherPalStateIdle();
    DestroyTask(taskId);
    ScriptContext_Enable();
}

#undef tFrameCount
