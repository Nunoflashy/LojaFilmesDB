#include "MenuManager.h"
#include <stdio.h>
#include "vec.h"
#include "stdarg.h"

MenuManager Menu_CriarMenu() {
    MenuManager m;
    m.items = vector_create();
}

void Menu_Free(MenuManager* menu) {
    vector_free(menu->items);
}

void Menu_Add(MenuManager* menu, const char* item) {
    vector_add(&menu->items, item);
}

void Menu_Mostrar(MenuManager menu) {
    for(int i = 0; i < vector_size(menu.items); i++) {
        // Retornar o item atual no vetor
        printf("%d - %s\n", i+1, menu.items[i]);
    }
    // Prompt
    printf("\n: ");
}

int Menu_GetInput(MenuManager menu) {
    int op;
    fflush(stdin);
    scanf("%d", &op);

    if(op >= 1 && op <= vector_size(menu.items)) {
        return op;
    }

    return -1;
}