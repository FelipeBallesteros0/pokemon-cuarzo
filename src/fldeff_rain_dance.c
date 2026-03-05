#include "global.h"
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

static void FieldCallback_RainDance(void);
static void FieldMove_RainDance(void);
static void EndRainDanceTask(u8 taskId);

#define tFrameCount data[0]

bool32 SetUpFieldMove_RainDance(void)
{
    u16 weatherOverride = VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE);

    if (gWeather.currWeather == WEATHER_RAIN
     || weatherOverride == WEATHER_RAIN)
        return FALSE;

    gSpecialVar_Result = GetCursorSelectionMonId();
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_RainDance;
    return TRUE;
}

static void FieldCallback_RainDance(void)
{
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    ScriptContext_SetupScript(EventScript_UseRainDance);
}

bool8 FldEff_RainDance(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (uintptr_t)FieldMove_RainDance >> 16;
    gTasks[taskId].data[9] = (uintptr_t)FieldMove_RainDance;
    return FALSE;
}

static void FieldMove_RainDance(void)
{
    PlaySE12WithPanning(SE_M_RAIN_DANCE, SOUND_PAN_ATTACKER);
    FieldEffectActiveListRemove(FLDEFF_RAIN_DANCE);

    VarSet(VAR_DYNAMIC_WEATHER_OVERRIDE, WEATHER_RAIN);
    VarSet(VAR_DYNAMIC_WEATHER, WEATHER_RAIN);
    VarSet(VAR_DYNAMIC_WEATHER_TIMER, 1);
    VarSet(VAR_DYNAMIC_WEATHER_MAPSEC, gMapHeader.regionMapSectionId);
    SetWeather(WEATHER_RAIN);

    u8 taskId = CreateTask(EndRainDanceTask, 0);
    gTasks[taskId].tFrameCount = 0;
}

static void EndRainDanceTask(u8 taskId)
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
