#ifndef FILME_H
#define FILME_H

#include <stdbool.h>

#define FILME_INVALIDO CriarFilme(-1, "null")

typedef struct {
    int id;
    char* nome;

    // pimpl
    struct FilmeImpl* impl;
} Filme;

Filme* CriarFilme(int id, const char* nome);
char* Filme_ToString(Filme);
bool IsFilmeValido(Filme*);
void FreeFilme(Filme*);


#endif // FILME_H
