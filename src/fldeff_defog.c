#include "global.h"
#include "event_data.h"
#include "event_scripts.h"
#include "dynamic_weather.h"
#include "field_effect.h"
#include "field_weather.h"
#include "fldeff.h"
#include "palette.h"
#include "party_menu.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "util.h"
#include "constants/battle_anim.h"
#include "constants/field_weather.h"
#include "constants/field_effects.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/vars.h"
#include "constants/weather.h"

static void FieldCallback_Defog(void);
static void FieldMove_Defog(void);
static void EndDefogTask(u8 taskId);

bool32 SetUpFieldMove_Defog(void)
{
    u16 weatherOverride = VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE);

    // Defog as an overworld weather move: set clear weather (Despejado).
    // Don't allow use if weather is already clear and override is already clear.
    if ((gWeather.currWeather == WEATHER_NONE || gWeather.currWeather == WEATHER_SUNNY_CLOUDS)
     && (weatherOverride == WEATHER_NONE || weatherOverride == WEATHER_SUNNY_CLOUDS))
        return FALSE;

    gSpecialVar_Result = GetCursorSelectionMonId();
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_Defog;
    return TRUE;
}

static void FieldCallback_Defog(void)
{
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    ScriptContext_SetupScript(EventScript_UseDefog);
}

bool8 FldEff_Defog(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (uintptr_t)FieldMove_Defog>> 16;
    gTasks[taskId].data[9] = (uintptr_t)FieldMove_Defog;
    return FALSE;
}

#define tFrameCount data[0]

static void FieldMove_Defog(void)
{
    PlaySE12WithPanning(SE_M_SOLAR_BEAM, SOUND_PAN_ATTACKER);
    FieldEffectActiveListRemove(FLDEFF_DEFOG);

    // Force clear weather like the other overworld weather moves.
    VarSet(VAR_DYNAMIC_WEATHER_OVERRIDE, WEATHER_NONE);
    VarSet(VAR_DYNAMIC_WEATHER, WEATHER_NONE);
    VarSet(VAR_DYNAMIC_WEATHER_TIMER, 1);
    VarSet(VAR_DYNAMIC_WEATHER_MAPSEC, gMapHeader.regionMapSectionId);
    SetWeather(WEATHER_NONE);

    u32 taskId = CreateTask(EndDefogTask, 0);
    gTasks[taskId].tFrameCount = 0;
};

static void EndDefogTask(u8 taskId)
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
