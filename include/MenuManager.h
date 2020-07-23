#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "vec.h"

typedef struct {
    char** items;
} MenuManager;

MenuManager Menu_CriarMenu();
void Menu_Free(MenuManager*);

void Menu_Add(MenuManager*, const char* item);
void Menu_Mostrar(MenuManager);
int  Menu_GetInput(MenuManager);

#endif // MENUMANAGER_H
