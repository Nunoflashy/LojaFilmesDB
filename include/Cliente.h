#ifndef CLIENTE_H
#define CLIENTE_H

#include <stdbool.h>

#define CLIENTE_INVALIDO CriarCliente(-1, "null")

typedef struct {
    int id;
    char* nome;
    
    // pimpl
    struct ClienteImpl* impl;
} Cliente;

// ctor
Cliente* CriarCliente(int id, const char* nome);

// dtor
void FreeCliente(Cliente*);

//----Util-----------------------------------------
void PrintCliente(Cliente);
char* Cliente_ToString(Cliente);
//-------------------------------------------------

bool IsClienteValido(Cliente*);

#endif // CLIENTE_H
