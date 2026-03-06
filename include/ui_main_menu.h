#ifndef GUARD_UI_MENU_H
#define GUARD_UI_MENU_H

#include "main.h"

void Task_OpenMainMenu(u8 taskId);
void MainMenu_Init(MainCallback callback);
void OpenCustomMainMenu(u8 menuType);

#endif // GUARD_UI_MENU_H
