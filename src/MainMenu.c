#include <stdio.h>
#include <stdlib.h>
#include "FilmeDB.h"
#include "Filme.h"
#include "ClienteDB.h"
#include "Cliente.h"
#include "Data.h"
#include "Logger.h"
#include "Util.h"
#include "App.h"
#include "vec.h"
#include "ClienteFilmeDB.h"
#include "MenuManager.h"
#include "MainMenu.h"
#include "events/ClienteDBEvents.h"
#include "events/FilmeDBEvents.h"

FilmeDB dbFilme;
ClienteDB dbCliente;

void InicializarValores() {
    dbFilme   = CriarFilmeDB("data/filmes.data");
    dbCliente = CriarClienteDB("data/clientes.data");

    //Definir callbacks FilmeDB (definido em FilmeDBEvents.h)
    dbFilme.FilmeMsgCallback        = dbFilme_OnMensagemRecebida;
    dbFilme.FilmeRemovidoCallback   = dbFilme_OnFilmeRemovido;
    dbFilme.FilmeAdicionadoCallback = dbFilme_OnFilmeAdicionado;

    // Definir callbacks ClienteDB (definido em ClienteDBEvents.h)
    dbCliente.ClienteMsgCallback        = dbCliente_OnMensagemRecebida;
    dbCliente.ClienteAdicionadoCallback = dbCliente_OnClienteAdicionado;
    dbCliente.ClienteRemovidoCallback   = dbCliente_OnClienteRemovido;
}

void MostrarRegistoClientesMenu() {
    MenuManager opMenu = Menu_CriarMenu();
    Menu_Add(&opMenu, "Mostrar Registo de Cliente");
    Menu_Add(&opMenu, "Voltar ao Menu");
    Menu_Mostrar(opMenu);
    switch(Menu_GetInput(opMenu)) {
        case 1: { MostrarInfoCliente(); break; }
        case 2: { system("clear"); return; }
    }
    Menu_Free(&opMenu);
}

void ListarClientes(bool subMenu) {
    system("clear");
    printf("ID\t\tNome\n---------------------------------------------------\n");
    for(int i = 0; i < GetNumeroClientes(&dbCliente); i++) {
         Cliente* c = GetCliente(&dbCliente, i);
        printf("%d\t\t%s\n", c->id, c->nome);
    }
    printf("---------------------------------------------------\n");
    printf("\nNumero de Clientes na Base de Dados: %d\n\n", GetNumeroClientes(&dbCliente));
    
    // Mostrar sub-menu de registo de clientes
    if(subMenu) { MostrarRegistoClientesMenu(); }
}

void AdicionarClientes() {
    system("clear"); 
    printf("********** Adicionar Cliente ********** \n");
    int id = GenClienteID(dbCliente);

    printf("Nome Cliente: ");
    char* nome = GetInputString();

    Cliente* c = CriarCliente(id, nome);

    if(!ClienteExiste(&dbCliente, c)) {
        AdicionarCliente(&dbCliente, c);
        return;
    }

    const char* clienteNome = GetClienteByID(&dbCliente, c->id)->nome;
    LOGF(LOG_WARN, "O cliente %s com id %d já existe!\n", clienteNome, c->id);
}

void RemoverClientes() {
    system("clear"); 
    // Listar os clientes disponiveis
    // Prompt para selecionar qual cliente a remover por ID
    // Remover o cliente
    ListarClientes(false);
    printf("********** Remover Cliente ********** \n");
    printf("Insira o ID do cliente que quer remover da base de dados.\n\n: ");
    int idInput = GetInputInt();

    Cliente* c = GetClienteByID(&dbCliente, idInput);
    if(ClienteExiste(&dbCliente, c)) {
        RemoverCliente(&dbCliente, c);
        RemoverRegistoCliente(c);
        return;
    }
    // O ID nao existe na base de dados
    LOGF(LOG_WARN, "O cliente com id %d nao existe!", idInput);
}

void MostrarInfoFilmesMenu() {
    MenuManager opMenu = Menu_CriarMenu();
    Menu_Add(&opMenu, "Mostrar Info de Filme");
    Menu_Add(&opMenu, "Voltar ao Menu");
    Menu_Mostrar(opMenu);
    switch(Menu_GetInput(opMenu)) {
        case 1: { MostrarInfoFilme(); break; }
        case 2: { system("clear"); break; }
    }
    Menu_Free(&opMenu);
}

void MostrarInfoFilme() {
    system("clear"); 
    // Se nao existirem filmes com info registada, retornar
    if(!ListarFilmesComInfo(&dbFilme)) {
        printf("Nao existem filmes com registo!\n");
        return;
    }
    MenuManager menu = Menu_CriarMenu();
    Menu_Add(&menu, "Avançar");
    Menu_Add(&menu, "Voltar ao Menu");
    Menu_Mostrar(menu);
    switch(Menu_GetInput(menu)) {
        case 1: break;
        case 2: system("clear"); return;
    }
    Menu_Free(&menu);

    printf("ID Filme: ");
    int idFilme = GetInputInt();
    Filme* f = GetFilmeByID(&dbFilme, idFilme);

    system("clear");

    if(!FilmeExiste(&dbFilme, f)) {
        printf("O filme não existe!\n");
        EsperarPrompt();
        return;
    }

    if(!FilmeTemInfo(f)) {
        printf("O filme %s não tem nenhum registo.\n", Filme_ToString(*f));
        EsperarPrompt();
        return;
    }

    MostrarInfoFromFilme(f);
    EsperarPrompt();
}

void ListarFilmes(bool subMenu) {
    system("clear");
    printf("ID\t\tFilme\n-----------------------------------------------------------------------------\n");
    for(int i = 0; i < GetNumeroFilmes(&dbFilme); i++) {
        Filme* f = GetFilme(&dbFilme, i);
        printf("%d\t\t%s\n", f->id, f->nome);
    }
    printf("-----------------------------------------------------------------------------\n");
    printf("\nNumero de Filmes na Base de Dados: %d\n\n", GetNumeroFilmes(&dbFilme));

    // Mostrar sub-menu de info de filmes
    if(subMenu) { MostrarInfoFilmesMenu(); }
}

void AdicionarFilmes() {
    system("clear"); 
    printf("********** Adicionar Filme ********** \n");
    int id = GenFilmeID(dbFilme);

    printf("Nome Filme: ");
    char* nome = GetInputString();

    Filme* f = CriarFilme(id, nome);

    if(!FilmeExiste(&dbFilme, f)) {
        AdicionarFilme(&dbFilme, f);

        printf("Gostaria de adicionar informação relacionada com este filme? (y/n): ");
        char input = getchar();
        if(input == 'y') {
            printf("Escreva a informação que pretende adicionar a este filme:\n\n");
            char* buffer = GetInputString();
            AddInfoToFilme(buffer, f);
        }
        system("clear");
        return;
    }

    LOGF(LOG_WARN, "O filme com id %d ja existe!\n", f->id);
}

void RemoverFilmes() {
    system("clear"); 
    // Listar os filmes disponiveis
    // Prompt para selecionar qual filme a remover por ID
    // Remover o filme
    ListarFilmes(false);
    printf("********** Remover Filme ********** \n");
    printf("Insira o ID do filme que quer remover da base de dados.\n\n: ");
    int idInput = GetInputInt();

    Filme* f = GetFilmeByID(&dbFilme, idInput);
    if(FilmeExiste(&dbFilme, f)) {
        RemoverFilme(&dbFilme, f);
        RemoverFilmeFromClientes(f, &dbCliente);
        if(FilmeTemInfo(f)) { RemoverInfoFromFilme(f); }
        return;
    }
    system("clear"); 
    // O ID nao existe na base de dados
    LOGF(LOG_WARN, "O filme com id %d nao existe!", idInput);
}

void AdicionarFilmeCliente() {
    system("clear"); 
    ListarClientes(false);
    ListarClientesComRegisto(&dbCliente);
    printf("ID Cliente: ");
    int idCliente = GetInputInt();
    Cliente* c = GetClienteByID(&dbCliente, idCliente);

    system("clear");
    printf("Cliente %s (ID %d) selecionado\n", c->nome, c->id);

    ListarFilmes(false);
    printf("ID Filme: ");
    int idFilme = GetInputInt();
    Filme* f = GetFilmeByID(&dbFilme, idFilme);

    AddFilmeToCliente(f, c);
}

void MostrarInfoCliente() {
    system("clear"); 
    if(!ListarClientesComRegisto(&dbCliente)) {
        printf("Nao existem clientes com registo...\n");
        return;
    }

    MenuManager menu = Menu_CriarMenu();
    Menu_Add(&menu, "Avançar");
    Menu_Add(&menu, "Voltar ao Menu");
    Menu_Mostrar(menu);

    switch(Menu_GetInput(menu)) {
        case 1: break;
        case 2: system("clear"); return;
    }
    Menu_Free(&menu);

    printf("ID Cliente: ");
    int idCliente = GetInputInt();
    Cliente* c = GetClienteByID(&dbCliente, idCliente);

    system("clear");

    if(!ClienteExiste(&dbCliente, c)) {
        printf("O cliente não existe!\n");
        EsperarPrompt();
        return;
    }

    if(!ClienteTemRegisto(c)) {
        printf("O cliente %s não tem nenhuma compra.\n", Cliente_ToString(*c));
        EsperarPrompt();
        return;
    }

    // Listar todos os filmes que este cliente comprou
    ListarFilmesByCliente(&dbFilme, c);

    MenuManager promptMenu = Menu_CriarMenu();
    Menu_Add(&promptMenu, "Apagar Info de Cliente");
    Menu_Add(&promptMenu, "Voltar ao Menu");
    Menu_Mostrar(promptMenu);
    switch(Menu_GetInput(promptMenu)) {
        case 1: system("clear"); RemoverRegistoCliente(c); break;
        case 2: system("clear"); break;
    }
    Menu_Free(&promptMenu);
}


int main(int argc, char* argv[]) {
    LimparLog();
    InicializarValores();

    MenuManager mainMenu = Menu_CriarMenu();
    Menu_Add(&mainMenu, "Listar Clientes");
    Menu_Add(&mainMenu, "Adicionar Cliente");
    Menu_Add(&mainMenu, "Remover Clientes");
    Menu_Add(&mainMenu, "Listar Filmes");
    Menu_Add(&mainMenu, "Adicionar Filme");
    Menu_Add(&mainMenu, "Remover Filme");
    Menu_Add(&mainMenu, "Adicionar Filme a Cliente");
    Menu_Add(&mainMenu, "Mostrar Info de Cliente");
    Menu_Add(&mainMenu, "Mostrar Info de Filme");
    Menu_Add(&mainMenu, "Sair");

    bool sair = false;

    do {
        Menu_Mostrar(mainMenu);
        switch(Menu_GetInput(mainMenu)) {
            case Menu_ListarClientes:     { ListarClientes(true); break; }
            case Menu_AdicionarCliente:   { AdicionarClientes(); break; }
            case Menu_RemoverCliente:     { RemoverClientes(); break; }
            case Menu_ListarFilmes:       { ListarFilmes(true); break; }
            case Menu_AdicionarFilme:     { AdicionarFilmes(); break; }
            case Menu_RemoverFilme:       { RemoverFilmes(); break; }
            case Menu_FilmeToCliente:     { AdicionarFilmeCliente(); break; }
            case Menu_MostrarInfoCliente: { MostrarInfoCliente(); break; }
            case Menu_MostrarInfoFilme:   { MostrarInfoFilme(); break; }
            case Menu_Sair:               { sair = true; break; }
            default: {
                printf("A opção é invalida!\n");
                sair = true;
            }
        }
    } while(!sair);
    Menu_Free(&mainMenu);

    CloseFilmeDB(&dbFilme);
    CloseClienteDB(&dbCliente);

    return 0;
}
