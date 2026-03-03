#include "global.h"
#include "electric_bike.h"
#include "item.h"
#include "bike.h"
#include "menu.h"
#include "item_menu.h"
#include "field_message_box.h"
#include "task.h"
#include "script.h"
#include "event_object_lock.h"
#include "constants/items.h"

#define ELECTRIC_BIKE_CHARGE_MAX 250

static const u8 sText_ElectricBikeSwapped[] = _("Cambiaste la batería. (250 pasos)");
static const u8 sText_ElectricBikeEmpty[] = _("La batería se agotó. Guardaste la Bicicleta Eléctrica.");

static void Task_CloseElectricBikeMessage(u8 taskId)
{
    ClearDialogWindowAndFrame(0, TRUE);
    DestroyTask(taskId);
    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
}

static void ElectricBike_ShowMessage(const u8 *text)
{
    if (FuncIsActiveTask(Task_CloseElectricBikeMessage))
        return;

    LockPlayerFieldControls();
    DisplayItemMessageOnField(CreateTask(TaskDummy, 0), text, Task_CloseElectricBikeMessage);
}

bool8 ElectricBike_IsActive(void)
{
    return gSaveBlock1Ptr->electricBikeActive && (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_MACH_BIKE);
}

void ElectricBike_SetActive(bool8 active)
{
    gSaveBlock1Ptr->electricBikeActive = active ? 1 : 0;
}

static bool8 ElectricBike_TryConsumeBattery(void)
{
    if (CheckBagHasItem(ITEM_BATERIA_DE_BICICLETA, 1))
    {
        RemoveBagItem(ITEM_BATERIA_DE_BICICLETA, 1);
        gSaveBlock1Ptr->electricBikeCharge = ELECTRIC_BIKE_CHARGE_MAX;
        return TRUE;
    }
    return FALSE;
}

static void ElectricBike_HandleDepleted(void)
{
    if (ElectricBike_TryConsumeBattery())
    {
        ElectricBike_ShowMessage(sText_ElectricBikeSwapped);
        return;
    }

    gSaveBlock1Ptr->electricBikeCharge = 0;
    ElectricBike_SetActive(FALSE);
    GetOnOffBike(PLAYER_AVATAR_FLAG_MACH_BIKE);
    ElectricBike_ShowMessage(sText_ElectricBikeEmpty);
}

void ElectricBike_OnStep(void)
{
    if (!ElectricBike_IsActive())
    {
        if (gSaveBlock1Ptr->electricBikeActive && !(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_BIKE))
            gSaveBlock1Ptr->electricBikeActive = 0;
        return;
    }

    if (gSaveBlock1Ptr->electricBikeCharge == 0)
    {
        ElectricBike_HandleDepleted();
        return;
    }

    gSaveBlock1Ptr->electricBikeCharge--;
    if (gSaveBlock1Ptr->electricBikeCharge == 0)
        ElectricBike_HandleDepleted();
}
