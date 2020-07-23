#ifndef MAIN_MENU_H
#define MAIN_MENU_H

enum {
    Menu_ListarClientes = 1,
    Menu_AdicionarCliente,
    Menu_RemoverCliente,
    Menu_ListarFilmes,
    Menu_AdicionarFilme,
    Menu_RemoverFilme,
    Menu_FilmeToCliente,
    Menu_MostrarInfoCliente,
    Menu_MostrarInfoFilme,
    Menu_Sair
};

// Init func
void InicializarValores();

//-------Clientes----------
void ListarClientes(bool subMenu);
void AdicionarClientes();
void RemoverClientes();
void MostrarRegistoClientesMenu();
//-------------------------

//---------Filmes----------
void ListarFilmes(bool subMenu);
void AdicionarFilmes();
void RemoverFilmes();
void MostrarInfoFilmesMenu();
void MostrarInfoFilme();
//-------------------------

//--Relacao Filme-Cliente--
void AdicionarFilmeCliente();
void MostrarInfoCliente();
//-------------------------

#endif // MAIN_MENU_H