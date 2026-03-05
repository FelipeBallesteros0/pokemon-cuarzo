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

static void FieldCallback_SunnyDay(void);
static void FieldMove_SunnyDay(void);
static void EndSunnyDayTask(u8 taskId);

#define tFrameCount data[0]

bool32 SetUpFieldMove_SunnyDay(void)
{
    u16 weatherOverride = VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE);

    if (gWeather.currWeather == WEATHER_DROUGHT
     || weatherOverride == WEATHER_DROUGHT)
        return FALSE;

    gSpecialVar_Result = GetCursorSelectionMonId();
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_SunnyDay;
    return TRUE;
}

static void FieldCallback_SunnyDay(void)
{
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    ScriptContext_SetupScript(EventScript_UseSunnyDay);
}

bool8 FldEff_SunnyDay(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (uintptr_t)FieldMove_SunnyDay >> 16;
    gTasks[taskId].data[9] = (uintptr_t)FieldMove_SunnyDay;
    return FALSE;
}

static void FieldMove_SunnyDay(void)
{
    PlaySE12WithPanning(SE_M_MORNING_SUN, SOUND_PAN_ATTACKER);
    FieldEffectActiveListRemove(FLDEFF_SUNNY_DAY);

    VarSet(VAR_DYNAMIC_WEATHER_OVERRIDE, WEATHER_DROUGHT);
    VarSet(VAR_DYNAMIC_WEATHER, WEATHER_DROUGHT);
    VarSet(VAR_DYNAMIC_WEATHER_TIMER, 1);
    VarSet(VAR_DYNAMIC_WEATHER_MAPSEC, gMapHeader.regionMapSectionId);
    SetWeather(WEATHER_DROUGHT);

    u8 taskId = CreateTask(EndSunnyDayTask, 0);
    gTasks[taskId].tFrameCount = 0;
}

static void EndSunnyDayTask(u8 taskId)
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
