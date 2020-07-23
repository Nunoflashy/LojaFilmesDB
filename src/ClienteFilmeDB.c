#include "ClienteFilmeDB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "Logger.h"
#include "App.h"
#include "vec.h"
#include "Util.h"

//--------------------------private---------------------------------
// Retorna o path dos registos do cliente, se existir.
static char* getClientePath(Cliente* c) {
    char ficheiroPath[1024], *ptr = ficheiroPath;
    snprintf(ficheiroPath, 1024, "%s//%d", "data//clientes", c->id);
    return ptr;
}

// Retorna o path da info do filme f, se existir.
static char* getFilmePath(Filme* f) {
    char path[1024], *ptr = path;
    snprintf(path, 1024, "%s//%d", "data//filmes", f->id);
    return ptr;
}
//------------------------------------------------------------------

void AddFilmeToCliente(Filme* f, Cliente* c) {
    const char* ficheiroPath = getClientePath(c);
    FILE* ficheiro = fopen(ficheiroPath, "a+");
    if(ficheiro == NULL) return;
    fprintf(ficheiro, "%d\n", f->id);
    fclose(ficheiro);
}

void RemoverFilmeFromCliente(Filme* f, Cliente* c) {
    const char* ficheiroPath = getClientePath(c);
    FILE* fichDB = fopen(ficheiroPath, "r");
    
    int* ids = vector_create();
    for(int i = 0; i < Ficheiro_GetLinhas(fichDB); i++) {
        int idAtual = atoi(Ficheiro_GetLinha(fichDB, i));
        vector_add(&ids, idAtual);
        DEBUG_LOG("Adicionado %d ao vetor", ids[i]);
    }
    fclose(fichDB);
    for(int i = 0; i < vector_size(ids); i++) {
        // Se estivermos na linha que tem o ID do filme passado
        if(ids[i] == f->id) {
            // Apagar esse ID do ficheiro
            Ficheiro_ApagarLinha(getClientePath(c), i+1);
        }
    }
    vector_free(ids);
}

void RemoverFilmeFromClientes(Filme* f, ClienteDB* db) {
    for(int i = 0; i < GetNumeroClientes(db); i++) {
        Cliente* c = GetCliente(db, i);
        if(ClienteTemRegisto(c)) {
            RemoverFilmeFromCliente(f, c);
            DEBUG_LOG("Filme %s apagado do cliente %s", \
                Filme_ToString(*f), c->nome);
        }
    }
}

void RemoverRegistoCliente(Cliente* c) {
    if(remove(getClientePath(c)) == 0) {
        DEBUG_LOG("O registo do cliente %s foi apagado com sucesso!", Cliente_ToString(*c));
        return;
    }
    LOGF(LOG_WARN, "Houve um erro ao apagar o registo por completo, apague o ficheiro %s manualmente!",  getClientePath(c));
    Ficheiro_Limpar(getClientePath(c));
}

bool ClienteTemRegisto(Cliente* c) {
    FILE* fich = fopen(getClientePath(c), "r");
    //bool temRegisto = true;

    if(fich == NULL || Ficheiro_GetTamanhoBytes(fich) == 0)
        return false;
    
    //fclose(fich);
    return true;
    //return temRegisto;
}


void ListarFilmesByCliente(FilmeDB* db, Cliente* c) {
    // Obter os filmes deste cliente na base de dados db
    Filme** filmeList = GetFilmesFromCliente(db, c);

    //-----------------------Menu Layout-----------------------------
    printf("Informação de %s (Cliente ID %d)\n\n", c->nome, c->id);
    printf("ID\t\tFilme\n\-----------------------------------------------------------------------------\n");
    for(int i = 0; i < vector_size(filmeList); i++) {
        Filme* f = filmeList[i];
        printf("%d\t\t%s\n", f->id, f->nome);
    }
    printf("\-----------------------------------------------------------------------------\n");
    printf("Filmes Adquiridos: %d\n\n", vector_size(filmeList));
    //---------------------------------------------------------------
    vector_free(filmeList);
}

Filme** GetFilmesFromCliente(FilmeDB* db, Cliente* c) {
    Filme** filmeList = vector_create();

    const char* ficheiroPath = getClientePath(c);
    FILE* fich = fopen(ficheiroPath, "r");
    if(fich == NULL) return;

    int linhas = Ficheiro_GetLinhas(fich);

    for(int i = 0; i < linhas; i++) {
        // Obter o id do filme a partir da linha atual do ficheiro
        int filmeID = atoi(Ficheiro_GetLinha(fich, i));
        // Adicionar o filme ao vetor se ele existir em db
        vector_add(&filmeList, GetFilmeByID(db, filmeID));
    }

    return filmeList;
}

bool ListarClientesComRegisto(ClienteDB* db) {
    Cliente** clienteList = GetClientesComRegisto(db);

    if(vector_size(clienteList) == 0) {
        return false;
    }
    printf("ID\t\tNome\n---------------------------------------------------\n");
    for(int i = 0; i < vector_size(clienteList); i++) {
        printf("%d\t\t%s\n", clienteList[i]->id, clienteList[i]->nome);
    }
    printf("---------------------------------------------------\n");
    printf("\nNumero de Clientes com Registo: %d\n\n", vector_size(clienteList));
    vector_free(clienteList);
    return true;
}

Cliente** GetClientesComRegisto(ClienteDB* db) {
    Cliente** clienteList = vector_create();
    for(int i = 0; i < GetNumeroClientes(db); i++) {
        Cliente* c = GetCliente(db, i);
        if(ClienteTemRegisto(c)) {
            vector_add(&clienteList, c);
        }
    }
    return clienteList;
}

void AddInfoToFilme(char* info, Filme* f) {
    DEBUG_LOG("%s", getFilmePath(f));
    FILE* fich = fopen(getFilmePath(f), "w");
    if(fich == NULL) return;
    fprintf(fich, "%s\n", info);
    fclose(fich);
}

void RemoverInfoFromFilme(Filme* f) {
    // Limpar o ficheiro, limpar o registo do filme.
    if(remove(getFilmePath(f)) == 0) {
        DEBUG_LOG("A informação do filme %s foi apagada com sucesso!", Filme_ToString(*f));
        return;
    }
    LOGF(LOG_WARN, "Houve um erro ao apagar o registo por completo, apague o ficheiro %s manualmente!",  getFilmePath(f));
    LOGF(LOG_INFO, "Ficheiro %s limpo.", getFilmePath(f));
    Ficheiro_Limpar(getFilmePath(f));
}


Filme** GetFilmesComInfo(FilmeDB* db) {
    Filme** filmeList = vector_create();
    for(int i = 0; i < GetNumeroFilmes(db); i++) {
        Filme* f = GetFilme(db, i);
        if(FilmeTemInfo(f)) {
            vector_add(&filmeList, f);
        }
    }
    return filmeList;
}

bool ListarFilmesComInfo(FilmeDB* db) {
    Filme** filmeList = GetFilmesComInfo(db);
    if(vector_size(filmeList) == 0) { return false; }
    printf("ID\t\tFilme\n-----------------------------------------------------------------------------\n");
    for(int i = 0; i < vector_size(filmeList); i++) {
        printf("%d\t\t%s\n", filmeList[i]->id, filmeList[i]->nome);
    }
    printf("\-----------------------------------------------------------------------------\n");
    printf("\nNumero de Filmes com Info registada: %d\n\n", vector_size(filmeList));
    vector_free(filmeList);
    return true;
}

bool FilmeTemInfo(Filme* f) {
    FILE* fich = fopen(getFilmePath(f), "r");
    if(fich == NULL || Ficheiro_GetTamanhoBytes(fich) == 0)
        return false;
    return true;
}

void MostrarInfoFromFilme(Filme* f) {
    char* dados = GetInfoFromFilme(f);
    printf("Informação de %s (Filme ID %d)\n\n", f->nome, f->id);
    printf("%s\n", dados);
    free(dados);
}

char* GetInfoFromFilme(Filme* f) {
    FILE* fich = fopen(getFilmePath(f), "r");
    // Obter a info toda do ficheiro, os bytes todos
    char* dados = Ficheiro_GetChars(fich);
    fclose(fich);
    return dados;
}