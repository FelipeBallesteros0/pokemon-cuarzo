#include "global.h"
#include "ui_main_menu.h"
#include "strings.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item.h"
#include "international_string_util.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "party_menu.h"
#include "scanline_effect.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text_window.h"
#include "overworld.h"
#include "main_menu.h"
#include "option_menu.h"
#include "mystery_event_menu.h"
#include "mystery_gift_menu.h"
#include "link.h"
#include "pokemon_icon.h"
#include "pokedex.h"
#include "region_map.h"
#include "title_screen.h"
#include "constants/songs.h"
#include "constants/rgb.h"

struct MainMenuResources
{
    MainCallback savedCallback;
    u8 gfxLoadState;
    u16 iconBoxSpriteIds[6];
    u16 iconMonSpriteIds[6];
    u16 mugshotSpriteId;
    u8 sSelectedOption;
};

enum WindowIds
{
    WINDOW_HEADER,
    WINDOW_MIDDLE,
    WINDOW_BTN_CONTINUE,
    WINDOW_BTN_NEWGAME,
    WINDOW_BTN_OPTIONS,
    WINDOW_BTN_MGIFT,
    WINDOW_BTN_MEVENTS,
    WINDOW_COUNT,
};

enum {
    HW_WIN_CONTINUE,
    HW_WIN_NEW_GAME,
    HW_WIN_OPTIONS,
    HW_WIN_MYSTERY_GIFT,
    HW_WIN_MYSTERY_EVENT,
    HW_WIN_MYSTERY_BOTH,
    HW_WIN_NO_SAVE_NEW_GAME,  // no-save: only 2 options
    HW_WIN_NO_SAVE_OPTIONS,
};

enum Colors { FONT_BLACK, FONT_WHITE, FONT_RED, FONT_BLUE };

enum {
    HAS_NO_SAVED_GAME,
    HAS_SAVED_GAME,
    HAS_MYSTERY_GIFT,
    HAS_MYSTERY_EVENTS,
};

#define try_free(ptr) ({        \
    void ** ptr__ = (void **)&(ptr);   \
    if (*ptr__ != NULL)                \
        Free(*ptr__);                  \
})

static EWRAM_DATA struct MainMenuResources *sMainMenuDataPtr = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
static EWRAM_DATA u8 *sBg2TilemapBuffer = NULL;
static EWRAM_DATA u8 sSelectedOption = 0;
static EWRAM_DATA u8 menuType = 0;

static void MainMenu_RunSetup(void);
static bool8 MainMenu_DoGfxSetup(void);
static bool8 MainMenu_InitBgs(void);
static void MainMenu_FadeAndBail(void);
static bool8 MainMenu_LoadGraphics(void);
static void MainMenu_InitWindows(void);
static void PrintToWindow(u8 windowId, u8 colorIdx);
static void Task_MainMenuWaitFadeIn(u8 taskId);
static void Task_MainMenuMain(u8 taskId);
static void MainMenu_InitializeGPUWindows(void);
static void CreateMugshot(void);
static void DestroyMugshot(void);
static void CreateIconShadow(void);
static void DestroyIconShadow(void);
static u32  GetHPEggCyclePercent(u32 partyIndex);
static void CreatePartyMonIcons(void);
static void DestroyMonIcons(void);

static const struct BgTemplate sMainMenuBgTemplates[] =
{
    { .bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .priority = 0 },
    { .bg = 1, .charBaseIndex = 3, .mapBaseIndex = 30, .priority = 2 },
    { .bg = 2, .charBaseIndex = 2, .mapBaseIndex = 28, .priority = 2 },
};

static const struct WindowTemplate sMainMenuWindowTemplates[] =
{
    [WINDOW_HEADER] =
    {
        // tilemapTop=0: misma fila que CONTINUAR, cols 16-29 (derecha del panel)
        .bg = 0, .tilemapLeft = 14, .tilemapTop = 0,
        .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 1,
    },
    [WINDOW_MIDDLE] =
    {
        .bg = 0, .tilemapLeft = 8, .tilemapTop = 4,
        .width = 18, .height = 7, .paletteNum = 15, .baseBlock = 1 + (18 * 2),
    },
    // Etiquetas de botones — paleta 15 = paleta de texto estándar del juego
    [WINDOW_BTN_CONTINUE] =
    {
        // tilemapTop=0 (y=0-15), cols 1-15 (~120px), centrado en x≈70 (~50px izq del centro)
        // WINDOW_HEADER ocupa cols 16-29 en la misma fila → sin solape
        .bg = 0, .tilemapLeft = 0, .tilemapTop = 0,
        .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 1 + (18*2) + (18*7),
    },
    [WINDOW_BTN_NEWGAME] =
    {
        .bg = 0, .tilemapLeft = 1, .tilemapTop = 13,
        .width = 13, .height = 2, .paletteNum = 15, .baseBlock = 1 + (18*2) + (18*7) + (28*2),
    },
    [WINDOW_BTN_OPTIONS] =
    {
        .bg = 0, .tilemapLeft = 15, .tilemapTop = 13,
        .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 1 + (18*2) + (18*7) + (28*2) + (13*2),
    },
    [WINDOW_BTN_MGIFT] =
    {
        .bg = 0, .tilemapLeft = 1, .tilemapTop = 17,
        .width = 13, .height = 2, .paletteNum = 15, .baseBlock = 1 + (18*2) + (18*7) + (28*2) + (13*2) + (14*2),
    },
    [WINDOW_BTN_MEVENTS] =
    {
        .bg = 0, .tilemapLeft = 15, .tilemapTop = 17,
        .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 1 + (18*2) + (18*7) + (28*2) + (13*2) + (14*2) + (13*2),
    },
    DUMMY_WIN_TEMPLATE
};

struct HighlightWindowCoords { u8 left; u8 right; };
struct HWWindowPosition { struct HighlightWindowCoords winh; struct HighlightWindowCoords winv; };

static const struct HWWindowPosition HWinCoords[] =
{
    [HW_WIN_CONTINUE]       = {{7, 233},   {0, 89}},  // empieza en y=0 para iluminar la etiqueta CONTINUAR
    [HW_WIN_NEW_GAME]       = {{7, 113},   {103, 122}},
    [HW_WIN_OPTIONS]        = {{120, 233}, {103, 122}},  // 120 = inicio de mi panel Options
    [HW_WIN_MYSTERY_GIFT]   = {{7, 113},   {135, 154}},
    [HW_WIN_MYSTERY_EVENT]  = {{120, 233}, {135, 154}},  // mismo ajuste
    [HW_WIN_MYSTERY_BOTH]   = {{7, 233},   {135, 154}},
    [HW_WIN_NO_SAVE_NEW_GAME] = {{7,   113}, {103, 122}},
    [HW_WIN_NO_SAVE_OPTIONS]  = {{120, 233}, {103, 122}},
};

// ─── Graphics data ────────────────────────────────────────────────────────────
// BG1 main tiles (gender variants + no-save)
// BG1 main tiles — paleta extraída por gbagfx directamente del PNG
static const u32 sMainBgTiles[]       = INCGFX_U32("graphics/ui_main_menu/main_tiles.png",        ".4bpp.smol");
static const u32 sMainBgTilesFem[]    = INCGFX_U32("graphics/ui_main_menu/main_tiles_fem.png",    ".4bpp.smol");
static const u32 sMainBgTilesNoSave[] = INCGFX_U32("graphics/ui_main_menu/main_tiles_nosave.png", ".4bpp.smol");
// Tilemaps completos (para menú con partida)
static const u32 sMainBgTilemap[]    = INCBIN_U32("graphics/ui_main_menu/main_tiles.bin.smolTM");
static const u32 sMainBgTilemapFem[] = INCBIN_U32("graphics/ui_main_menu/main_tiles_fem.bin.smolTM");
// Tilemaps limitados (solo filas 13-15, para menú sin partida)
static const u32 sMainBgTilemapNoSave[]    = INCBIN_U32("graphics/ui_main_menu/main_tiles_nosave_limited.bin.smolTM");
static const u32 sMainBgTilemapNSNoSave[]  = INCBIN_U32("graphics/ui_main_menu/main_tiles_nosave_nosave_limited.bin.smolTM");
// Paletas desde PNG
static const u16 sMainBgPalette[]       = INCGFX_U16("graphics/ui_main_menu/main_tiles.png",        ".gbapal");
static const u16 sMainBgPaletteFem[]    = INCGFX_U16("graphics/ui_main_menu/main_tiles_fem.png",    ".gbapal");
static const u16 sMainBgPaletteNoSave[] = INCGFX_U16("graphics/ui_main_menu/main_tiles_nosave.png", ".gbapal");

// BG2 scroll tiles
static const u32 sScrollBgTiles[]   = INCGFX_U32("graphics/ui_main_menu/scroll_tiles.png", ".4bpp.smol");
static const u32 sScrollBgTilemap[] = INCBIN_U32("graphics/ui_main_menu/scroll_tiles.bin.smolTM");
static const u16 sScrollBgPalette[] = INCGFX_U16("graphics/ui_main_menu/scroll_tiles.png", ".gbapal");

// OBJ sprites: icon shadow
static const u16 sIconBox_Pal[]       = INCGFX_U16("graphics/ui_main_menu/icon_shadow.png",     ".gbapal");
static const u32 sIconBox_Gfx[]       = INCGFX_U32("graphics/ui_main_menu/icon_shadow.png",     ".4bpp.smol");
static const u16 sIconBox_PalFem[]    = INCGFX_U16("graphics/ui_main_menu/icon_shadow_fem.png", ".gbapal");
static const u32 sIconBox_GfxFem[]    = INCGFX_U32("graphics/ui_main_menu/icon_shadow_fem.png", ".4bpp.smol");

// OBJ sprites: mugshots
static const u16 sBrendanMugshot_Pal[] = INCGFX_U16("graphics/ui_main_menu/brendan_mugshot.png", ".gbapal");
static const u32 sBrendanMugshot_Gfx[] = INCGFX_U32("graphics/ui_main_menu/brendan_mugshot.png", ".4bpp.smol");
static const u16 sMayMugshot_Pal[]     = INCGFX_U16("graphics/ui_main_menu/may_mugshot.png",     ".gbapal");
static const u32 sMayMugshot_Gfx[]     = INCGFX_U32("graphics/ui_main_menu/may_mugshot.png",     ".4bpp.smol");

// ─── Sprite templates ─────────────────────────────────────────────────────────
#define TAG_MUGSHOT  30012
#define TAG_ICON_BOX 30006

static const struct OamData sOamData_Mugshot =
{
    .size = SPRITE_SIZE(64x64), .shape = SPRITE_SHAPE(64x64), .priority = 1,
};
static const struct OamData sOamData_IconBox =
{
    .size = SPRITE_SIZE(32x32), .shape = SPRITE_SHAPE(32x32), .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_BrendanMugshot =
    { .data = sBrendanMugshot_Gfx, .size = 64*64/2, .tag = TAG_MUGSHOT };
static const struct SpritePalette sSpritePal_BrendanMugshot =
    { .data = sBrendanMugshot_Pal, .tag = TAG_MUGSHOT };
static const struct CompressedSpriteSheet sSpriteSheet_MayMugshot =
    { .data = sMayMugshot_Gfx, .size = 64*64/2, .tag = TAG_MUGSHOT };
static const struct SpritePalette sSpritePal_MayMugshot =
    { .data = sMayMugshot_Pal, .tag = TAG_MUGSHOT };

static const struct CompressedSpriteSheet sSpriteSheet_IconBox =
    { .data = sIconBox_Gfx, .size = 32*32/2, .tag = TAG_ICON_BOX };
static const struct SpritePalette sSpritePal_IconBox =
    { .data = sIconBox_Pal, .tag = TAG_ICON_BOX };
static const struct CompressedSpriteSheet sSpriteSheet_IconBoxFem =
    { .data = sIconBox_GfxFem, .size = 32*32/2, .tag = TAG_ICON_BOX };
static const struct SpritePalette sSpritePal_IconBoxFem =
    { .data = sIconBox_PalFem, .tag = TAG_ICON_BOX };

static const union AnimCmd sSpriteAnim_Mugshot[] =
    { ANIMCMD_FRAME(0, 32), ANIMCMD_JUMP(0) };
static const union AnimCmd *const sSpriteAnimTable_Mugshot[] =
    { sSpriteAnim_Mugshot };

static const struct SpriteTemplate sSpriteTemplate_Mugshot =
{
    .tileTag = TAG_MUGSHOT, .paletteTag = TAG_MUGSHOT,
    .oam = &sOamData_Mugshot, .anims = sSpriteAnimTable_Mugshot,
    .images = NULL, .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};
static const struct SpriteTemplate sSpriteTemplate_IconBox =
{
    .tileTag = TAG_ICON_BOX, .paletteTag = TAG_ICON_BOX,
    .oam = &sOamData_IconBox, .anims = sSpriteAnimTable_Mugshot,
    .images = NULL, .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// ─── Entry point from main_menu.c Task_DisplayMainMenu ───────────────────────
void Task_OpenMainMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!gPaletteFade.active)
    {
        menuType = data[0];
        CleanupOverworldWindowsAndTilemaps();
        MainMenu_Init(CB2_InitTitleScreen);
        DestroyTask(taskId);
    }
}

void MainMenu_Init(MainCallback callback)
{
    u32 i;
    if ((sMainMenuDataPtr = AllocZeroed(sizeof(struct MainMenuResources))) == NULL)
    {
        SetMainCallback2(callback);
        return;
    }
    sMainMenuDataPtr->gfxLoadState = 0;
    sMainMenuDataPtr->savedCallback = callback;
    for (i = 0; i < 6; i++)
    {
        sMainMenuDataPtr->iconBoxSpriteIds[i] = SPRITE_NONE;
        sMainMenuDataPtr->iconMonSpriteIds[i] = SPRITE_NONE;
    }
    SetMainCallback2(MainMenu_RunSetup);
}

static void MainMenu_RunSetup(void)
{
    while (1)
        if (MainMenu_DoGfxSetup() == TRUE)
            break;
}

static void MainMenu_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void MainMenu_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ChangeBgY(2, 128, BG_COORD_SUB);
}

static void MainMenu_FreeResources(void)
{
    try_free(sMainMenuDataPtr);
    try_free(sBg1TilemapBuffer);
    try_free(sBg2TilemapBuffer);
    FreeAllWindowBuffers();
    DestroyMugshot();
    DestroyIconShadow();
    if (menuType != HAS_NO_SAVED_GAME)
        DestroyMonIcons();
    DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
}

static void Task_MainMenuWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sMainMenuDataPtr->savedCallback);
        MainMenu_FreeResources();
        DestroyTask(taskId);
    }
}

static void MainMenu_FadeAndBail(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_MainMenuWaitFadeAndBail, 0);
    SetVBlankCallback(MainMenu_VBlankCB);
    SetMainCallback2(MainMenu_MainCB);
}

static void Task_MainMenuWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_MainMenuMain;
}

static void Task_MainMenuTurnOff(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WIN1H, 0);
        SetGpuReg(REG_OFFSET_WIN1V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        SetMainCallback2(sMainMenuDataPtr->savedCallback);
        MainMenu_FreeResources();
        DestroyTask(taskId);
    }
}

static bool8 MainMenu_DoGfxSetup(void)
{
    switch (gMain.state)
    {
    case 0:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        ResetVramOamAndBgCntRegs();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        MainMenu_InitializeGPUWindows();
        gMain.state++;
        break;
    case 2:
        if (MainMenu_InitBgs())
        {
            sMainMenuDataPtr->gfxLoadState = 0;
            gMain.state++;
        }
        else
        {
            MainMenu_FadeAndBail();
            return TRUE;
        }
        break;
    case 3:
        if (MainMenu_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 4:
        LoadMessageBoxAndBorderGfx();
        MainMenu_InitWindows();
        gMain.state++;
        break;
    case 5:
        PrintToWindow(WINDOW_HEADER, FONT_WHITE);
        CreateIconShadow();
        if (menuType != HAS_NO_SAVED_GAME)
            CreatePartyMonIcons();
        CreateMugshot();
        CreateTask(Task_MainMenuWaitFadeIn, 0);
        BlendPalettes(0xFFFFFFFF, 16, RGB_BLACK);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(MainMenu_VBlankCB);
        SetMainCallback2(MainMenu_MainCB);
        return TRUE;
    }
    return FALSE;
}

static bool8 MainMenu_InitBgs(void)
{
    ResetAllBgsCoordinates();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sMainMenuBgTemplates, NELEMS(sMainMenuBgTemplates));

    sBg1TilemapBuffer = Alloc(0x800);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;
    CpuFill16(0, sBg1TilemapBuffer, 0x800);
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    sBg2TilemapBuffer = Alloc(0x800);
    if (sBg2TilemapBuffer == NULL)
        return FALSE;
    CpuFill16(0, sBg2TilemapBuffer, 0x800);
    SetBgTilemapBuffer(2, sBg2TilemapBuffer);
    ScheduleBgCopyTilemapToVram(2);

    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    return TRUE;
}

static void MainMenu_InitializeGPUWindows(void)
{
    sSelectedOption = (menuType == HAS_NO_SAVED_GAME) ? HW_WIN_NO_SAVE_NEW_GAME : HW_WIN_CONTINUE;
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN1_ON | DISPCNT_WIN0_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(HWinCoords[sSelectedOption].winh.left, HWinCoords[sSelectedOption].winh.right));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(HWinCoords[sSelectedOption].winv.left, HWinCoords[sSelectedOption].winv.right));
    switch (menuType)
    {
    case HAS_NO_SAVED_GAME:
        SetGpuReg(REG_OFFSET_WIN1H, 0);
        SetGpuReg(REG_OFFSET_WIN1V, 0);
        break;
    case HAS_SAVED_GAME:
        SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(HWinCoords[HW_WIN_MYSTERY_BOTH].winh.left, HWinCoords[HW_WIN_MYSTERY_BOTH].winh.right));
        SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(HWinCoords[HW_WIN_MYSTERY_BOTH].winv.left, HWinCoords[HW_WIN_MYSTERY_BOTH].winv.right));
        break;
    case HAS_MYSTERY_GIFT:
        SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(HWinCoords[HW_WIN_MYSTERY_EVENT].winh.left, HWinCoords[HW_WIN_MYSTERY_EVENT].winh.right));
        SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(HWinCoords[HW_WIN_MYSTERY_EVENT].winv.left, HWinCoords[HW_WIN_MYSTERY_EVENT].winv.right));
        break;
    }
    SetGpuReg(REG_OFFSET_WININ,  (WININ_WIN1_BG0 | WININ_WIN1_BG2) | (WININ_WIN0_BG_ALL | WININ_WIN0_OBJ));
    SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_ALL);
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_EFFECT_DARKEN | BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG1 | BLDCNT_TGT1_OBJ);
    SetGpuReg(REG_OFFSET_BLDY, 7);
}

static void MoveHWindowsWithInput(void)
{
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(HWinCoords[sSelectedOption].winh.left, HWinCoords[sSelectedOption].winh.right));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(HWinCoords[sSelectedOption].winv.left, HWinCoords[sSelectedOption].winv.right));
}

static bool8 MainMenu_LoadGraphics(void)
{
    switch (sMainMenuDataPtr->gfxLoadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        if (menuType == HAS_NO_SAVED_GAME)
            DecompressAndCopyTileDataToVram(1, sMainBgTilesNoSave, 0, 0, 0);
        else if (gSaveBlock2Ptr->playerGender == MALE)
            DecompressAndCopyTileDataToVram(1, sMainBgTiles, 0, 0, 0);
        else
            DecompressAndCopyTileDataToVram(1, sMainBgTilesFem, 0, 0, 0);
        sMainMenuDataPtr->gfxLoadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            if (menuType == HAS_NO_SAVED_GAME)
                DecompressDataWithHeaderWram(sMainBgTilemapNSNoSave, sBg1TilemapBuffer);
            else if (gSaveBlock2Ptr->playerGender == FEMALE)
                DecompressDataWithHeaderWram(sMainBgTilemapFem, sBg1TilemapBuffer);
            else
                DecompressDataWithHeaderWram(sMainBgTilemap, sBg1TilemapBuffer);
            sMainMenuDataPtr->gfxLoadState++;
        }
        break;
    case 2:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(2, sScrollBgTiles, 0, 0, 0);
        sMainMenuDataPtr->gfxLoadState++;
        break;
    case 3:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sScrollBgTilemap, sBg2TilemapBuffer);
            sMainMenuDataPtr->gfxLoadState++;
        }
        break;
    case 4:
        if (menuType == HAS_NO_SAVED_GAME)
        {
            LoadPalette(sMainBgPaletteNoSave, 0, 32);
        }
        else if (gSaveBlock2Ptr->playerGender == MALE)
        {
            LoadCompressedSpriteSheet(&sSpriteSheet_IconBox);
            LoadSpritePalette(&sSpritePal_IconBox);
            LoadCompressedSpriteSheet(&sSpriteSheet_BrendanMugshot);
            LoadSpritePalette(&sSpritePal_BrendanMugshot);
            LoadPalette(sMainBgPalette, 0, 32);
        }
        else
        {
            LoadCompressedSpriteSheet(&sSpriteSheet_IconBoxFem);
            LoadSpritePalette(&sSpritePal_IconBoxFem);
            LoadCompressedSpriteSheet(&sSpriteSheet_MayMugshot);
            LoadSpritePalette(&sSpritePal_MayMugshot);
            LoadPalette(sMainBgPaletteFem, 0, 32);
        }
        LoadPalette(sScrollBgPalette, 16, 32);
        sMainMenuDataPtr->gfxLoadState++;
        break;
    default:
        sMainMenuDataPtr->gfxLoadState = 0;
        return TRUE;
    }
    return FALSE;
}

static const u8 sText_BtnContinuar[]    = _("CONTINUAR");
static const u8 sText_BtnNuevaPartida[] = _("NUEVA PARTIDA");
static const u8 sText_BtnOpciones[]     = _("OPCIONES");
static const u8 sText_BtnRegMisterioso[]= _("REG. MISTERIOSO");
static const u8 sText_BtnEvMisterioso[] = _("EV. MISTERIOSO");

static void PrintButtonLabel(u8 winId, const u8 *text)
{
    // Usar paleta de texto estándar (paletteNum=15): bg=transparent, fg=blanco, shadow=gris
    const u8 colors[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};
    u16 winW = sMainMenuWindowTemplates[winId].width * 8;
    u8 xOff = GetStringCenterAlignXOffset(FONT_NORMAL, text, winW);
    FillWindowPixelBuffer(winId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    AddTextPrinterParameterized4(winId, FONT_NORMAL, xOff, 1, 0, 0, colors, TEXT_SKIP_DRAW, text);
    PutWindowTilemap(winId);
    CopyWindowToVram(winId, 3);
}

static void MainMenu_InitWindows(void)
{
    InitWindows(sMainMenuWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);

    FillWindowPixelBuffer(WINDOW_HEADER, 0);
    PutWindowTilemap(WINDOW_HEADER);
    CopyWindowToVram(WINDOW_HEADER, 3);
    FillWindowPixelBuffer(WINDOW_MIDDLE, 0);
    PutWindowTilemap(WINDOW_MIDDLE);
    CopyWindowToVram(WINDOW_MIDDLE, 3);

    // Etiquetas de botones
    if (menuType == HAS_NO_SAVED_GAME)
    {
        PrintButtonLabel(WINDOW_BTN_NEWGAME, sText_BtnNuevaPartida);
        PrintButtonLabel(WINDOW_BTN_OPTIONS,  sText_BtnOpciones);
    }
    else
    {
        PrintButtonLabel(WINDOW_BTN_CONTINUE, sText_BtnContinuar);
        PrintButtonLabel(WINDOW_BTN_NEWGAME,  sText_BtnNuevaPartida);
        PrintButtonLabel(WINDOW_BTN_OPTIONS,   sText_BtnOpciones);
        if (menuType == HAS_MYSTERY_GIFT || menuType == HAS_MYSTERY_EVENTS)
        {
            PrintButtonLabel(WINDOW_BTN_MGIFT, sText_BtnRegMisterioso);
            if (menuType == HAS_MYSTERY_EVENTS)
                PrintButtonLabel(WINDOW_BTN_MEVENTS, sText_BtnEvMisterioso);
        }
    }
}

// ─── Sprite creation ──────────────────────────────────────────────────────────
static void CreateMugshot(void)
{
    if (menuType == HAS_NO_SAVED_GAME)
    {
        sMainMenuDataPtr->mugshotSpriteId = SPRITE_NONE;
        return;
    }
    sMainMenuDataPtr->mugshotSpriteId = CreateSprite(&sSpriteTemplate_Mugshot, 48, 56, 1);
    gSprites[sMainMenuDataPtr->mugshotSpriteId].invisible = FALSE;
    StartSpriteAnim(&gSprites[sMainMenuDataPtr->mugshotSpriteId], 0);
    gSprites[sMainMenuDataPtr->mugshotSpriteId].oam.priority = 0;
}

static void DestroyMugshot(void)
{
    if (sMainMenuDataPtr != NULL && sMainMenuDataPtr->mugshotSpriteId != SPRITE_NONE)
    {
        DestroySprite(&gSprites[sMainMenuDataPtr->mugshotSpriteId]);
        sMainMenuDataPtr->mugshotSpriteId = SPRITE_NONE;
    }
}

#define ICON_BOX_1_START_X  144
#define ICON_BOX_1_START_Y  38
#define ICON_BOX_X_DIFF     32
#define ICON_BOX_Y_DIFF     32

static void CreateIconShadow(void)
{
    u8 i;
    if (menuType == HAS_NO_SAVED_GAME)
    {
        for (i = 0; i < 6; i++)
            sMainMenuDataPtr->iconBoxSpriteIds[i] = SPRITE_NONE;
        return;
    }
    for (i = 0; i < 6; i++)
    {
        u8 x = ICON_BOX_1_START_X + ICON_BOX_X_DIFF * (i % 3);
        u8 y = ICON_BOX_1_START_Y + ICON_BOX_Y_DIFF * (i / 3);
        sMainMenuDataPtr->iconBoxSpriteIds[i] = CreateSprite(&sSpriteTemplate_IconBox, x, y, 2);
        gSprites[sMainMenuDataPtr->iconBoxSpriteIds[i]].invisible = (i >= gPlayerPartyCount);
        if (i < gPlayerPartyCount)
        {
            StartSpriteAnim(&gSprites[sMainMenuDataPtr->iconBoxSpriteIds[i]], 0);
            gSprites[sMainMenuDataPtr->iconBoxSpriteIds[i]].oam.priority = 1;
        }
    }
}

static void DestroyIconShadow(void)
{
    u8 i;
    if (sMainMenuDataPtr == NULL)
        return;
    for (i = 0; i < 6; i++)
    {
        if (sMainMenuDataPtr->iconBoxSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sMainMenuDataPtr->iconBoxSpriteIds[i]]);
            sMainMenuDataPtr->iconBoxSpriteIds[i] = SPRITE_NONE;
        }
    }
}

static u32 GetHPEggCyclePercent(u32 partyIndex)
{
    struct Pokemon *mon = &gPlayerParty[partyIndex];
    if (!GetMonData(mon, MON_DATA_IS_EGG))
        return GetMonData(mon, MON_DATA_HP) * 100 / GetMonData(mon, MON_DATA_MAX_HP);
    else
        return GetMonData(mon, MON_DATA_FRIENDSHIP) * 100 / gSpeciesInfo[GetMonData(mon, MON_DATA_SPECIES)].eggCycles;
}

static void CreatePartyMonIcons(void)
{
    u8 i;
    s16 x, y;
    LoadMonIconPalettes();
    for (i = 0; i < gPlayerPartyCount; i++)
    {
        x = ICON_BOX_1_START_X + ICON_BOX_X_DIFF * (i % 3);
        y = ICON_BOX_1_START_Y + ICON_BOX_Y_DIFF * (i / 3);
        sMainMenuDataPtr->iconMonSpriteIds[i] = CreateMonIcon(
            GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG),
            SpriteCB_MonIcon, x, y - 2, 0,
            GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY));
        gSprites[sMainMenuDataPtr->iconMonSpriteIds[i]].oam.priority = 0;
        if (GetHPEggCyclePercent(i) == 0)
            gSprites[sMainMenuDataPtr->iconMonSpriteIds[i]].callback = SpriteCallbackDummy;
    }
}

static void DestroyMonIcons(void)
{
    u8 i;
    if (sMainMenuDataPtr == NULL)
        return;
    for (i = 0; i < 6; i++)
    {
        if (sMainMenuDataPtr->iconMonSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sMainMenuDataPtr->iconMonSpriteIds[i]]);
            sMainMenuDataPtr->iconMonSpriteIds[i] = SPRITE_NONE;
        }
    }
}

// ─── Text printing ────────────────────────────────────────────────────────────
static const u8 sText_DexNum[]  = _("Dex {STR_VAR_1}");
static const u8 sText_Badges[] = _("Med. {STR_VAR_1}");

static void PrintToWindow(u8 windowId, u8 colorIdx)
{
    const u8 colors[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};
    u8 mapDisplayHeader[24];
    u8 *withoutPrefixPtr, *playTimePtr;
    u16 dexCount = 0;
    u8 badgeCount = 0;
    u32 i;

    FillWindowPixelBuffer(WINDOW_HEADER, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_MIDDLE, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    if (menuType == HAS_NO_SAVED_GAME)
    {
        PutWindowTilemap(WINDOW_HEADER);
        CopyWindowToVram(WINDOW_HEADER, 3);
        PutWindowTilemap(WINDOW_MIDDLE);
        CopyWindowToVram(WINDOW_MIDDLE, 3);
        return;
    }

    withoutPrefixPtr = &(mapDisplayHeader[3]);
    GetMapName(withoutPrefixPtr, GetCurrentRegionMapSectionId(), 0);
    mapDisplayHeader[0] = EXT_CTRL_CODE_BEGIN;
    mapDisplayHeader[1] = EXT_CTRL_CODE_HIGHLIGHT;
    mapDisplayHeader[2] = TEXT_COLOR_TRANSPARENT;
    AddTextPrinterParameterized4(WINDOW_HEADER, FONT_NARROW, GetStringCenterAlignXOffset(FONT_NARROW, withoutPrefixPtr, 10 * 8), 1, 0, 0, colors, 0xFF, mapDisplayHeader);

    playTimePtr = ConvertIntToDecimalStringN(gStringVar4, gSaveBlock2Ptr->playTimeHours, STR_CONV_MODE_LEFT_ALIGN, 3);
    *playTimePtr = 0xF0;
    ConvertIntToDecimalStringN(playTimePtr + 1, gSaveBlock2Ptr->playTimeMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    AddTextPrinterParameterized4(WINDOW_HEADER, FONT_NORMAL, 60 + GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, (6*8)), 1, 0, 0, colors, TEXT_SKIP_DRAW, gStringVar4);

    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
    {
        if (IsNationalPokedexEnabled())
            dexCount = GetNationalPokedexCount(FLAG_GET_CAUGHT);
        else
            dexCount = GetHoennPokedexCount(FLAG_GET_CAUGHT);
        ConvertIntToDecimalStringN(gStringVar1, dexCount, STR_CONV_MODE_RIGHT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar4, sText_DexNum);
        AddTextPrinterParameterized4(WINDOW_MIDDLE, FONT_NORMAL, 8 + 8, 16 + 2, 0, 0, colors, TEXT_SKIP_DRAW, gStringVar4);
    }

    for (i = FLAG_BADGE01_GET; i < FLAG_BADGE01_GET + NUM_BADGES; i++)
        if (FlagGet(i))
            badgeCount++;
    ConvertIntToDecimalStringN(gStringVar1, badgeCount, STR_CONV_MODE_LEADING_ZEROS, 1);
    StringExpandPlaceholders(gStringVar4, sText_Badges);
    AddTextPrinterParameterized4(WINDOW_MIDDLE, FONT_NORMAL, 16, 32 + 2, 0, 0, colors, TEXT_SKIP_DRAW, gStringVar4);

    AddTextPrinterParameterized3(WINDOW_MIDDLE, FONT_NORMAL, 16, 2, colors, TEXT_SKIP_DRAW, gSaveBlock2Ptr->playerName);

    PutWindowTilemap(WINDOW_HEADER);
    CopyWindowToVram(WINDOW_HEADER, 3);
    PutWindowTilemap(WINDOW_MIDDLE);
    CopyWindowToVram(WINDOW_MIDDLE, 3);
}

// ─── Input handling ───────────────────────────────────────────────────────────
static void Task_MainMenuMain(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
        switch (menuType)
        {
        case HAS_NO_SAVED_GAME:
            if (sSelectedOption == HW_WIN_NO_SAVE_NEW_GAME)
                sMainMenuDataPtr->savedCallback = CB2_NewGameFromUI;
            else
            {
                gMain.savedCallback = CB2_InitMainMenu;
                sMainMenuDataPtr->savedCallback = CB2_InitOptionMenu;
            }
            break;
        default:
            switch (sSelectedOption)
            {
            case HW_WIN_CONTINUE:
                sMainMenuDataPtr->savedCallback = CB2_ContinueSavedGame;
                sSelectedOption = HW_WIN_CONTINUE;
                break;
            case HW_WIN_NEW_GAME:
                sMainMenuDataPtr->savedCallback = CB2_NewGameFromUI;
                sSelectedOption = HW_WIN_CONTINUE;
                break;
            case HW_WIN_OPTIONS:
                gMain.savedCallback = CB2_InitMainMenu;
                sMainMenuDataPtr->savedCallback = CB2_InitOptionMenu;
                break;
            case HW_WIN_MYSTERY_EVENT:
                sMainMenuDataPtr->savedCallback = CB2_InitMysteryEventMenu;
                sSelectedOption = HW_WIN_CONTINUE;
                break;
            case HW_WIN_MYSTERY_GIFT:
                sMainMenuDataPtr->savedCallback = CB2_InitMysteryGift;
                sSelectedOption = HW_WIN_CONTINUE;
                break;
            }
        }
        gTasks[taskId].func = Task_MainMenuTurnOff;
    }

    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
        sMainMenuDataPtr->savedCallback = CB2_InitTitleScreen;
        sSelectedOption = (menuType == HAS_NO_SAVED_GAME) ? HW_WIN_NO_SAVE_NEW_GAME : HW_WIN_CONTINUE;
        gTasks[taskId].func = Task_MainMenuTurnOff;
    }

    if (JOY_NEW(DPAD_DOWN) || JOY_NEW(DPAD_UP))
    {
        switch (menuType)
        {
        case HAS_NO_SAVED_GAME:
            // no vertical navigation (only 2 side-by-side options)
            break;
        case HAS_SAVED_GAME:
            if (sSelectedOption == HW_WIN_CONTINUE)
                sSelectedOption = HW_WIN_NEW_GAME;
            else
                sSelectedOption = HW_WIN_CONTINUE;
            break;
        case HAS_MYSTERY_GIFT:
            if (sSelectedOption == HW_WIN_CONTINUE)
                sSelectedOption = HW_WIN_NEW_GAME;
            else if (sSelectedOption == HW_WIN_NEW_GAME || sSelectedOption == HW_WIN_OPTIONS)
                sSelectedOption = (JOY_NEW(DPAD_DOWN)) ? HW_WIN_MYSTERY_GIFT : HW_WIN_CONTINUE;
            else
                sSelectedOption = HW_WIN_CONTINUE;
            break;
        case HAS_MYSTERY_EVENTS:
            if (sSelectedOption == HW_WIN_CONTINUE)
                sSelectedOption = HW_WIN_NEW_GAME;
            else if (sSelectedOption == HW_WIN_NEW_GAME)
                sSelectedOption = (JOY_NEW(DPAD_DOWN)) ? HW_WIN_MYSTERY_GIFT : HW_WIN_CONTINUE;
            else if (sSelectedOption == HW_WIN_OPTIONS)
                sSelectedOption = (JOY_NEW(DPAD_DOWN)) ? HW_WIN_MYSTERY_EVENT : HW_WIN_CONTINUE;
            else
                sSelectedOption = HW_WIN_CONTINUE;
            break;
        }
        MoveHWindowsWithInput();
    }

    if (JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
    {
        switch (menuType)
        {
        case HAS_NO_SAVED_GAME:
            sSelectedOption = (sSelectedOption == HW_WIN_NO_SAVE_NEW_GAME)
                ? HW_WIN_NO_SAVE_OPTIONS : HW_WIN_NO_SAVE_NEW_GAME;
            break;
        case HAS_SAVED_GAME:
        case HAS_MYSTERY_GIFT:
        case HAS_MYSTERY_EVENTS:
            if (sSelectedOption == HW_WIN_NEW_GAME)
                sSelectedOption = HW_WIN_OPTIONS;
            else if (sSelectedOption == HW_WIN_OPTIONS)
                sSelectedOption = HW_WIN_NEW_GAME;
            else if (sSelectedOption == HW_WIN_MYSTERY_GIFT)
                sSelectedOption = HW_WIN_MYSTERY_EVENT;
            else if (sSelectedOption == HW_WIN_MYSTERY_EVENT)
                sSelectedOption = HW_WIN_MYSTERY_GIFT;
            break;
        }
        MoveHWindowsWithInput();
    }
}
