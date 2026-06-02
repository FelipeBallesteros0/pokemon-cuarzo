#ifndef GUARD_UI_START_MENU_H
#define GUARD_UI_START_MENU_H

#include "global.h"

void UIStartMenu_Load(u8 count);
void UIStartMenu_AddButton(u8 slot, const u8 *text);
void UIStartMenu_Finalize(void);
void UIStartMenu_MoveCursor(u8 pos);
void UIStartMenu_ScrollBg(void);
void UIStartMenu_Free(void);
u8   UIStartMenu_Navigate(u8 pos, s8 dx, s8 dy, u8 count);

#endif // GUARD_UI_START_MENU_H
