#include "FilmeDB.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "Util.h"
#include "Logger.h"
#include "vec.h"
#include "App.h"

typedef struct {
    char* ficheiro;
    char* tempFicheiro;
    int numFilmes;
    Filme** filmes;
} FilmeDBImpl;

typedef FilmeDBImpl Impl;

//-----------------------------------private------------------------------------------

// Funcao para fazer parse à linha do ficheiro e retornar o respetivo id do filme
// Esta funcao pede um FILE* como argumento para evitar de o abrir desnecessariamente,
// e tambem permitir o uso enquanto o ficheiro nao é fechado noutra função.
// Retorna -1 se o ficheiro não estiver aberto
static unsigned int getFilmeId(FILE*, int linha);

// Funcao para fazer parse à linha do ficheiro e retornar o respetivo nome do filme
// Esta funcao pede um FILE* como argumento para evitar de o abrir desnecessariamente,
// e tambem permitir o uso enquanto o ficheiro nao é fechado noutra função.
// Retorna NULL se o ficheiro não estiver aberto
static char* getFilmeNome(FILE*, int linha);

// Funcao que retorna um determinado filme a partir do ficheiro da base de dados,
// O filme retornado e determinado pela sua posicao no ficheiro, neste caso a linha onde
// ele se encontra
static Filme* getFilmeFromFile(FilmeDB*, int linha);

// Funcao para verificar se o filme é valido.
// Esta função analisa o Filme* e vê se os dados
// da estrutura são validos.
static bool isFilmeValido(Filme*);

// Define o ficheiro usado para a base de dados
static void setFicheiro(FilmeDB*, char* ficheiro);

static void setFicheiroTemp(FilmeDB*, char* ficheiro);
static char* getFicheiroTemp(FilmeDB*);

// Carrega os filmes a partir do ficheiro da base de dados
// para o vetor interno onde se faz as operacoes I/O
static void loadFilmes(FilmeDB*);

// Cria a instancia do vetor para guardar os dados
static void initFilmeList(FilmeDB*);

// Adiciona o filme ao vetor
static void addFilme(FilmeDB*, Filme*);

// Retorna o handle de filmes
static Filme** getFilmes(FilmeDB*);

// Retorna o tamanho em bytes que os filmes ocupam na DB
static size_t getFilmesSize(FilmeDB*);

// Dealloca todos os filmes carregados
static void freeFilmes(FilmeDB*);

// Define o numero de filmes no vetor
static void setFilmes(FilmeDB*, int filmes);

// Modifica o valor de filmes (+)(-)
static void modFilmes(FilmeDB*, int valor);

// Remove o filme no index n do vetor se ele existir
static void deleteFilme(FilmeDB*, int index);


//------------------------------------------------------------------------------------

FilmeDB CriarFilmeDB(const char *ficheiro) {
    FilmeDB db, *this = &db;
    this->impl = malloc(sizeof(FilmeDBImpl));

    FILE* dbFicheiro = fopen(ficheiro, "r");
    if(dbFicheiro == NULL) {
        LOGF(LOG_CRITICAL, "%s", "A base de dados nao existe!");
    }

    int linhas = Ficheiro_GetLinhas(dbFicheiro);

    setFicheiro(this, ficheiro);
    setFilmes(this, linhas);
    initFilmeList(this);
    setFicheiroTemp(this, "data/filmes.temp");

    int filmes = GetNumeroFilmes(this);
    loadFilmes(this);

    DEBUG_LOG("%s: Filmes(%d)", GetPathFilmesDB(this), filmes);

    fclose(dbFicheiro);
    return *this;
}

void CloseFilmeDB(FilmeDB* db) {
    freeFilmes(db);
    free(db->impl); // Dealloc impl
    DEBUG_LOG("Dealocados %d bytes (%d filmes)", getFilmesSize(db), GetNumeroFilmes(db));
}

Filme* GetFilme(FilmeDB* db, int linha) {
    if(linha+1 > GetNumeroFilmes(db)) {
        db->FilmeMsgCallback("O numero de linhas excedeu as linhas disponiveis!\n", Critical);
    }
    return getFilmes(db)[linha];
}

void AdicionarFilme(FilmeDB *db, const Filme *filme) {
    if(!IsFilmeValido(filme)) {
        db->FilmeMsgCallback("O filme não é valido!", Warn);
        return;
    }

    FILE* dbFilmes = fopen(GetPathFilmesDB(db), "a+");
    // Add ao ficheiro
    fprintf(dbFilmes, "%d:%s\n", filme->id, filme->nome);
    fclose(dbFilmes);

    // Add ao vetor
    addFilme(db, filme);
    modFilmes(db, 1);

    db->FilmeAdicionadoCallback(*filme);
    DEBUG_LOG("filmes[%d] << %s", GetNumeroFilmes(db)-1, Filme_ToString(*filme));
}

void RemoverFilme(FilmeDB* db, const Filme* filme) {
    int filmes = GetNumeroFilmes(db);
    Filme** filmesList = malloc(sizeof(Filme*) * filmes);
    FILE* fTemp = fopen(GetPathFilmesDB(db), "r");
    for(int i = 0; i < filmes; i++) {
        int linhaBytes = Ficheiro_GetTamanhoBytesLinha(fTemp, i);
        DEBUG_LOG("Bytes na linha %d: %d", i, linhaBytes);
        filmesList[i] = malloc(linhaBytes);
    }
        

    for(int i = 0; i < filmes; i++) {
        Filme* f = GetFilme(db, i);
        int linhaBytes = Ficheiro_GetTamanhoBytesLinha(fTemp, i);
        snprintf(filmesList[i], linhaBytes, "%d:%s", f->id, f->nome);
        printf("%s\n", filmesList[i]);
    }
    fclose(fTemp);

    FILE* fichTemp = fopen(getFicheiroTemp(db), "w");
    FILE* fichOrig = fopen(GetPathFilmesDB(db), "r");
    for(int i = 0; i < filmes; i++) {
        if(getFilmeId(fichOrig, i) == filme->id) {
            deleteFilme(db, i);
            modFilmes(db, -1);
            db->FilmeRemovidoCallback(*filme);
            DEBUG_LOG("Filme %s(id:%d) removido!", filme->nome, filme->id);
            continue;
        }
        fprintf(fichTemp, "%s\n", filmesList[i]);
    }
    fclose(fichTemp);
    fclose(fichOrig);

    for(int i = 0; i < filmes; i++)
        free(filmesList[i]);
    free(filmesList);

    const char* pathDB     = GetPathFilmesDB(db);
    const char* pathDBTemp = getFicheiroTemp(db);

    // Mover o ficheiro alterado para o original
    rename(pathDBTemp, pathDB);
}

/* void RemoverFilme(FilmeDB* db, const Filme* filme) {
    if(!IsFilmeValido(filme)) {
        db->FilmeMsgCallback("O filme não é valido!", Warn);
        return;
    }

    for(int i = 0; i < GetNumeroFilmes(db); i++) {
        if(getFilmes(db)[i]->id == filme->id) {
            // Remover do ficheiro DB
            Ficheiro_ApagarLinha(GetPathFilmesDB(db), i);

            // Remover do vetor
            deleteFilme(db, i); // Remover cliente do vetor
            modFilmes(db, -1); // Decrementar contador de clientes 
            
            db->FilmeRemovidoCallback(*filme);
            DEBUG_LOG("Filme %s(id:%d) removido!", filme->nome, filme->id);
            return;
        }
    }
} */

bool FilmeExiste(FilmeDB* db, const Filme* filme) {
    bool existe = false;
    for(int i = 0; i < GetNumeroFilmes(db); i++) {
        if(filme->id == getFilmes(db)[i]->id) {
            existe = true;
            break;
        }
    }
    return existe;
}

int GenFilmeID(FilmeDB db) {
    int* idFilmes = malloc(sizeof(int) * GetNumeroFilmes(&db));
    unsigned int numFilmes = GetNumeroFilmes(&db);

    int novoId = 1;

    for(int i = 0; i < numFilmes; i++) {
        idFilmes[i] = GetFilme(&db, i)->id;
    }

    Bubblesort(idFilmes, numFilmes);

    bool instanciado = false;

    for(int i = 0; i < numFilmes; i++) {
        if(!instanciado) {
            novoId = idFilmes[i]; // Comecar no id mais pequeno da DB
            instanciado = true;
        }

        // Enquanto o id for igual a um dos na DB,
        // incrementar ate nao ser
        if(novoId == idFilmes[i]) {
            ++novoId;
            continue;
        }
        // O id é unico, sair do loop
        break;
    }

    free(idFilmes);
    return novoId;
}


unsigned int LinhaToFilmeID(FilmeDB* db, int linha) {
    for(int i = 0; i < GetNumeroFilmes(db); i++) {
        if(i == linha) {
            Filme* f = GetFilme(db, linha);
            return f->id;
        }
    }
}

Filme* GetFilmeByID(FilmeDB* db, int id) {
    Filme* f = NULL;
    for(int i = 0; i < GetNumeroFilmes(db); i++) {
        f = getFilmes(db)[i];
        if(id == f->id) {
            return f;
        }
    }
    return FILME_INVALIDO;
}

char* GetPathFilmesDB(FilmeDB* db) {
    Impl* impl = db->impl;
    return impl->ficheiro;
}

unsigned int GetNumeroFilmes(FilmeDB* db) {
    Impl* impl = db->impl;
    return impl->numFilmes;
}


//-------private--------

static void loadFilmes(FilmeDB* db) {
    FILE* dbFicheiro = fopen(GetPathFilmesDB(db), "r");
    unsigned int filmes = GetNumeroFilmes(db);

    for(int i = 0; i < filmes; i++) {
        const char* nome = getFilmeNome(dbFicheiro, i);
        unsigned int id  = getFilmeId(dbFicheiro, i);
        Filme* c = CriarFilme(id, nome);
        addFilme(db, c);
        DEBUG_LOG("filmes[%d] << %s", i, Filme_ToString(*c));
    }

    fclose(dbFicheiro);
}

static unsigned int getFilmeId(FILE* f, int linha) {
    if(f == NULL)
        return -1;

    const char* linhaSelecionada = Ficheiro_GetLinha(f, linha);
    char idBuffer[4];
    snprintf(idBuffer, 4, "%c%c%c", linhaSelecionada[0],linhaSelecionada[1],linhaSelecionada[2]);

    return (unsigned int)atoi(idBuffer);
}

static char* getFilmeNome(FILE* f, int linha) {
    if(f == NULL)
        return NULL;

    char* linhaSelecionada = Ficheiro_GetLinha(f, linha);
    char* nome = strchr(linhaSelecionada, ':');

    // Buffer temporario, ha um problema com a funcao GetLinha...
    // sem o buffer, so apanha algumas partes da string, o que leva a crer
    // que o problema é o resultado da função a apontar para outra parte da string.
    char temp[4096];
    return ++nome;
}

static bool isFilmeValido(Filme* f) {
    if(f->id >= 1 && f->id <= 999) {
        return true;
    }

    return false;
}

static Filme* getFilmeFromFile(FilmeDB* db, int linha) {
    FILE* f = fopen(GetPathFilmesDB(db), "r");
    int filmes = GetNumeroFilmes(db);

    const char* nome = getFilmeNome(f, linha);
    unsigned int id  = getFilmeId(f, linha);

    fclose(f);

    Filme* filme = CriarFilme(id, nome);
    return filme;
}

static Filme** getFilmes(FilmeDB* db) {
    Impl* p = db->impl;
    return p->filmes;
}

static void addFilme(FilmeDB* db, Filme* f) {
    Impl* p = db->impl;
    vector_add(&p->filmes, f);
}

static void setFicheiro(FilmeDB* db, char* ficheiro) {
    Impl* p = db->impl;
    p->ficheiro = ficheiro;
}

static char* getFicheiroTemp(FilmeDB* db) {
    Impl* p = db->impl;
    return p->tempFicheiro;
}

static void setFicheiroTemp(FilmeDB* db, char* ficheiro) {
    Impl* p = db->impl;
    p->tempFicheiro = ficheiro;
}

static void setFilmes(FilmeDB* db, int filmes) {
    Impl* p = db->impl;
    p->numFilmes = filmes;
}

static void modFilmes(FilmeDB* db, int valor) {
    Impl* p = db->impl;
    p->numFilmes += valor;
}

static size_t getFilmesSize(FilmeDB* db) {
    Impl* p = db->impl;
    return sizeof(Filme) * p->numFilmes;
}

static void deleteFilme(FilmeDB* db, int index) {
    Impl* p = db->impl;
    if(index < vector_size(p->filmes)) {
        vector_remove(p->filmes, index);
        return;
    }
    DEBUG_LOG("index > filmes[%d]", index, vector_size(p->filmes));
}

static void initFilmeList(FilmeDB* db) {
    Impl* p = db->impl;
    p->filmes = vector_create();
}

static void freeFilmes(FilmeDB* db) {
    Impl* p = db->impl;
    for(int i = 0; i < p->numFilmes; i++) {
        FreeFilme(p->filmes[i]);
    }
    vector_free(p->filmes);
}
//----------------------
