#include "Cliente.h"
#include <stdlib.h>
#include <string.h>
#include "Logger.h"
#include "App.h"

typedef struct {
    bool alocado;
} ClienteImpl;


//------------------private--------------------
static void setAlocado(Cliente* c, bool v) {
    ClienteImpl* p = c->impl;
    p->alocado = v;
}

static bool isAlocado(Cliente* c) {
    ClienteImpl* p = c->impl;
    return p->alocado;
}
//--------------------------------------------

Cliente* CriarCliente(int id, const char* nome) {
    Cliente* c   = malloc(sizeof(Cliente));
    c->impl      = malloc(sizeof(ClienteImpl));
    char* buffer = malloc(sizeof(char) * strlen(nome));
    
    setAlocado(c, true);

    int i;
    for(i = 0; i < strlen(nome); i++) {
        buffer[i] = nome[i];
    }
    buffer[i] = '\0';
    
    c->id = id;
    c->nome = buffer;
    return c;
}

void PrintCliente(Cliente c) {
    printf("%s(%d)", c.nome, c.id);
}

char* Cliente_ToString(Cliente c) {
    char buffer[1024], *ptr = buffer;
    snprintf(buffer, 1024, "%d:%s", c.id, c.nome);
    return ptr;
}


bool IsClienteValido(Cliente* c) {
    if(c->id >= 1 && c->id <= 999) {
        return true;
    }
    return false;
}

void FreeCliente(Cliente* c) {
    if(isAlocado(c)) {
        free(c->nome);
        free(c);
        setAlocado(c, false);
        c->nome = NULL;
    }
    free(c->impl);
}