#include "global.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "bg.h"
#include "debug.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_object_lock.h"
#include "event_scripts.h"
#include "fieldmap.h"
#include "field_camera.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "field_weather.h"
#include "field_screen_effect.h"
#include "frontier_pass.h"
#include "frontier_util.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "item_menu.h"
#include "link.h"
#include "load_save.h"
#include "main.h"
#include "menu.h"
#include "new_game.h"
#include "option_menu.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokenav.h"
#include "safari_zone.h"
#include "save.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "start_menu.h"
#include "rtc.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "tileset_anims.h"
#include "trainer_card.h"
#include "window.h"
#include "union_room.h"
#include "dexnav.h"
#include "wild_encounter.h"
#include "constants/battle_frontier.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// Menu actions
enum
{
    MENU_ACTION_POKEDEX,
    MENU_ACTION_POKEMON,
    MENU_ACTION_BAG,
    MENU_ACTION_POKENAV,
    MENU_ACTION_PLAYER,
    MENU_ACTION_SAVE,
    MENU_ACTION_OPTION,
    MENU_ACTION_EXIT,
    MENU_ACTION_RETIRE_SAFARI,
    MENU_ACTION_PLAYER_LINK,
    MENU_ACTION_REST_FRONTIER,
    MENU_ACTION_RETIRE_FRONTIER,
    MENU_ACTION_PYRAMID_BAG,
    MENU_ACTION_DEBUG,
    MENU_ACTION_DEXNAV,
};

// Save status
enum
{
    SAVE_IN_PROGRESS,
    SAVE_SUCCESS,
    SAVE_CANCELED,
    SAVE_ERROR
};

// IWRAM common
COMMON_DATA bool8 (*gMenuCallback)(void) = NULL;

// EWRAM
EWRAM_DATA static u8 sSafariBallsWindowId = 0;
EWRAM_DATA static u8 sBattlePyramidFloorWindowId = 0;
EWRAM_DATA static u8 sStartMenuCursorPos = 0;
EWRAM_DATA static u8 sNumStartMenuActions = 0;
EWRAM_DATA static u8 sCurrentStartMenuActions[9] = {0};
EWRAM_DATA static s8 sInitStartMenuData[2] = {0};

EWRAM_DATA static u8 (*sSaveDialogCallback)(void) = NULL;
EWRAM_DATA static u8 sSaveDialogTimer = 0;
EWRAM_DATA static bool8 sSavingComplete = FALSE;
EWRAM_DATA static u8 sSaveInfoWindowId = 0;
EWRAM_DATA static u8 sStartMenuButtonTextWindowIds[10] = {0};
EWRAM_DATA static u8 sStartMenuButtonTextWindowCount = 0;
EWRAM_DATA static u8 sStartMenuClockWindowId = 0;
EWRAM_DATA static u8 sStartMenuClockHour = 0;
EWRAM_DATA static u8 sStartMenuClockMinute = 0;
EWRAM_DATA static bool8 sStartMenuClockWindowActive = FALSE;
EWRAM_DATA static bool8 sStartMenuClockValueValid = FALSE;
EWRAM_DATA static bool8 sStartMenuScrollBgActive = FALSE;
EWRAM_DATA static bool8 sStartMenuObjWasEnabled = FALSE;
EWRAM_DATA static bool8 sStartMenuTransitionPendingUnblank = FALSE;
EWRAM_DATA static u16 sStartMenuScrollBgTilemapBuffer[32 * 32];
EWRAM_DATA static void *sStartMenuSavedBg3TilemapBuffer = NULL;
EWRAM_DATA static s8 sStartMenuGridActionIndices[9] = {0};
EWRAM_DATA static u8 sStartMenuSelectedGridSlot = 0;

#define START_MENU_BUTTON_PAL_SLOT 13
#define START_MENU_TEXT_PAL_SLOT 14
#define START_MENU_BUTTON_WIDTH_TILES 8
#define START_MENU_BUTTON_HEIGHT_TILES 4
#define START_MENU_BUTTON_GRID_COLUMNS 3
#define START_MENU_BUTTON_GRID_ROWS 3
#define START_MENU_BUTTON_GRID_X 2
#define START_MENU_BUTTON_GRID_Y 3
#define START_MENU_BUTTON_GRID_X_SPACING 9
#define START_MENU_BUTTON_GRID_Y_SPACING 5
// BG0 in overworld uses charbase 2; keep custom tiles below 0x200 to avoid spilling into map VRAM.
#define START_MENU_BUTTON_BASE_TILE 0x140
#define START_MENU_BUTTON_SELECTED_BASE_TILE 0x160
#define START_MENU_BUTTON_TEXT_BASEBLOCK 0x80
#define START_MENU_BAR_TILE 0x1F0

// Menu action callbacks
static bool8 StartMenuPokedexCallback(void);
static bool8 StartMenuPokemonCallback(void);
static bool8 StartMenuBagCallback(void);
static bool8 StartMenuPokeNavCallback(void);
static bool8 StartMenuPlayerNameCallback(void);
static bool8 StartMenuSaveCallback(void);
static bool8 StartMenuOptionCallback(void);
static bool8 StartMenuExitCallback(void);
static bool8 StartMenuSafariZoneRetireCallback(void);
static bool8 StartMenuLinkModePlayerNameCallback(void);
static bool8 StartMenuBattlePyramidRetireCallback(void);
static bool8 StartMenuBattlePyramidBagCallback(void);
static bool8 StartMenuDebugCallback(void);
static bool8 StartMenuDexNavCallback(void);

// Menu callbacks
static bool8 SaveStartCallback(void);
static bool8 SaveCallback(void);
static bool8 BattlePyramidRetireStartCallback(void);
static bool8 BattlePyramidRetireReturnCallback(void);
static bool8 BattlePyramidRetireCallback(void);
static bool8 HandleStartMenuInput(void);

// Save dialog callbacks
static u8 SaveConfirmSaveCallback(void);
static u8 SaveYesNoCallback(void);
static u8 SaveConfirmInputCallback(void);
static u8 SaveFileExistsCallback(void);
static u8 SaveConfirmOverwriteDefaultNoCallback(void);
static u8 SaveConfirmOverwriteCallback(void);
static u8 SaveOverwriteInputCallback(void);
static u8 SaveSavingMessageCallback(void);
static u8 SaveDoSaveCallback(void);
static u8 SaveSuccessCallback(void);
static u8 SaveReturnSuccessCallback(void);
static u8 SaveErrorCallback(void);
static u8 SaveReturnErrorCallback(void);
static u8 BattlePyramidConfirmRetireCallback(void);
static u8 BattlePyramidRetireYesNoCallback(void);
static u8 BattlePyramidRetireInputCallback(void);

// Task callbacks
static void StartMenuTask(u8 taskId);
static void SaveGameTask(u8 taskId);
static void Task_SaveAfterLinkBattle(u8 taskId);
static void Task_WaitForBattleTowerLinkSave(u8 taskId);
static bool8 FieldCB_ReturnToFieldStartMenu(void);

static const struct WindowTemplate sWindowTemplate_SafariBalls = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 9,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x8
};

static const u8 *const sPyramidFloorNames[FRONTIER_STAGES_PER_CHALLENGE + 1] =
{
    gText_Floor1,
    gText_Floor2,
    gText_Floor3,
    gText_Floor4,
    gText_Floor5,
    gText_Floor6,
    gText_Floor7,
    gText_Peak
};

static const struct WindowTemplate sWindowTemplate_PyramidFloor = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 10,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x8
};

static const struct WindowTemplate sWindowTemplate_PyramidPeak = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 12,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x8
};

static const u8 sText_MenuDebug[] = _("DEBUG");

static const struct MenuAction sStartMenuItems[] =
{
    [MENU_ACTION_POKEDEX]         = {gText_MenuPokedex, {.u8_void = StartMenuPokedexCallback}},
    [MENU_ACTION_POKEMON]         = {gText_MenuPokemon, {.u8_void = StartMenuPokemonCallback}},
    [MENU_ACTION_BAG]             = {gText_MenuBag,     {.u8_void = StartMenuBagCallback}},
    [MENU_ACTION_POKENAV]         = {gText_MenuPokenav, {.u8_void = StartMenuPokeNavCallback}},
    [MENU_ACTION_PLAYER]          = {gText_MenuPlayer,  {.u8_void = StartMenuPlayerNameCallback}},
    [MENU_ACTION_SAVE]            = {gText_MenuSave,    {.u8_void = StartMenuSaveCallback}},
    [MENU_ACTION_OPTION]          = {gText_MenuOption,  {.u8_void = StartMenuOptionCallback}},
    [MENU_ACTION_EXIT]            = {gText_MenuExit,    {.u8_void = StartMenuExitCallback}},
    [MENU_ACTION_RETIRE_SAFARI]   = {gText_MenuRetire,  {.u8_void = StartMenuSafariZoneRetireCallback}},
    [MENU_ACTION_PLAYER_LINK]     = {gText_MenuPlayer,  {.u8_void = StartMenuLinkModePlayerNameCallback}},
    [MENU_ACTION_REST_FRONTIER]   = {gText_MenuRest,    {.u8_void = StartMenuSaveCallback}},
    [MENU_ACTION_RETIRE_FRONTIER] = {gText_MenuRetire,  {.u8_void = StartMenuBattlePyramidRetireCallback}},
    [MENU_ACTION_PYRAMID_BAG]     = {gText_MenuBag,     {.u8_void = StartMenuBattlePyramidBagCallback}},
    [MENU_ACTION_DEBUG]           = {sText_MenuDebug,   {.u8_void = StartMenuDebugCallback}},
    [MENU_ACTION_DEXNAV]          = {gText_MenuDexNav,  {.u8_void = StartMenuDexNavCallback}},
};

static const struct BgTemplate sBgTemplates_LinkBattleSave[] =
{
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    }
};

static const struct WindowTemplate sWindowTemplates_LinkBattleSave[] =
{
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x194
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sSaveInfoWindowTemplate = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 14,
    .height = 10,
    .paletteNum = 15,
    .baseBlock = 8
};

static const u8 sStartMenuScrollBgTiles[] = INCBIN_U8("graphics/ui_startmenu_full/pause_scroll.img.bin");
static const u16 sStartMenuScrollBgTilemap[] = INCBIN_U16("graphics/ui_startmenu_full/pause_scroll.map.bin");
static const u16 sStartMenuScrollBgPalette[] = INCBIN_U16("graphics/ui_startmenu_full/pause_scroll.pal.bin");
static const u8 sStartMenuButtonTiles[] = INCBIN_U8("graphics/ui_startmenu_full/boton.4bpp");
static const u8 sStartMenuButtonSelectedTiles[] = INCBIN_U8("graphics/ui_startmenu_full/boton_selected.4bpp");
static const u16 sStartMenuButtonPalette[] = INCBIN_U16("graphics/ui_startmenu_full/boton.gbapal");
static const u16 sStartMenuCursorPalette[] = INCBIN_U16("graphics/ui_startmenu_full/cursor.gbapal");
static u16 sStartMenuButtonPaletteDynamic[ARRAY_COUNT(sStartMenuButtonPalette)];

// Text colors using dedicated text palette slots: bg, fg, shadow.
static const u8 sStartMenuButtonTextColors[] = {1, 2, 3};
static const u8 sStartMenuClockTextColors[] = {5, 3, 2};

// Local functions
static void BuildStartMenuActions(void);
static void AddStartMenuAction(u8 action);
static void BuildNormalStartMenu(void);
static void BuildDebugStartMenu(void);
static void BuildSafariZoneStartMenu(void);
static void BuildLinkModeStartMenu(void);
static void BuildUnionRoomStartMenu(void);
static void BuildBattlePikeStartMenu(void);
static void BuildBattlePyramidStartMenu(void);
static void BuildMultiPartnerRoomStartMenu(void);
static void ShowSafariBallsWindow(void);
static void ShowPyramidFloorWindow(void);
static void RemoveExtraStartMenuWindows(void);
static bool32 PrintStartMenuActions(s8 *pIndex, u32 count);
static bool32 InitStartMenuStep(void);
static void InitStartMenu(void);
static void CreateStartMenuTask(TaskFunc followupFunc);
static void InitSave(void);
static u8 RunSaveCallback(void);
static void ShowSaveMessage(const u8 *message, u8 (*saveCallback)(void));
static void HideSaveMessageWindow(void);
static void HideSaveInfoWindow(void);
static void SaveStartTimer(void);
static bool8 SaveSuccesTimer(void);
static bool8 SaveErrorTimer(void);
static void InitBattlePyramidRetire(void);
static void VBlankCB_LinkBattleSave(void);
static bool32 InitSaveWindowAfterLinkBattle(u8 *par1);
static void CB2_SaveAfterLinkBattle(void);
static void ShowSaveInfoWindow(void);
static void RemoveSaveInfoWindow(void);
static void HideStartMenuWindow(void);
static void HideStartMenuDebug(void);
static void StartMenu_EnableScrollingBg(void);
static void StartMenu_UpdateScrollingBg(void);
static void StartMenu_DisableScrollingBg(void);
static void StartMenu_DrawButtonGrid(void);
static void StartMenu_DrawEdgeBars(void);
static void StartMenu_DrawButtonText(void);
static void StartMenu_RemoveButtonTextWindow(void);
static void StartMenu_BuildButtonPaletteByGender(void);
static void StartMenu_LoadTextPalette(void);
static void StartMenu_UpdateClockWindow(void);
static void StartMenu_BuildGridActionMap(void);
static void StartMenu_RefreshCustomMenuVisuals(void);
static bool8 StartMenu_MoveCursorInGrid(s8 dx, s8 dy);

void SetDexPokemonPokenavFlags(void) // unused
{
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKENAV_GET);
}

static void BuildStartMenuActions(void)
{
    sNumStartMenuActions = 0;

    if (IsOverworldLinkActive() == TRUE)
    {
        BuildLinkModeStartMenu();
    }
    else if (InUnionRoom() == TRUE)
    {
        BuildUnionRoomStartMenu();
    }
    else if (GetSafariZoneFlag() == TRUE)
    {
        BuildSafariZoneStartMenu();
    }
    else if (InBattlePike())
    {
        BuildBattlePikeStartMenu();
    }
    else if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
    {
        BuildBattlePyramidStartMenu();
    }
    else if (InMultiPartnerRoom())
    {
        BuildMultiPartnerRoomStartMenu();
    }
    else
    {
        if (DEBUG_OVERWORLD_MENU == TRUE && DEBUG_OVERWORLD_IN_MENU == TRUE)
            BuildDebugStartMenu();
        else
            BuildNormalStartMenu();
    }
}

static void AddStartMenuAction(u8 action)
{
    AppendToList(sCurrentStartMenuActions, &sNumStartMenuActions, action);
}

static void BuildNormalStartMenu(void)
{
    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKEDEX);

    if (DN_FLAG_DEXNAV_GET != 0 && FlagGet(DN_FLAG_DEXNAV_GET))
        AddStartMenuAction(MENU_ACTION_DEXNAV);

    if (FlagGet(FLAG_SYS_POKEMON_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKEMON);

    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKENAV);

    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_SAVE);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildDebugStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_DEBUG);
    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKEDEX);
    if (FlagGet(FLAG_SYS_POKEMON_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);
    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKENAV);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_SAVE);
    AddStartMenuAction(MENU_ACTION_OPTION);
}

static void BuildSafariZoneStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_RETIRE_SAFARI);
    AddStartMenuAction(MENU_ACTION_POKEDEX);
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildLinkModeStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKENAV);
    }

    AddStartMenuAction(MENU_ACTION_PLAYER_LINK);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildUnionRoomStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKENAV);
    }

    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildBattlePikeStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEDEX);
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildBattlePyramidStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PYRAMID_BAG);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_REST_FRONTIER);
    AddStartMenuAction(MENU_ACTION_RETIRE_FRONTIER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildMultiPartnerRoomStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void ShowSafariBallsWindow(void)
{
    sSafariBallsWindowId = AddWindow(&sWindowTemplate_SafariBalls);
    PutWindowTilemap(sSafariBallsWindowId);
    DrawStdWindowFrame(sSafariBallsWindowId, FALSE);
    ConvertIntToDecimalStringN(gStringVar1, gNumSafariBalls, STR_CONV_MODE_RIGHT_ALIGN, 2);
    StringExpandPlaceholders(gStringVar4, gText_SafariBallStock);
    AddTextPrinterParameterized(sSafariBallsWindowId, FONT_NORMAL, gStringVar4, 0, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(sSafariBallsWindowId, COPYWIN_GFX);
}

static void ShowPyramidFloorWindow(void)
{
    if (gSaveBlock2Ptr->frontier.curChallengeBattleNum == FRONTIER_STAGES_PER_CHALLENGE)
        sBattlePyramidFloorWindowId = AddWindow(&sWindowTemplate_PyramidPeak);
    else
        sBattlePyramidFloorWindowId = AddWindow(&sWindowTemplate_PyramidFloor);

    PutWindowTilemap(sBattlePyramidFloorWindowId);
    DrawStdWindowFrame(sBattlePyramidFloorWindowId, FALSE);
    StringCopy(gStringVar1, sPyramidFloorNames[gSaveBlock2Ptr->frontier.curChallengeBattleNum]);
    StringExpandPlaceholders(gStringVar4, gText_BattlePyramidFloor);
    AddTextPrinterParameterized(sBattlePyramidFloorWindowId, FONT_NORMAL, gStringVar4, 0, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(sBattlePyramidFloorWindowId, COPYWIN_GFX);
}

static void RemoveExtraStartMenuWindows(void)
{
    if (GetSafariZoneFlag())
    {
        ClearStdWindowAndFrameToTransparent(sSafariBallsWindowId, FALSE);
        CopyWindowToVram(sSafariBallsWindowId, COPYWIN_GFX);
        RemoveWindow(sSafariBallsWindowId);
    }
    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
    {
        ClearStdWindowAndFrameToTransparent(sBattlePyramidFloorWindowId, FALSE);
        RemoveWindow(sBattlePyramidFloorWindowId);
    }
}

static bool32 PrintStartMenuActions(s8 *pIndex, u32 count)
{
    s8 index = *pIndex;

    do
    {
        if (sStartMenuItems[sCurrentStartMenuActions[index]].func.u8_void == StartMenuPlayerNameCallback)
        {
            PrintPlayerNameOnWindow(GetStartMenuWindowId(), sStartMenuItems[sCurrentStartMenuActions[index]].text, 8, (index << 4) + 9);
        }
        else
        {
            StringExpandPlaceholders(gStringVar4, sStartMenuItems[sCurrentStartMenuActions[index]].text);
            AddTextPrinterParameterized(GetStartMenuWindowId(), FONT_NORMAL, gStringVar4, 8, (index << 4) + 9, TEXT_SKIP_DRAW, NULL);
        }

        index++;
        if (index >= sNumStartMenuActions)
        {
            *pIndex = index;
            return TRUE;
        }

        count--;
    }
    while (count != 0);

    *pIndex = index;
    return FALSE;
}

static bool32 InitStartMenuStep(void)
{
    s8 state = sInitStartMenuData[0];

    switch (state)
    {
    case 0:
        StartMenu_EnableScrollingBg();
        sInitStartMenuData[0]++;
        break;
    case 1:
        BuildStartMenuActions();
        sInitStartMenuData[0]++;
        break;
    case 2:
        LoadMessageBoxAndBorderGfx();
        AddStartMenuWindow(sNumStartMenuActions); // Kept for save/retire flows that remove this window.
        StartMenu_BuildGridActionMap();
        StartMenu_RefreshCustomMenuVisuals();
        sInitStartMenuData[0]++;
        break;
    case 3:
        if (GetSafariZoneFlag())
            ShowSafariBallsWindow();
        if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
            ShowPyramidFloorWindow();
        return TRUE;
    }

    return FALSE;
}

static void InitStartMenu(void)
{
    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    while (!InitStartMenuStep())
        ;
}

static void StartMenuTask(u8 taskId)
{
    InitStartMenu();
    SwitchTaskToFollowupFunc(taskId);
}

static void CreateStartMenuTask(TaskFunc followupFunc)
{
    u8 taskId;

    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    taskId = CreateTask(StartMenuTask, 0x50);
    SetTaskFuncWithFollowupFunc(taskId, StartMenuTask, followupFunc);
}

static bool8 FieldCB_ReturnToFieldStartMenu(void)
{
    if (InitStartMenuStep() == FALSE)
    {
        return FALSE;
    }

    ReturnToFieldOpenStartMenu();
    return TRUE;
}

void ShowReturnToFieldStartMenu(void)
{
    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    gFieldCallback2 = FieldCB_ReturnToFieldStartMenu;
}

void Task_ShowStartMenu(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch(task->data[0])
    {
    case 0:
        if (InUnionRoom() == TRUE)
            SetUsingUnionRoomStartMenu();

        // After returning from submenus, force a clean redraw before handling input.
        LoadBgTiles(0, sStartMenuButtonSelectedTiles, sizeof(sStartMenuButtonSelectedTiles), START_MENU_BUTTON_SELECTED_BASE_TILE);
        StartMenu_RefreshCustomMenuVisuals();
        gMenuCallback = HandleStartMenuInput;
        task->data[0]++;
        break;
    case 1:
        if (gMenuCallback() == TRUE)
            DestroyTask(taskId);
        break;
    }
}

void ShowStartMenu(void)
{
    if (!IsOverworldLinkActive())
    {
        FreezeObjectEvents();
        PlayerFreeze();
        StopPlayerAvatar();
    }
    CreateStartMenuTask(Task_ShowStartMenu);
    LockPlayerFieldControls();
}

static bool8 HandleStartMenuInput(void)
{
    s8 actionIndex;

    StartMenu_UpdateScrollingBg();

    if (JOY_NEW(DPAD_LEFT) && StartMenu_MoveCursorInGrid(-1, 0))
    {
        PlaySE(SE_SELECT);
    }
    else if (JOY_NEW(DPAD_RIGHT) && StartMenu_MoveCursorInGrid(1, 0))
    {
        PlaySE(SE_SELECT);
    }
    else if (JOY_NEW(DPAD_UP) && StartMenu_MoveCursorInGrid(0, -1))
    {
        PlaySE(SE_SELECT);
    }
    else if (JOY_NEW(DPAD_DOWN) && StartMenu_MoveCursorInGrid(0, 1))
    {
        PlaySE(SE_SELECT);
    }

    if (JOY_NEW(A_BUTTON))
    {
        actionIndex = sStartMenuGridActionIndices[sStartMenuSelectedGridSlot];
        if (actionIndex < 0)
            return FALSE;

        sStartMenuCursorPos = actionIndex;
        PlaySE(SE_SELECT);
        if (sStartMenuItems[sCurrentStartMenuActions[sStartMenuCursorPos]].func.u8_void == StartMenuPokedexCallback)
        {
            if (GetNationalPokedexCount(FLAG_GET_SEEN) == 0)
                return FALSE;
        }
        gMenuCallback = sStartMenuItems[sCurrentStartMenuActions[sStartMenuCursorPos]].func.u8_void;

        if (gMenuCallback != StartMenuSaveCallback
            && gMenuCallback != StartMenuExitCallback
            && gMenuCallback != StartMenuDebugCallback
            && gMenuCallback != StartMenuSafariZoneRetireCallback
            && gMenuCallback != StartMenuBattlePyramidRetireCallback)
        {
           FadeScreen(FADE_TO_BLACK, 0);
        }

        return FALSE;
    }

    if (JOY_NEW(START_BUTTON | B_BUTTON))
    {
        RemoveExtraStartMenuWindows();
        HideStartMenu();
        return TRUE;
    }

    return FALSE;
}

bool8 StartMenuPokedexCallback(void)
{
    if (!gPaletteFade.active)
    {
        IncrementGameStat(GAME_STAT_CHECKED_POKEDEX);
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_OpenPokedex);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPokemonCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PartyMenuFromStartMenu); // Display party menu

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuBagCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_BagMenuFromStartMenu); // Display bag menu

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPokeNavCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitPokeNav);  // Display PokéNav

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();

        if (IsOverworldLinkActive() || InUnionRoom())
            ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu); // Display trainer card
        else if (FlagGet(FLAG_SYS_FRONTIER_PASS))
            ShowFrontierPass(CB2_ReturnToFieldWithOpenMenu); // Display frontier pass
        else
            ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu); // Display trainer card

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuSaveCallback(void)
{
    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
        RemoveExtraStartMenuWindows();

    gMenuCallback = SaveStartCallback; // Display save menu

    return FALSE;
}

static bool8 StartMenuOptionCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitOptionMenu); // Display option menu
        gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuExitCallback(void)
{
    RemoveExtraStartMenuWindows();
    HideStartMenu(); // Hide start menu

    return TRUE;
}

static bool8 StartMenuDebugCallback(void)
{
    RemoveExtraStartMenuWindows();
    HideStartMenuDebug(); // Hide start menu without enabling movement

    if (DEBUG_OVERWORLD_MENU)
    {
        FreezeObjectEvents();
        Debug_ShowMainMenu();
    }

return TRUE;
}

static bool8 StartMenuSafariZoneRetireCallback(void)
{
    RemoveExtraStartMenuWindows();
    HideStartMenu();
    SafariZoneRetirePrompt();

    return TRUE;
}

static void HideStartMenuDebug(void)
{
    PlaySE(SE_SELECT);
    StartMenu_DisableScrollingBg();
    ClearStdWindowAndFrame(GetStartMenuWindowId(), TRUE);
    RemoveStartMenuWindow();
}

static bool8 StartMenuLinkModePlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        ShowTrainerCardInLink(gLocalLinkPlayerId, CB2_ReturnToFieldWithOpenMenu);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuBattlePyramidRetireCallback(void)
{
    gMenuCallback = BattlePyramidRetireStartCallback; // Confirm retire

    return FALSE;
}

// Functionally unused
void ShowBattlePyramidStartMenu(void)
{
    ClearDialogWindowAndFrameToTransparent(0, FALSE);
    ScriptUnfreezeObjectEvents();
    CreateStartMenuTask(Task_ShowStartMenu);
    LockPlayerFieldControls();
}

static bool8 StartMenuBattlePyramidBagCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PyramidBagMenuFromStartMenu);

        return TRUE;
    }

    return FALSE;
}

static bool8 SaveStartCallback(void)
{
    InitSave();
    gMenuCallback = SaveCallback;

    return FALSE;
}

static bool8 SaveCallback(void)
{
    switch (RunSaveCallback())
    {
    case SAVE_IN_PROGRESS:
        return FALSE;
    case SAVE_CANCELED: // Back to start menu
        ClearDialogWindowAndFrameToTransparent(0, FALSE);
        InitStartMenu();
        gMenuCallback = HandleStartMenuInput;
        return FALSE;
    case SAVE_SUCCESS:
    case SAVE_ERROR:    // Close start menu
        ClearDialogWindowAndFrameToTransparent(0, TRUE);
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        SoftResetInBattlePyramid();
        return TRUE;
    }

    return FALSE;
}

static bool8 BattlePyramidRetireStartCallback(void)
{
    InitBattlePyramidRetire();
    gMenuCallback = BattlePyramidRetireCallback;

    return FALSE;
}

static bool8 BattlePyramidRetireReturnCallback(void)
{
    InitStartMenu();
    gMenuCallback = HandleStartMenuInput;

    return FALSE;
}

static bool8 BattlePyramidRetireCallback(void)
{
    switch (RunSaveCallback())
    {
    case SAVE_SUCCESS: // No (Stay in battle pyramid)
        RemoveExtraStartMenuWindows();
        gMenuCallback = BattlePyramidRetireReturnCallback;
        return FALSE;
    case SAVE_IN_PROGRESS:
        return FALSE;
    case SAVE_CANCELED: // Yes (Retire from battle pyramid)
        ClearDialogWindowAndFrameToTransparent(0, TRUE);
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        ScriptContext_SetupScript(BattlePyramid_Retire);
        return TRUE;
    }

    return FALSE;
}

static void InitSave(void)
{
    SaveMapView();
    sSaveDialogCallback = SaveConfirmSaveCallback;
    sSavingComplete = FALSE;
}

static u8 RunSaveCallback(void)
{
    // True if text is still printing
    if (RunTextPrintersAndIsPrinter0Active() == TRUE)
    {
        return SAVE_IN_PROGRESS;
    }

    sSavingComplete = FALSE;
    return sSaveDialogCallback();
}

void SaveGame(void)
{
    InitSave();
    CreateTask(SaveGameTask, 0x50);
}

static void ShowSaveMessage(const u8 *message, u8 (*saveCallback)(void))
{
    StringExpandPlaceholders(gStringVar4, message);
    LoadMessageBoxAndFrameGfx(0, TRUE);
    AddTextPrinterForMessage(TRUE);
    sSavingComplete = TRUE;
    sSaveDialogCallback = saveCallback;
}

static void SaveGameTask(u8 taskId)
{
    u8 status = RunSaveCallback();

    switch (status)
    {
    case SAVE_CANCELED:
    case SAVE_ERROR:
        gSpecialVar_Result = 0;
        break;
    case SAVE_SUCCESS:
        gSpecialVar_Result = status;
        break;
    case SAVE_IN_PROGRESS:
        return;
    }

    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void HideSaveMessageWindow(void)
{
    ClearDialogWindowAndFrame(0, TRUE);
}

static void HideSaveInfoWindow(void)
{
    RemoveSaveInfoWindow();
}

static void SaveStartTimer(void)
{
    sSaveDialogTimer = 60;
}

static bool8 SaveSuccesTimer(void)
{
    sSaveDialogTimer--;

    if (JOY_HELD(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        return TRUE;
    }
    if (sSaveDialogTimer == 0)
    {
        return TRUE;
    }

    return FALSE;
}

static bool8 SaveErrorTimer(void)
{
    if (sSaveDialogTimer != 0)
    {
        sSaveDialogTimer--;
    }
    else if (JOY_HELD(A_BUTTON))
    {
        return TRUE;
    }

    return FALSE;
}

static u8 SaveConfirmSaveCallback(void)
{
    if (IsStartMenuScrollBgActive())
        StartMenu_DisableScrollingBg();

    ClearStdWindowAndFrame(GetStartMenuWindowId(), FALSE);
    RemoveStartMenuWindow();
    ShowSaveInfoWindow();

    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
    {
        ShowSaveMessage(gText_BattlePyramidConfirmRest, SaveYesNoCallback);
    }
    else
    {
        ShowSaveMessage(gText_ConfirmSave, SaveYesNoCallback);
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveYesNoCallback(void)
{
    DisplayYesNoMenuDefaultYes(); // Show Yes/No menu
    sSaveDialogCallback = SaveConfirmInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        switch (gSaveFileStatus)
        {
        case SAVE_STATUS_EMPTY:
        case SAVE_STATUS_CORRUPT:
            if (gDifferentSaveFile == FALSE)
            {
                sSaveDialogCallback = SaveFileExistsCallback;
                return SAVE_IN_PROGRESS;
            }

            sSaveDialogCallback = SaveSavingMessageCallback;
            return SAVE_IN_PROGRESS;
        default:
            sSaveDialogCallback = SaveFileExistsCallback;
            return SAVE_IN_PROGRESS;
        }
    case MENU_B_PRESSED:
    case 1: // No
        HideSaveInfoWindow();
        HideSaveMessageWindow();
        return SAVE_CANCELED;
    }

    return SAVE_IN_PROGRESS;
}

// A different save file exists
static u8 SaveFileExistsCallback(void)
{
    if (gDifferentSaveFile == TRUE)
    {
        ShowSaveMessage(gText_DifferentSaveFile, SaveConfirmOverwriteDefaultNoCallback);
    }
    else
    {
        ShowSaveMessage(gText_AlreadySavedFile, SaveConfirmOverwriteCallback);
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmOverwriteDefaultNoCallback(void)
{
    DisplayYesNoMenuWithDefault(1); // Show Yes/No menu (No selected as default)
    sSaveDialogCallback = SaveOverwriteInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmOverwriteCallback(void)
{
    DisplayYesNoMenuDefaultYes(); // Show Yes/No menu
    sSaveDialogCallback = SaveOverwriteInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveOverwriteInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        sSaveDialogCallback = SaveSavingMessageCallback;
        return SAVE_IN_PROGRESS;
    case MENU_B_PRESSED:
    case 1: // No
        HideSaveInfoWindow();
        HideSaveMessageWindow();
        return SAVE_CANCELED;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveSavingMessageCallback(void)
{
    ShowSaveMessage(gText_SavingDontTurnOff, SaveDoSaveCallback);
    return SAVE_IN_PROGRESS;
}

static u8 SaveDoSaveCallback(void)
{
    u8 saveStatus;

    IncrementGameStat(GAME_STAT_SAVED_GAME);
    PausePyramidChallenge();

    if (gDifferentSaveFile == TRUE)
    {
        saveStatus = TrySavingData(SAVE_OVERWRITE_DIFFERENT_FILE);
        gDifferentSaveFile = FALSE;
    }
    else
    {
        saveStatus = TrySavingData(SAVE_NORMAL);
    }

    if (saveStatus == SAVE_STATUS_OK)
        ShowSaveMessage(gText_PlayerSavedGame, SaveSuccessCallback);
    else
        ShowSaveMessage(gText_SaveError, SaveErrorCallback);

    SaveStartTimer();
    return SAVE_IN_PROGRESS;
}

static u8 SaveSuccessCallback(void)
{
    if (!IsTextPrinterActive(0))
    {
        PlaySE(SE_SAVE);
        sSaveDialogCallback = SaveReturnSuccessCallback;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveReturnSuccessCallback(void)
{
    if (!IsSEPlaying() && SaveSuccesTimer())
    {
        HideSaveInfoWindow();
        return SAVE_SUCCESS;
    }
    else
    {
        return SAVE_IN_PROGRESS;
    }
}

static u8 SaveErrorCallback(void)
{
    if (!IsTextPrinterActive(0))
    {
        PlaySE(SE_BOO);
        sSaveDialogCallback = SaveReturnErrorCallback;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveReturnErrorCallback(void)
{
    if (!SaveErrorTimer())
    {
        return SAVE_IN_PROGRESS;
    }
    else
    {
        HideSaveInfoWindow();
        return SAVE_ERROR;
    }
}

static void InitBattlePyramidRetire(void)
{
    sSaveDialogCallback = BattlePyramidConfirmRetireCallback;
    sSavingComplete = FALSE;
}

static u8 BattlePyramidConfirmRetireCallback(void)
{
    if (IsStartMenuScrollBgActive())
        StartMenu_DisableScrollingBg();

    ClearStdWindowAndFrame(GetStartMenuWindowId(), FALSE);
    RemoveStartMenuWindow();
    ShowSaveMessage(gText_BattlePyramidConfirmRetire, BattlePyramidRetireYesNoCallback);

    return SAVE_IN_PROGRESS;
}

static u8 BattlePyramidRetireYesNoCallback(void)
{
    DisplayYesNoMenuWithDefault(1); // Show Yes/No menu (No selected as default)
    sSaveDialogCallback = BattlePyramidRetireInputCallback;

    return SAVE_IN_PROGRESS;
}

static u8 BattlePyramidRetireInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        return SAVE_CANCELED;
    case MENU_B_PRESSED:
    case 1: // No
        HideSaveMessageWindow();
        return SAVE_SUCCESS;
    }

    return SAVE_IN_PROGRESS;
}

static void VBlankCB_LinkBattleSave(void)
{
    TransferPlttBuffer();
}

static bool32 InitSaveWindowAfterLinkBattle(u8 *state)
{
    switch (*state)
    {
    case 0:
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
        SetVBlankCallback(NULL);
        ScanlineEffect_Stop();
        DmaClear16(3, PLTT, PLTT_SIZE);
        DmaFillLarge16(3, 0, (void *)VRAM, VRAM_SIZE, 0x1000);
        break;
    case 1:
        ResetSpriteData();
        ResetTasks();
        ResetPaletteFade();
        ScanlineEffect_Clear();
        break;
    case 2:
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sBgTemplates_LinkBattleSave, ARRAY_COUNT(sBgTemplates_LinkBattleSave));
        InitWindows(sWindowTemplates_LinkBattleSave);
        LoadUserWindowBorderGfx_(0, 8, BG_PLTT_ID(14));
        Menu_LoadStdPalAt(BG_PLTT_ID(15));
        break;
    case 3:
        ShowBg(0);
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        SetVBlankCallback(VBlankCB_LinkBattleSave);
        EnableInterrupts(1);
        break;
    case 4:
        return TRUE;
    }

    (*state)++;
    return FALSE;
}

void CB2_SetUpSaveAfterLinkBattle(void)
{
    if (InitSaveWindowAfterLinkBattle(&gMain.state))
    {
        CreateTask(Task_SaveAfterLinkBattle, 0x50);
        SetMainCallback2(CB2_SaveAfterLinkBattle);
    }
}

static void CB2_SaveAfterLinkBattle(void)
{
    RunTasks();
    UpdatePaletteFade();
}

static void Task_SaveAfterLinkBattle(u8 taskId)
{
    s16 *state = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        switch (*state)
        {
        case 0:
            FillWindowPixelBuffer(0, PIXEL_FILL(1));
            AddTextPrinterParameterized2(0,
                                        FONT_NORMAL,
                                        gText_SavingDontTurnOffPower,
                                        TEXT_SKIP_DRAW,
                                        NULL,
                                        TEXT_COLOR_DARK_GRAY,
                                        TEXT_COLOR_WHITE,
                                        TEXT_COLOR_LIGHT_GRAY);
            DrawTextBorderOuter(0, 8, 14);
            PutWindowTilemap(0);
            CopyWindowToVram(0, COPYWIN_FULL);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);

            if (gWirelessCommType != 0 && InUnionRoom())
            {
                if (Link_AnyPartnersPlayingFRLG_JP())
                {
                    *state = 1;
                }
                else
                {
                    *state = 5;
                }
            }
            else
            {
                gSoftResetDisabled = TRUE;
                *state = 1;
            }
            break;
        case 1:
            SetContinueGameWarpStatusToDynamicWarp();
            WriteSaveBlock2();
            *state = 2;
            break;
        case 2:
            if (WriteSaveBlock1Sector())
            {
                ClearContinueGameWarpStatus2();
                *state = 3;
                gSoftResetDisabled = FALSE;
            }
            break;
        case 3:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            *state = 4;
            break;
        case 4:
            FreeAllWindowBuffers();
            SetMainCallback2(gMain.savedCallback);
            DestroyTask(taskId);
            break;
        case 5:
            CreateTask(Task_LinkFullSave, 5);
            *state = 6;
            break;
        case 6:
            if (!FuncIsActiveTask(Task_LinkFullSave))
            {
                *state = 3;
            }
            break;
        }
    }
}

static void ShowSaveInfoWindow(void)
{
    struct WindowTemplate saveInfoWindow = sSaveInfoWindowTemplate;
    u8 gender;
    u8 color;
    u32 xOffset;
    u32 yOffset;

    if (!FlagGet(FLAG_SYS_POKEDEX_GET))
    {
        saveInfoWindow.height -= 2;
    }

    sSaveInfoWindowId = AddWindow(&saveInfoWindow);
    DrawStdWindowFrame(sSaveInfoWindowId, FALSE);

    gender = gSaveBlock2Ptr->playerGender;
    color = TEXT_COLOR_RED;  // Red when female, blue when male.

    if (gender == MALE)
    {
        color = TEXT_COLOR_BLUE;
    }

    // Print region name
    yOffset = 1;
    BufferSaveMenuText(SAVE_MENU_LOCATION, gStringVar4, TEXT_COLOR_GREEN);
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gStringVar4, 0, yOffset, TEXT_SKIP_DRAW, NULL);

    // Print player name
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gText_SavingPlayer, 0, yOffset, TEXT_SKIP_DRAW, NULL);
    BufferSaveMenuText(SAVE_MENU_NAME, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x70);
    PrintPlayerNameOnWindow(sSaveInfoWindowId, gStringVar4, xOffset, yOffset);

    // Print badge count
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gText_SavingBadges, 0, yOffset, TEXT_SKIP_DRAW, NULL);
    BufferSaveMenuText(SAVE_MENU_BADGES, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x70);
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gStringVar4, xOffset, yOffset, TEXT_SKIP_DRAW, NULL);

    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
    {
        // Print Pokédex count
        yOffset += 16;
        AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gText_SavingPokedex, 0, yOffset, TEXT_SKIP_DRAW, NULL);
        BufferSaveMenuText(SAVE_MENU_CAUGHT, gStringVar4, color);
        xOffset = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x70);
        AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gStringVar4, xOffset, yOffset, TEXT_SKIP_DRAW, NULL);
    }

    // Print play time
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gText_SavingTime, 0, yOffset, TEXT_SKIP_DRAW, NULL);
    BufferSaveMenuText(SAVE_MENU_PLAY_TIME, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x70);
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gStringVar4, xOffset, yOffset, TEXT_SKIP_DRAW, NULL);

    CopyWindowToVram(sSaveInfoWindowId, COPYWIN_GFX);
}

static void RemoveSaveInfoWindow(void)
{
    ClearStdWindowAndFrame(sSaveInfoWindowId, FALSE);
    RemoveWindow(sSaveInfoWindowId);
}

static void Task_WaitForBattleTowerLinkSave(u8 taskId)
{
    if (!FuncIsActiveTask(Task_LinkFullSave))
    {
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

#define tInBattleTower data[2]

void SaveForBattleTowerLink(void)
{
    u8 taskId = CreateTask(Task_LinkFullSave, 5);
    gTasks[taskId].tInBattleTower = TRUE;
    gTasks[CreateTask(Task_WaitForBattleTowerLinkSave, 6)].data[1] = taskId;
}

#undef tInBattleTower

static void HideStartMenuWindow(void)
{
    StartMenu_DisableScrollingBg();
    ClearStdWindowAndFrame(GetStartMenuWindowId(), TRUE);
    RemoveStartMenuWindow();
    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
}

static void StartMenu_BuildGridActionMap(void)
{
    s8 i;
    s8 selectedSlot = -1;

    for (i = 0; i < (s8)ARRAY_COUNT(sStartMenuGridActionIndices); i++)
        sStartMenuGridActionIndices[i] = -1;

    // Compact the grid from top-left to bottom-right with currently available actions.
    for (i = 0; i < sNumStartMenuActions && i < (s8)ARRAY_COUNT(sStartMenuGridActionIndices); i++)
        sStartMenuGridActionIndices[i] = i;

    for (i = 0; i < (s8)ARRAY_COUNT(sStartMenuGridActionIndices); i++)
    {
        if (sStartMenuGridActionIndices[i] == sStartMenuCursorPos)
        {
            selectedSlot = i;
            break;
        }
    }

    if (selectedSlot < 0)
    {
        for (i = 0; i < (s8)ARRAY_COUNT(sStartMenuGridActionIndices); i++)
        {
            if (sStartMenuGridActionIndices[i] >= 0)
            {
                selectedSlot = i;
                break;
            }
        }
    }

    if (selectedSlot < 0)
        selectedSlot = 0;

    sStartMenuSelectedGridSlot = selectedSlot;
    sStartMenuCursorPos = sStartMenuGridActionIndices[sStartMenuSelectedGridSlot];
}

static void StartMenu_RemoveButtonTextWindow(void)
{
    u8 i;

    for (i = 0; i < sStartMenuButtonTextWindowCount; i++)
    {
        RemoveWindow(sStartMenuButtonTextWindowIds[i]);
    }
    sStartMenuButtonTextWindowCount = 0;
    sStartMenuClockWindowActive = FALSE;
    sStartMenuClockValueValid = FALSE;
}

static void StartMenu_LoadTextPalette(void)
{
    static const u16 sStartMenuStrongBlack = RGB_BLACK;

    StartMenu_BuildButtonPaletteByGender();

    // Keep button-matched window bg, with default FONT_NORMAL fg/shadow.
    LoadPalette(&sStartMenuButtonPaletteDynamic[2],           BG_PLTT_ID(START_MENU_TEXT_PAL_SLOT) + 1, PLTT_SIZEOF(1)); // idx 1 bg
    LoadPalette(&sStartMenuStrongBlack,                       BG_PLTT_ID(START_MENU_TEXT_PAL_SLOT) + 2, PLTT_SIZEOF(1)); // idx 2 fg
    LoadPalette(&gStandardMenuPalette[TEXT_COLOR_LIGHT_GRAY], BG_PLTT_ID(START_MENU_TEXT_PAL_SLOT) + 3, PLTT_SIZEOF(1)); // idx 3 shadow
    LoadPalette(&sStartMenuStrongBlack,                       BG_PLTT_ID(START_MENU_TEXT_PAL_SLOT) + 5, PLTT_SIZEOF(1)); // idx 5 bar fill
}

static void StartMenu_BuildButtonPaletteByGender(void)
{
    u32 i;
    bool8 isMale;

    for (i = 0; i < ARRAY_COUNT(sStartMenuButtonPalette); i++)
        sStartMenuButtonPaletteDynamic[i] = sStartMenuButtonPalette[i];

    isMale = (gSaveBlock2Ptr->playerGender == MALE);

    if (isMale)
    {
        // Replace old gray shades: #B3B3B3, #7D7D7D, #444444
        sStartMenuButtonPaletteDynamic[1] = RGB(16, 21, 31); // #84ADFF
        sStartMenuButtonPaletteDynamic[2] = RGB(8,  9,  22); // #424AB5
        sStartMenuButtonPaletteDynamic[4] = RGB(5,  4,  12); // #292163
    }
    else
    {
        sStartMenuButtonPaletteDynamic[1] = RGB(31, 18, 16); // #FF9782
        sStartMenuButtonPaletteDynamic[2] = RGB(22, 9,  9);  // #B34D47
        sStartMenuButtonPaletteDynamic[4] = RGB(12, 5,  5);  // #612929
    }
}

static void StartMenu_DrawButtonText(void)
{
    u8 slot;
    u8 action;
    u8 textX;
    u8 textY;
    u8 windowWidth;
    u8 tilemapLeft;
    u8 tilemapTop;
    u16 textWidth;
    u16 textXAbs;
    u16 textYAbs;
    u16 baseBlock;
    u8 windowId;
    const u8 *label;
    s8 actionIndex;
    struct WindowTemplate winTemplate;

    StartMenu_RemoveButtonTextWindow();
    StartMenu_LoadTextPalette();
    baseBlock = START_MENU_BUTTON_TEXT_BASEBLOCK;

    for (slot = 0; slot < 9; slot++)
    {
        actionIndex = sStartMenuGridActionIndices[slot];
        if (actionIndex < 0)
            continue;

        action = sCurrentStartMenuActions[actionIndex];
        StringExpandPlaceholders(gStringVar4, sStartMenuItems[action].text);
        label = gStringVar4;
        textWidth = GetStringWidth(FONT_NORMAL, label, 0);
        textXAbs = START_MENU_BUTTON_GRID_X * 8 + (slot % 3) * 64 + ((64 - textWidth) / 2);
        if ((slot % 3) == 1)
            textXAbs += 8;
        else if ((slot % 3) == 2)
            textXAbs += 16;
        textYAbs = START_MENU_BUTTON_GRID_Y * 8 + (slot / 3) * 32 + 12;
        if ((slot / 3) == 1)
            textYAbs += 8;
        else if ((slot / 3) == 2)
            textYAbs += 16;
        tilemapLeft = textXAbs / 8;
        tilemapTop = textYAbs / 8;
        textX = textXAbs % 8;
        textY = textYAbs % 8;
        windowWidth = (textWidth + textX + 7) / 8;
        if (windowWidth == 0)
            windowWidth = 1;
        if (windowWidth > 8)
            windowWidth = 8;

        winTemplate.bg = 0;
        winTemplate.tilemapLeft = tilemapLeft;
        winTemplate.tilemapTop = tilemapTop;
        winTemplate.width = windowWidth;
        winTemplate.height = 2;
        winTemplate.paletteNum = START_MENU_TEXT_PAL_SLOT;
        winTemplate.baseBlock = baseBlock;

        windowId = AddWindow(&winTemplate);
        sStartMenuButtonTextWindowIds[sStartMenuButtonTextWindowCount++] = windowId;
        FillWindowPixelBuffer(windowId, PIXEL_FILL(sStartMenuButtonTextColors[0]));
        AddTextPrinterParameterized3(windowId, FONT_NORMAL, textX, textY, sStartMenuButtonTextColors, TEXT_SKIP_DRAW, label);
        PutWindowTilemap(windowId);
        CopyWindowToVram(windowId, COPYWIN_FULL);
        baseBlock += windowWidth * 2;
    }

    // Clock centered on the top black bar.
    winTemplate.bg = 0;
    winTemplate.tilemapLeft = 12;
    winTemplate.tilemapTop = 0;
    winTemplate.width = 6;
    winTemplate.height = 2;
    winTemplate.paletteNum = START_MENU_TEXT_PAL_SLOT;
    winTemplate.baseBlock = baseBlock;
    sStartMenuClockWindowId = AddWindow(&winTemplate);
    sStartMenuClockWindowActive = TRUE;
    sStartMenuButtonTextWindowIds[sStartMenuButtonTextWindowCount++] = sStartMenuClockWindowId;
    baseBlock += winTemplate.width * 2;
    StartMenu_UpdateClockWindow();
}

static void StartMenu_UpdateClockWindow(void)
{
    u8 x;
    u16 textWidth;

    if (!sStartMenuClockWindowActive)
        return;

    RtcCalcLocalTime();
    if (sStartMenuClockValueValid
        && gLocalTime.hours == sStartMenuClockHour
        && gLocalTime.minutes == sStartMenuClockMinute)
        return;

    sStartMenuClockValueValid = TRUE;
    sStartMenuClockHour = gLocalTime.hours;
    sStartMenuClockMinute = gLocalTime.minutes;

    ConvertIntToDecimalStringN(gStringVar1, gLocalTime.hours, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar1, COMPOUND_STRING(":"));
    ConvertIntToDecimalStringN(gStringVar2, gLocalTime.minutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar1, gStringVar2);

    textWidth = GetStringWidth(FONT_NORMAL, gStringVar1, 0);
    x = (6 * 8 - textWidth) / 2;

    FillWindowPixelBuffer(sStartMenuClockWindowId, PIXEL_FILL(sStartMenuClockTextColors[0]));
    AddTextPrinterParameterized3(sStartMenuClockWindowId, FONT_NORMAL, x, 3, sStartMenuClockTextColors, TEXT_SKIP_DRAW, gStringVar1);
    PutWindowTilemap(sStartMenuClockWindowId);
    CopyWindowToVram(sStartMenuClockWindowId, COPYWIN_FULL);
}

static void StartMenu_DrawButtonGrid(void)
{
    u16 *bg0TilemapBuffer = GetBgTilemapBuffer(0);
    u16 x, y, bx, by;
    u8 slot;

    if (bg0TilemapBuffer == NULL)
        return;

    FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 32, 32);

    for (by = 0; by < START_MENU_BUTTON_GRID_ROWS; by++)
    {
        for (bx = 0; bx < START_MENU_BUTTON_GRID_COLUMNS; bx++)
        {
            const u16 originX = START_MENU_BUTTON_GRID_X + bx * START_MENU_BUTTON_GRID_X_SPACING;
            const u16 originY = START_MENU_BUTTON_GRID_Y + by * START_MENU_BUTTON_GRID_Y_SPACING;
            slot = by * START_MENU_BUTTON_GRID_COLUMNS + bx;
            if (sStartMenuGridActionIndices[slot] < 0)
                continue;

            for (y = 0; y < START_MENU_BUTTON_HEIGHT_TILES; y++)
            {
                for (x = 0; x < START_MENU_BUTTON_WIDTH_TILES; x++)
                {
                    const u16 tileId = START_MENU_BUTTON_BASE_TILE + y * START_MENU_BUTTON_WIDTH_TILES + x;
                    const u16 entry = tileId | (START_MENU_BUTTON_PAL_SLOT << 12);
                    const u16 dstX = originX + x;
                    const u16 dstY = originY + y;
                    bg0TilemapBuffer[dstY * 32 + dstX] = entry;
                }
            }
        }
    }
}

static void StartMenu_DrawEdgeBars(void)
{
    u16 *bg0TilemapBuffer = GetBgTilemapBuffer(0);
    u16 x, y;
    u16 entry;

    if (bg0TilemapBuffer == NULL)
        return;

    entry = START_MENU_BAR_TILE | (START_MENU_TEXT_PAL_SLOT << 12);

    // Top edge bar: rows 0-1 (16 px).
    for (y = 0; y < 2; y++)
    {
        for (x = 0; x < 30; x++)
            bg0TilemapBuffer[y * 32 + x] = entry;
    }

    // Bottom edge bar: rows 18-19 (16 px).
    for (y = 18; y < 20; y++)
    {
        for (x = 0; x < 30; x++)
            bg0TilemapBuffer[y * 32 + x] = entry;
    }
}

static void StartMenu_RefreshCustomMenuVisuals(void)
{
    u16 *bg0TilemapBuffer = GetBgTilemapBuffer(0);
    u16 x, y;
    u8 slotX;
    u8 slotY;
    u16 originX;
    u16 originY;

    if (bg0TilemapBuffer == NULL)
        return;

    // Ensure selected button graphics are valid before writing tilemap entries.
    LoadBgTiles(0, sStartMenuButtonSelectedTiles, sizeof(sStartMenuButtonSelectedTiles), START_MENU_BUTTON_SELECTED_BASE_TILE);

    StartMenu_DrawButtonGrid();
    StartMenu_DrawEdgeBars();

    slotX = sStartMenuSelectedGridSlot % START_MENU_BUTTON_GRID_COLUMNS;
    slotY = sStartMenuSelectedGridSlot / START_MENU_BUTTON_GRID_COLUMNS;
    originX = START_MENU_BUTTON_GRID_X + slotX * START_MENU_BUTTON_GRID_X_SPACING;
    originY = START_MENU_BUTTON_GRID_Y + slotY * START_MENU_BUTTON_GRID_Y_SPACING;

    for (y = 0; y < START_MENU_BUTTON_HEIGHT_TILES; y++)
    {
        for (x = 0; x < START_MENU_BUTTON_WIDTH_TILES; x++)
        {
            const u16 tileId = START_MENU_BUTTON_SELECTED_BASE_TILE + y * START_MENU_BUTTON_WIDTH_TILES + x;
            const u16 entry = tileId | (START_MENU_BUTTON_PAL_SLOT << 12);
            bg0TilemapBuffer[(originY + y) * 32 + (originX + x)] = entry;
        }
    }

    StartMenu_DrawButtonText();
    CopyBgTilemapBufferToVram(0);
}

static bool8 StartMenu_MoveCursorInGrid(s8 dx, s8 dy)
{
    s8 row = sStartMenuSelectedGridSlot / START_MENU_BUTTON_GRID_COLUMNS;
    s8 col = sStartMenuSelectedGridSlot % START_MENU_BUTTON_GRID_COLUMNS;
    s8 step;
    s8 candidateRow;
    s8 candidateCol;
    s8 candidateSlot;

    for (step = 1; step < START_MENU_BUTTON_GRID_COLUMNS + 1; step++)
    {
        candidateRow = (row + dy * step + START_MENU_BUTTON_GRID_ROWS) % START_MENU_BUTTON_GRID_ROWS;
        candidateCol = (col + dx * step + START_MENU_BUTTON_GRID_COLUMNS) % START_MENU_BUTTON_GRID_COLUMNS;
        candidateSlot = candidateRow * START_MENU_BUTTON_GRID_COLUMNS + candidateCol;
        if (sStartMenuGridActionIndices[candidateSlot] >= 0)
        {
            sStartMenuSelectedGridSlot = candidateSlot;
            sStartMenuCursorPos = sStartMenuGridActionIndices[candidateSlot];
            StartMenu_RefreshCustomMenuVisuals();
            return TRUE;
        }
    }

    return FALSE;
}

static void StartMenu_EnableScrollingBg(void)
{
    u16 i;
    u8 bg0CharBase;

    // Avoid forced blank here: on hardware/emulators it shows as a white flash.
    sStartMenuTransitionPendingUnblank = FALSE;
    sStartMenuSavedBg3TilemapBuffer = GetBgTilemapBuffer(3);
    SetBgTilemapBuffer(3, sStartMenuScrollBgTilemapBuffer);
    // BG3 uses mapBaseIndex 30 in overworld, so charbase 3 would overlap map VRAM.
    // Use charbase 1 to keep tile graphics and tilemap in separate VRAM regions.
    SetBgAttribute(3, BG_ATTR_CHARBASEINDEX, 1);
    LoadBgTiles(3, sStartMenuScrollBgTiles, sizeof(sStartMenuScrollBgTiles), 0);
    CopyToBgTilemapBuffer(3, sStartMenuScrollBgTilemap, sizeof(sStartMenuScrollBgTilemap), 0);
    for (i = 0; i < ARRAY_COUNT(sStartMenuScrollBgTilemapBuffer); i++)
        sStartMenuScrollBgTilemapBuffer[i] = (sStartMenuScrollBgTilemapBuffer[i] & 0x0FFF) | (12 << 12);
    LoadPalette(sStartMenuScrollBgPalette, BG_PLTT_ID(12), sizeof(sStartMenuScrollBgPalette));
    ChangeBgY(3, 0, BG_COORD_SET);
    CopyBgTilemapBufferToVram(3);
    HideBg(1);
    HideBg(2);

    // Ensure BG0 is transparent outside menu windows so BG3 background is visible.
    bg0CharBase = GetBgAttribute(0, BG_ATTR_CHARBASEINDEX);
    CpuFill16(0, (void *)BG_CHAR_ADDR(bg0CharBase), TILE_SIZE_4BPP);
    // Build a solid tile using color index 5 (black) for edge bars.
    CpuFill16(0x5555, (void *)(BG_CHAR_ADDR(bg0CharBase) + START_MENU_BAR_TILE * TILE_SIZE_4BPP), TILE_SIZE_4BPP);
    FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 32, 32);
    LoadBgTiles(0, sStartMenuButtonTiles, sizeof(sStartMenuButtonTiles), START_MENU_BUTTON_BASE_TILE);
    LoadBgTiles(0, sStartMenuButtonSelectedTiles, sizeof(sStartMenuButtonSelectedTiles), START_MENU_BUTTON_SELECTED_BASE_TILE);
    StartMenu_BuildButtonPaletteByGender();
    LoadPalette(sStartMenuButtonPaletteDynamic, BG_PLTT_ID(START_MENU_BUTTON_PAL_SLOT), sizeof(sStartMenuButtonPaletteDynamic));
    LoadPalette(&sStartMenuCursorPalette[1], BG_PLTT_ID(START_MENU_BUTTON_PAL_SLOT) + 6, PLTT_SIZEOF(1));
    // Load text fg/shadow in reserved button palette entries.
    StartMenu_LoadTextPalette();
    CopyBgTilemapBufferToVram(0);

    ShowBg(3);
    sStartMenuObjWasEnabled = (GetGpuReg(REG_OFFSET_DISPCNT) & DISPCNT_OBJ_ON) != 0;
    if (sStartMenuObjWasEnabled)
        ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON);
    sStartMenuScrollBgActive = TRUE;
}

static void StartMenu_UpdateScrollingBg(void)
{
    if (sStartMenuScrollBgActive)
    {
        StartMenu_LoadTextPalette();
        StartMenu_UpdateClockWindow();
        // Keep the animation lightweight to avoid transition artifacts.
        LoadPalette(sStartMenuScrollBgPalette, BG_PLTT_ID(12), sizeof(sStartMenuScrollBgPalette));
        ChangeBgY(3, 64, BG_COORD_SUB);
    }
}

static void StartMenu_DisableScrollingBg(void)
{
    u8 bg0CharBase;

    if (!sStartMenuScrollBgActive)
        return;

    sStartMenuScrollBgActive = FALSE;
    sStartMenuTransitionPendingUnblank = FALSE;
    if (sStartMenuObjWasEnabled)
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON);
    HideBg(1);
    HideBg(2);
    if (sStartMenuSavedBg3TilemapBuffer != NULL)
    {
        SetBgTilemapBuffer(3, sStartMenuSavedBg3TilemapBuffer);
        sStartMenuSavedBg3TilemapBuffer = NULL;
    }
    CopyMapTilesetsToVram(gMapHeader.mapLayout);
    LoadMapTilesetPalettes(gMapHeader.mapLayout);
    InitTilesetAnimations();
    UpdateTilesetAnimations();
    TransferTilesetAnimsBuffer();
    SetBgAttribute(3, BG_ATTR_CHARBASEINDEX, 0);
    ShowBg(3);
    ChangeBgY(3, 0, BG_COORD_SET);

    // Remove the temporary fixed button grid from BG0 when closing Start Menu.
    bg0CharBase = GetBgAttribute(0, BG_ATTR_CHARBASEINDEX);
    CpuFill16(0, (void *)BG_CHAR_ADDR(bg0CharBase), TILE_SIZE_4BPP);
    FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 32, 32);
    CopyBgTilemapBufferToVram(0);
    StartMenu_RemoveButtonTextWindow();

    DrawWholeMapView();
    CopyBgTilemapBufferToVram(1);
    CopyBgTilemapBufferToVram(2);
    CopyBgTilemapBufferToVram(3);
    ShowBg(1);
    ShowBg(2);
}

void HideStartMenu(void)
{
    PlaySE(SE_SELECT);
    HideStartMenuWindow();
}

bool8 IsStartMenuScrollBgActive(void)
{
    return sStartMenuScrollBgActive;
}

void StartMenu_RefreshForReturnFade(void)
{
    if (!sStartMenuScrollBgActive)
        return;

    LoadBgTiles(0, sStartMenuButtonTiles, sizeof(sStartMenuButtonTiles), START_MENU_BUTTON_BASE_TILE);
    LoadBgTiles(0, sStartMenuButtonSelectedTiles, sizeof(sStartMenuButtonSelectedTiles), START_MENU_BUTTON_SELECTED_BASE_TILE);
    StartMenu_BuildButtonPaletteByGender();
    LoadPalette(sStartMenuButtonPaletteDynamic, BG_PLTT_ID(START_MENU_BUTTON_PAL_SLOT), sizeof(sStartMenuButtonPaletteDynamic));
    LoadPalette(&sStartMenuCursorPalette[1], BG_PLTT_ID(START_MENU_BUTTON_PAL_SLOT) + 6, PLTT_SIZEOF(1));
    StartMenu_LoadTextPalette();
    StartMenu_RefreshCustomMenuVisuals();
}

void StartMenu_PrepareForReturnInput(void)
{
    if (InUnionRoom() == TRUE)
        SetUsingUnionRoomStartMenu();

    StartMenu_RefreshForReturnFade();
    gMenuCallback = HandleStartMenuInput;
}

void StartMenu_ProcessTransition(void)
{
    if (sStartMenuTransitionPendingUnblank && !IsDma3ManagerBusyWithBgCopy())
    {
        ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_FORCED_BLANK);
        sStartMenuTransitionPendingUnblank = FALSE;
    }
}

void AppendToList(u8 *list, u8 *pos, u8 newEntry)
{
    list[*pos] = newEntry;
    (*pos)++;
}

static bool8 StartMenuDexNavCallback(void)
{
    CreateTask(Task_OpenDexNavFromStartMenu, 0);
    return TRUE;
}

void Script_ForceSaveGame(struct ScriptContext *ctx)
{
    SaveGame();
    ShowSaveInfoWindow();
    gMenuCallback = SaveCallback;
    sSaveDialogCallback = SaveSavingMessageCallback;
}
