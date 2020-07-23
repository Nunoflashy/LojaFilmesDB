#ifndef CLIENTEFILMEDB_H
#define CLIENTEFILMEDB_H

#include <stdbool.h>
#include "ClienteDB.h"
#include "FilmeDB.h"

// Atribui um filme a um cliente, esta funcao nao requer nenhuma
// base de dados passada como parametro, e como tal, o cliente e o filme
// são independentes da base de dados.
void AddFilmeToCliente(Filme*, Cliente*);

// Remove o filme especificado de um cliente, a funcao nao requer nenhuma
// base de dados passada como parametro, e como tal, o cliente e o filme
// são independentes da base de dados.
void RemoverFilmeFromCliente(Filme*, Cliente*);

// Remove o filme especificado de todos os clientes na base de dados.
// Funcao usada para quando se apaga um filme da base de dados principal
void RemoverFilmeFromClientes(Filme*, ClienteDB*);

// Remove o registo do cliente, isto é, todos os filmes
// comprados por este cliente.
void RemoverRegistoCliente(Cliente*);

// Retorna true se o cliente tiver registo(algum filme comprado),
// false se nao tiver nada comprado ou se o cliente for apagado.
bool ClienteTemRegisto(Cliente*); 

// Lista os filmes registados a um cliente na base de dados
// de filmes especificada.
void ListarFilmesByCliente(FilmeDB*, Cliente*);

// Lista todos os clientes na base de dados que têm registo.
// Retorna true se existir algum registo, false se nao existirem registos
bool ListarClientesComRegisto(ClienteDB*);

// Retorna todos os filmes atribuidos a este cliente na base de dados especificada.
// Como só retorna ID's, é função da própria base de dados fazer a ligação com o nome
// do filme, usar uma base de dados diferente da que foi usada para adicionar o filme
// pode retornar um filme completamente diferente porque o ID será igual.
Filme** GetFilmesFromCliente(FilmeDB*, Cliente*);

// Retorna todos os clientes que têm registo na base de dados.
Cliente** GetClientesComRegisto(ClienteDB*);

// Adiciona info ao filme, seja descricao/cast/genre, etc...
void AddInfoToFilme(char* info, Filme*);

// Remove a info deste filme completamente
void RemoverInfoFromFilme(Filme*);

// Mostra a informacao acerca de um determinado filme,
// normalmente a descricao do que se trata o filme.
void MostrarInfoFromFilme(Filme*);

// Lista todos os filmes que têm registo na base de dados.
// Retorna true se existir algum registo, false se nao existirem registos
bool ListarFilmesComInfo(FilmeDB*);

// Retorna todos os filmes que têm registo na base de dados.
Filme** GetFilmesComInfo(FilmeDB*);

// Retorna true se o filme tiver algum registo adicionado
// false se nao tiver nenhum ou se o filme for apagado
bool FilmeTemInfo(Filme*);

// Retorna a informacao do filme especificado para um buffer,
// normalmente a descricao do que se trata o filme.
// É preciso deallocar o buffer
char* GetInfoFromFilme(Filme*);

#endif // CLIENTEFILMEDB_H
