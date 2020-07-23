#ifndef CLIENTEDB_H
#define CLIENTEDB_H

#include <stdbool.h>
#include "Cliente.h"
#include "MessageInfoLevel.h"

typedef void(*ClienteMsgEventHandler)(const char* msg, MessageInfoLevel level);
typedef void(*ClienteEventHandler)(Cliente);

typedef struct {
    // EventHandlers
    ClienteEventHandler     ClienteAdicionadoCallback;
    ClienteEventHandler     ClienteRemovidoCallback;
    ClienteMsgEventHandler  ClienteMsgCallback;

    //pimpl
    struct ClienteDBImpl* impl;

} ClienteDB;

// ctor
ClienteDB CriarClienteDB(const char* ficheiro);

// dtor
void CloseClienteDB(ClienteDB*);

void AdicionarCliente(ClienteDB*, const Cliente*);
void RemoverCliente(ClienteDB*, const Cliente*);
bool ClienteExiste(ClienteDB*, const Cliente*);

int GenClienteID(ClienteDB);
unsigned int LinhaToClienteID(ClienteDB*, int linha);

// Getters
Cliente*     GetCliente(ClienteDB*, int linha);
Cliente*     GetClienteByID(ClienteDB*, int id);
char*        GetPathClientesDB(ClienteDB*);
unsigned int GetNumeroClientes(ClienteDB*);

#endif // CLIENTEDB_H
