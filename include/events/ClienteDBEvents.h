#include "Cliente.h"
#include "Logger.h"
#include <stdlib.h>

void dbCliente_OnMensagemRecebida(const char* msg, MessageInfoLevel level) {
    LOGF(level == Critical ? LOG_CRITICAL \
       : level == Error    ? LOG_ERROR \
       : level == Warn     ? LOG_WARN \
       : LOG_INFO,
       "%s", msg);

    switch(level) {
        case Info:
        case Warn:
        case Error: {
            break;
        }
        case Critical: {
            LOGF(LOG_CRITICAL, "Houve um erro critico, o programa vai terminar...\n", "");
            exit(EXIT_FAILURE);
        }
    }
}

void dbCliente_OnClienteAdicionado(Cliente cliente) {
    LOGF(LOG_INFO, "Cliente %s(id: %d) adicionado a base de dados!", cliente.nome, cliente.id);
}

void dbCliente_OnClienteRemovido(Cliente cliente) {
    LOGF(LOG_INFO, "Cliente %s(id: %d) removido da base de dados!", cliente.nome, cliente.id);
}