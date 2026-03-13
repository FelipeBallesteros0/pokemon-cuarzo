#include "global.h"
#include "event_data.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "field_weather.h"
#include "fldeff.h"
#include "palette.h"
#include "overworld.h"
#include "party_menu.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "util.h"
#include "constants/battle_anim.h"
#include "constants/field_effects.h"
#include "constants/field_weather.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/weather.h"

static void FieldCallback_Defog(void);
static void FieldMove_Defog(void);
static void EndDefogTask(u8 taskId);
static bool32 CanDefogCurrentWeather(void);

static bool32 CanDefogCurrentWeather(void)
{
    switch (gWeather.currWeather)
    {
    case WEATHER_FOG_HORIZONTAL:
    case WEATHER_FOG_DIAGONAL:
    case WEATHER_FOG:      // aggregate id used by some custom/scripted flows
    case WEATHER_SHADE:    // custom fog-style atmosphere
        return TRUE;
    default:
        return FALSE;
    }
}

bool32 SetUpFieldMove_Defog(void)
{
    if (!CanDefogCurrentWeather())
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

#define tFrameCount  data[0]
#define tFogBlend    data[1]
#define tBlendDelay  data[2]

static void FieldMove_Defog(void)
{
    PlaySE12WithPanning(SE_M_SOLAR_BEAM, SOUND_PAN_ATTACKER);
    SetWeatherScreenFadeOut();
    FieldEffectActiveListRemove(FLDEFF_DEFOG);
    // Do not queue a parallel weather transition here; EndDefogTask handles
    // the entire fog clear and commits WEATHER_NONE atomically at the end.
    u32 taskId = CreateTask(EndDefogTask, 0);
    gTasks[taskId].tFrameCount = 0;
    gTasks[taskId].tFogBlend = 9;   // max fog coeff used by custom fog palette blend
    gTasks[taskId].tBlendDelay = 0; // slow down per-step clear for smoother finish
};

static void EndDefogTask(u8 taskId)
{
    if (gPaletteFade.active)
        return;

    gTasks[taskId].tFrameCount++;

    if (gTasks[taskId].tFrameCount < 120)
        return;

    if (gTasks[taskId].tFogBlend > 0)
    {
        gTasks[taskId].tBlendDelay++;
        if (gTasks[taskId].tBlendDelay >= 2)
        {
            gTasks[taskId].tBlendDelay = 0;
            gTasks[taskId].tFogBlend--;
            ApplyFogPalettesForTransition(gTasks[taskId].tFogBlend);
        }
        return;
    }

    // Reset weather state machine/palettes explicitly so custom fog modes
    // are cleared immediately without requiring UI/map refresh.
    SetWeatherPalStateIdle();
    SetCurrentAndNextWeatherNoDelay(WEATHER_NONE);
    ApplyWeatherColorMapIfIdle(0);
    ApplyWeatherColorMapToPals(0, 32);
    UpdateAltBgPalettes(PALETTES_BG);
    UpdatePalettesWithTime(PALETTES_ALL);
    Weather_SetBlendCoeffs(8, BASE_SHADOW_INTENSITY);
    UpdateShadowColor(RGB_BLACK);

    DestroyTask(taskId);
    ScriptContext_Enable();
}

#undef tFrameCount
