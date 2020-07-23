#include "ClienteDB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "Util.h"
#include "Logger.h"
#include "vec.h"
#include "App.h"


typedef struct {
    char* ficheiro;
    int numClientes;
    Cliente** clientes;
} ClienteDBImpl;

typedef ClienteDBImpl Impl;

//-----------------------------------private------------------------------------------

// Funcao para fazer parse à linha do ficheiro e retornar o respetivo id do cliente
// Esta funcao pede um FILE* como argumento para evitar de o abrir desnecessariamente,
// e tambem permitir o uso enquanto o ficheiro nao é fechado noutra função.
// Retorna -1 se o ficheiro não estiver aberto
static unsigned int getClienteId(FILE*, int linha);

// Funcao para fazer parse à linha do ficheiro e retornar o respetivo nome do cliente
// Esta funcao pede um FILE* como argumento para evitar de o abrir desnecessariamente,
// e tambem permitir o uso enquanto o ficheiro nao é fechado noutra função.
// Retorna NULL se o ficheiro não estiver aberto
static char* getClienteNome(FILE*, int linha);

// Retorna um determinado cliente a partir do ficheiro da base de dados,
// O cliente retornado é determinado pela sua posição no ficheiro, neste caso a linha onde
// ele se encontra
static Cliente* getClienteFromFile(ClienteDB*, int linha);

// Define o ficheiro usado para a base de dados
static void setFicheiro(ClienteDB*, char* ficheiro);

// Carrega os clientes a partir do ficheiro da base de dados
// para o vetor interno onde se faz as operacoes I/O
static void loadClientes(ClienteDB*);

// Cria a instancia do vetor para guardar os dados
static void initClienteList(ClienteDB*);

// Adiciona o cliente ao vetor
static void addCliente(ClienteDB*, Cliente*);

// Retorna o handle de clientes
static Cliente** getClientes(ClienteDB*);

// Retorna o tamanho em bytes que os clientes ocupam na DB
static size_t getClientesSize(ClienteDB*);

// Dealloca todos os clientes carregados
static void freeClientes(ClienteDB*);

// Definir o numero de clientes iniciais
static void setClientes(ClienteDB*, int clientes);

// Modifica o valor de clientes (+)(-)
static void modClientes(ClienteDB*, int valor);

// Remove o cliente no index n do vetor se ele existir
static void deleteCliente(ClienteDB*, int index);

//------------------------------------------------------------------------------------

ClienteDB CriarClienteDB(const char* ficheiro) {
    ClienteDB db, *this = &db;
    this->impl = malloc(sizeof(ClienteDBImpl));

    FILE* dbFicheiro = fopen(ficheiro, "r");
    if(dbFicheiro == NULL) {
        LOGF(LOG_CRITICAL, "%s", "A base de dados nao existe!");
        exit(EXIT_FAILURE);
    }

    int linhas = Ficheiro_GetLinhas(dbFicheiro);

    setFicheiro(this, ficheiro);
    setClientes(this, linhas);
    initClienteList(this);

    int clientes = GetNumeroClientes(this);
    loadClientes(this);

    DEBUG_LOG("%s: Clientes(%d)", GetPathClientesDB(this), clientes);

    fclose(dbFicheiro);
    return *this;
}

void CloseClienteDB(ClienteDB* db) {
    freeClientes(db);
    free(db->impl); // Dealloc impl
    DEBUG_LOG("Dealocados %d bytes (%d clientes)", getClientesSize(db), GetNumeroClientes(db));
}

Cliente* GetCliente(ClienteDB* db, int linha) {
    if(linha+1 > GetNumeroClientes(db)) {
        db->ClienteMsgCallback("O numero de linhas excedeu as linhas disponiveis!", Critical);
    }
    DEBUG_LOG("%s", Cliente_ToString(*(getClientes(db)[linha])));
    // Retornar o cliente na linha passada, este
    // valor é fetched pelo vetor que guarda os clientes
    // onde linha = index do vetor
    return getClientes(db)[linha];
}

void AdicionarCliente(ClienteDB* db, const Cliente* cliente) {
    if(!IsClienteValido(cliente)) {
        db->ClienteMsgCallback("O cliente não é valido!", Warn);
        return;
    }

    FILE* dbClientes = fopen(GetPathClientesDB(db), "a+");
    // Add ao ficheiro
    fprintf(dbClientes, "%d:%s\n", cliente->id, cliente->nome);
    fclose(dbClientes);

    // Add ao vetor
    addCliente(db, cliente);
    modClientes(db, 1);

    db->ClienteAdicionadoCallback(*cliente);
    DEBUG_LOG("clientes[%d] << %s", GetNumeroClientes(db)-1, Cliente_ToString(*cliente));
}

void RemoverCliente(ClienteDB* db, const Cliente* cliente) {
    if(!IsClienteValido(cliente)) {
        db->ClienteMsgCallback("O cliente não é valido!", Warn);
        return;
    }

    for(int i = 0; i < GetNumeroClientes(db); i++) {
        if(getClientes(db)[i]->id == cliente->id) {
            // Remover do ficheiro DB
            Ficheiro_ApagarLinha(GetPathClientesDB(db), i+1);

            // Remover do vetor
            deleteCliente(db, i); // Remover cliente do vetor
            modClientes(db, -1); // Decrementar contador de clientes 
            
            db->ClienteRemovidoCallback(*cliente);
            DEBUG_LOG("Cliente %s(id:%d) removido!", cliente->nome, cliente->id);
            return;
        }
    }
}

bool ClienteExiste(ClienteDB* db, const Cliente* cliente) {
    // Se o id do cliente coincidir com algum na base de dados, ele existe
    bool existe = false;
    for(int i = 0; i < GetNumeroClientes(db); i++) {
        if(cliente->id == getClientes(db)[i]->id) {
            DEBUG_LOG("clientes[%d] = id:%d", i, getClientes(db)[i]->id);
            existe = true;
            break;
        }
    }
    return existe;
}

unsigned int LinhaToClienteID(ClienteDB* db, int linha) {
    for(int i = 0; i < GetNumeroClientes(db); i++) {
        if(i == linha) {
            Cliente* c = GetCliente(db, linha);
            return c->id;
        }
    }
}

Cliente* GetClienteByID(ClienteDB* db, int id) {
    Cliente* c = NULL;
    for(int i = 0; i < GetNumeroClientes(db); i++)  {
        c = getClientes(db)[i];
        if(id == c->id) {
            // Encontramos o cliente pretendido
            DEBUG_LOG("[%s] encontrado", Cliente_ToString(*c));
            return c;
        }
    }
    db->ClienteMsgCallback("O cliente não existe na base de dados!", Warn);
    return CLIENTE_INVALIDO;
}

int GenClienteID(ClienteDB db) {
    int* idClientes = malloc(sizeof(int) * GetNumeroClientes(&db));
    unsigned int numClientes = GetNumeroClientes(&db);

    int novoId = 1;

    for(int i = 0; i < numClientes; i++)
        idClientes[i] = GetCliente(&db, i)->id;

    Bubblesort(idClientes, numClientes);

    bool instanciado = false;

    for(int i = 0; i < numClientes; i++) {
        if(!instanciado) {
            novoId = idClientes[i]; // Comecar no id mais pequeno da DB
            instanciado = true;
        }

        // Enquanto o id for igual a um dos na DB,
        // incrementar ate nao ser
        if(novoId == idClientes[i]) {
            ++novoId;
            continue;
        }
        // O id é unico, sair do loop
        break;
    }

    free(idClientes);
    return novoId;
}

char* GetPathClientesDB(ClienteDB* db) {
    Impl* p = db->impl;
    return p->ficheiro;
}

unsigned int GetNumeroClientes(ClienteDB* db) {
    Impl* p = db->impl;
    return p->numClientes;
}


//-----------------------------------private-------------------------------------
static void loadClientes(ClienteDB* db) {
    FILE* dbFicheiro = fopen(GetPathClientesDB(db), "r");
    unsigned int clientes = GetNumeroClientes(db);

    for(int i = 0; i < clientes; i++) {
        const char* nome = getClienteNome(dbFicheiro, i);
        unsigned int id  = getClienteId(dbFicheiro, i);
        Cliente* c = CriarCliente(id, nome);
        addCliente(db, c);
        DEBUG_LOG("clientes[%d] << %s", i, Cliente_ToString(*c));
    }

    fclose(dbFicheiro);
}

static unsigned int getClienteId(FILE* f, int linha) {
    if(f == NULL)
        return -1;

    const char* linhaSelecionada = Ficheiro_GetLinha(f, linha);
    char idBuffer[4];
    snprintf(idBuffer, 4, "%c%c%c", linhaSelecionada[0],linhaSelecionada[1],linhaSelecionada[2]);
    return (unsigned int)atoi(idBuffer);
}

static char* getClienteNome(FILE* f, int linha) {
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

static Cliente* getClienteFromFile(ClienteDB* db, int linha) {
    FILE* f = fopen(GetPathClientesDB(db), "r");
    int clientes = GetNumeroClientes(db);

    const char* nome = getClienteNome(f, linha);
    unsigned int id  = getClienteId(f, linha);

    fclose(f);

    Cliente* c = CriarCliente(id, nome);
    return c;
}

static Cliente** getClientes(ClienteDB* db) {
    Impl* p = db->impl;
    return p->clientes;
}

static void addCliente(ClienteDB* db, Cliente* c) {
    Impl* p = db->impl;
    vector_add(&p->clientes, c);
}

static void setFicheiro(ClienteDB* db, char* ficheiro) {
    Impl* p = db->impl;
    p->ficheiro = ficheiro;
}

static void setClientes(ClienteDB* db, int clientes) {
    Impl* p = db->impl;
    p->numClientes = clientes;
}

static void modClientes(ClienteDB* db, int valor) {
    Impl* p = db->impl;
    p->numClientes += valor;
}

static size_t getClientesSize(ClienteDB* db) {
    Impl* p = db->impl;
    return sizeof(Cliente) * p->numClientes;
}

static void deleteCliente(ClienteDB* db, int index) {
    Impl* p = db->impl;
    if(index < vector_size(p->clientes)) {
        vector_remove(p->clientes, index);
        return;
    }
    DEBUG_LOG("index > clientes[%d]", index, vector_size(p->clientes));
}

static void initClienteList(ClienteDB* db) {
    Impl* p = db->impl;
    p->clientes = vector_create();
}

static void freeClientes(ClienteDB* db) {
    Impl* p = db->impl;
    for(int i = 0; i < p->numClientes; i++) {
        FreeCliente(p->clientes[i]);
    }
    vector_free(p->clientes);
}

//-------------------------------------------------------------------------------
