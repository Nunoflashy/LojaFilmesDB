#ifndef FILMEDB_H
#define FILMEDB_H

#include <stdbool.h>
#include "Filme.h"
#include "MessageInfoLevel.h"

typedef void(*FilmeMsgEventHandler)(const char* msg, MessageInfoLevel level);
typedef void(*FilmeEventHandler)(Filme);

typedef struct {
    // EventHandlers
    FilmeEventHandler     FilmeAdicionadoCallback;
    FilmeEventHandler     FilmeRemovidoCallback;
    FilmeMsgEventHandler  FilmeMsgCallback;
    ////////

    //pimpl
    struct FilmeDBImpl* impl;
} FilmeDB;

FilmeDB CriarFilmeDB(const char *ficheiro); // ctor
void CloseFilmeDB(FilmeDB*); //dtor

void AdicionarFilme(FilmeDB*, const Filme*);
void RemoverFilme(FilmeDB*, const Filme*);
bool FilmeExiste(FilmeDB*, const Filme*);

int GenFilmeID(FilmeDB);
unsigned int LinhaToFilmeID(FilmeDB*, int linha);

// Getters
Filme*       GetFilme(FilmeDB*, int linha);
Filme*       GetFilmeByID(FilmeDB*, int id);
char*        GetPathFilmesDB(FilmeDB*);
unsigned int GetNumeroFilmes(FilmeDB*);

#endif // FILMEDB_H
