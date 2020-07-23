#include "Filme.h"
#include "Logger.h"
#include <stdlib.h>
#include "MessageInfoLevel.h"

void dbFilme_OnMensagemRecebida(const char* msg, MessageInfoLevel level) {
    LOGF(level == Critical ? LOG_CRITICAL \
       : level == Warn     ? LOG_WARN \
       : LOG_INFO,
       "%s", msg);

    switch(level) {
        case Info:
        case Warn:
        case Error:
        case Critical: {
            LOGF(LOG_CRITICAL, "Houve um erro critico, o programa vai terminar...\n", "");
            exit(EXIT_FAILURE);
        }
    }
}

void dbFilme_OnFilmeAdicionado(Filme filme) {
    LOGF(LOG_INFO, "Filme %s adicionado a base de dados!", filme.nome);
}

void dbFilme_OnFilmeRemovido(Filme filme) {
    LOGF(LOG_INFO, "Filme %s removido da base de dados!", filme.nome);
}