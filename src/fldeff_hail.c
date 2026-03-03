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

static void FieldCallback_Hail(void);
static void FieldMove_Hail(void);
static void EndHailTask(u8 taskId);

#define tFrameCount data[0]

bool32 SetUpFieldMove_Hail(void)
{
    u16 weatherOverride = VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE);

    if (gWeather.currWeather == WEATHER_SNOW
     || weatherOverride == WEATHER_SNOW)
        return FALSE;

    gSpecialVar_Result = GetCursorSelectionMonId();
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_Hail;
    return TRUE;
}

static void FieldCallback_Hail(void)
{
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    ScriptContext_SetupScript(EventScript_UseHail);
}

bool8 FldEff_Hail(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (uintptr_t)FieldMove_Hail >> 16;
    gTasks[taskId].data[9] = (uintptr_t)FieldMove_Hail;
    return FALSE;
}

static void FieldMove_Hail(void)
{
    PlaySE12WithPanning(SE_M_HAIL, SOUND_PAN_ATTACKER);
    FieldEffectActiveListRemove(FLDEFF_HAIL);

    VarSet(VAR_DYNAMIC_WEATHER_OVERRIDE, WEATHER_SNOW);
    VarSet(VAR_DYNAMIC_WEATHER, WEATHER_SNOW);
    VarSet(VAR_DYNAMIC_WEATHER_TIMER, 1);
    VarSet(VAR_DYNAMIC_WEATHER_MAPSEC, gMapHeader.regionMapSectionId);
    SetWeather(WEATHER_SNOW);

    u8 taskId = CreateTask(EndHailTask, 0);
    gTasks[taskId].tFrameCount = 0;
}

static void EndHailTask(u8 taskId)
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
