#include <stdio.h>
#include "Filme.h"
#include "Logger.h"
#include "Util.h"
#include "FilmeDB.h"
#include "App.h"
#include "vec.h"

typedef struct {
    bool alocado;
} FilmeImpl;


//------------------private--------------------
static void setAlocado(Filme* f, bool v) {
    FilmeImpl* p = f->impl;
    p->alocado = v;
}

static bool isAlocado(Filme* f) {
    FilmeImpl* p = f->impl;
    return p->alocado;
}
//--------------------------------------------

Filme* CriarFilme(int id, const char* nome) {
    Filme* f     = malloc(sizeof(Filme));
    f->impl      = malloc(sizeof(FilmeImpl));
    char* buffer = malloc(sizeof(char) * strlen(nome) +1);
    
    setAlocado(f, true);

    int i;
    for(i = 0; i < strlen(nome); i++) {
        buffer[i] = nome[i];
    }
    buffer[i] = '\0';
    
    f->id = id;
    f->nome = buffer;
    return f;
}

char* Filme_ToString(Filme f) {
    char buffer[1024], *ptr = buffer;
    snprintf(buffer, 1024, "%d:%s", f.id, f.nome);
    return ptr;
}

bool IsFilmeValido(Filme* f) {
    if(f->id >= 1 && f->id <= 999) {
        return true;
    }
    return false;
}

void FreeFilme(Filme* f) {
    if(isAlocado(f)) {
        free(f->nome);
        free(f);
        setAlocado(f, false);
        f->nome = NULL;
    }
    free(f->impl);
}

