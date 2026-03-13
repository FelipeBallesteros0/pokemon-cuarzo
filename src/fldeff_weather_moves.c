#include "global.h"
#include "event_data.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "fldeff.h"
#include "overworld.h"
#include "party_menu.h"
#include "script.h"

extern const u8 EventScript_UseSunnyDay[];
extern const u8 EventScript_UseRainDance[];
extern const u8 EventScript_UseHail[];

static void FieldCallback_SunnyDay(void);
static void FieldCallback_RainDance(void);
static void FieldCallback_Hail(void);

bool32 SetUpFieldMove_SunnyDay(void)
{
    gSpecialVar_Result = GetCursorSelectionMonId();
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_SunnyDay;
    return TRUE;
}

bool32 SetUpFieldMove_RainDance(void)
{
    gSpecialVar_Result = GetCursorSelectionMonId();
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_RainDance;
    return TRUE;
}

bool32 SetUpFieldMove_Hail(void)
{
    gSpecialVar_Result = GetCursorSelectionMonId();
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_Hail;
    return TRUE;
}

static void FieldCallback_SunnyDay(void)
{
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    ScriptContext_SetupScript(EventScript_UseSunnyDay);
}

static void FieldCallback_RainDance(void)
{
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    ScriptContext_SetupScript(EventScript_UseRainDance);
}

static void FieldCallback_Hail(void)
{
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    ScriptContext_SetupScript(EventScript_UseHail);
}
