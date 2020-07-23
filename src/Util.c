#include "Util.h"
#include <stdlib.h>
#include <stdint.h>
#include "Logger.h"
#include "App.h"

bool string_IsEqual(const char *str1, const char *str2) {
    if(strcmp(str1, str2) == 0)
        return true;
    return false;
}

bool string_IsEmpty(const char *str) {
    return str == "" || str == NULL;
}

size_t Ficheiro_GetTamanhoBytes(FILE* f) {
    if(f == NULL)
        return -1;

    fseek(f, 0, SEEK_END);
    size_t bytes = ftell(f);
    return bytes;
}

size_t Ficheiro_GetTamanhoBytesLinha(FILE* f, int linha) {
    char* dataLinha = Ficheiro_GetLinha(f, linha);
    return strlen(dataLinha) +1;
}

 char* Ficheiro_GetLinha(FILE* file, int linha) {
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    char* ptr = buffer;
    int linhaAtual = 0;
    fseek(file, 0, SEEK_SET);
    while(fgets(buffer, BUFFER_SIZE, file) != '\n') {
        strtok(ptr, "\n"); // Remover \n da string
        if(linhaAtual == linha) {
            break;
        }
        linhaAtual++;
    }
    rewind(file);
    return ptr;
}

char* Ficheiro_GetChars(FILE* f) {
    size_t bytes = Ficheiro_GetTamanhoBytes(f);
    char* buffer = malloc(bytes);

    char c;
    int i = 0;
    fseek(f, 0, SEEK_SET);
    while((c = getc(f)) != EOF) {
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    rewind(f);
    return buffer;
}

int Ficheiro_GetLinhas(FILE* file) {
    int linhaCount = 0;
    char buffer[4096];
    fseek(file, 0, SEEK_SET);
    while(fgets(buffer, 4096, file) != NULL)
        linhaCount++;

    rewind(file);
    return linhaCount;
}

void Ficheiro_Limpar(const char* ficheiro) {
    fclose(fopen(ficheiro, "w"));
}

void Ficheiro_ApagarLinha(const char* ficheiro, int linha) {
    /* Para remover uma linha de um ficheiro, é preciso
    copiar o ficheiro todo para um buffer e ignorar a linha que
    é suposto apagar.
    Para isso, precisamos de usar um ficheiro temporario para guardar o conteudo
    no fim de a linha ser ignorada, e depois mover o ficheiro temporario para o original,
    dando a ilusão que o original foi modificado. */
    if(string_IsEmpty(ficheiro))
        return;
    
    FILE* fin = fopen(ficheiro, "r");
    int numLinhas = Ficheiro_GetLinhas(fin);

    if(fin == NULL || linha > numLinhas)
        return;
        
    char** dataLinhas = malloc(sizeof(char*) * numLinhas);
        for(int i = 0; i < numLinhas; i++) {
        // Alocar espaco para a linha atual
        dataLinhas[i] = malloc(Ficheiro_GetTamanhoBytesLinha(fin, i));
    }

    for(int i = 0; i < numLinhas; i++) {
        snprintf(dataLinhas[i], Ficheiro_GetTamanhoBytesLinha(fin, i), "%s", Ficheiro_GetLinha(fin, i));
        //DEBUG_LOG(LOG_INFO, "Linha %d: %d bytes", i+1, Ficheiro_GetTamanhoBytesLinha(fin, i));
    }
    // Criar ficheiro temp a partir do original
    char fTempPath[1024];
    // Guardar o nome original em tempStr para dar rename mais tarde
    char* tempStr = malloc(strlen(ficheiro)+1);
    strcpy(tempStr, ficheiro);
    //------------------------------------
    snprintf(fTempPath, 1024, "%s.tmp", ficheiro);
    FILE* ftemp = fopen(fTempPath, "w");
    // ----------------------------------------

    for(int i = 0; i < numLinhas; i++) {
        if(i+1 == linha) {
            // Estamos na linha que é suposto apagar
            // ignorar esta linha ao escrever para o ficheiro temp
            //DEBUG_LOG(LOG_INFO, "Linha %d apagada", i+1);
            continue;
        }
        fprintf(ftemp, "%s\n", dataLinhas[i]);
    }
    fclose(ftemp);
    fclose(fin);

    for(int i = 0; i < numLinhas; i++)
        free(dataLinhas[i]);
    free(dataLinhas);

    // Mover o ficheiro alterado para o original
    rename(fTempPath, tempStr);
    free(tempStr);
}

//----Input---------------------------------------------
int GetInputInt() {
    int in;
    scanf("%d", &in);
    return in;
}

char* GetInputString() {
    char buffer[1024], *ptr = buffer;
//    scanf("%s", buffer);
//    return ptr;
    gets(buffer);
    gets(buffer);
    return ptr;
}
//------------------------------------------------------

//----Algoritmos Sorting--------------------------------
void swap(int* xp, int* yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void Bubblesort(int* arr, int n) {
    for(int i = 0; i < n-1; i++) {
        for(int j = 0; j < n-i-1; j++) {
            if(arr[j] > arr[j+1])
                swap(&arr[j], &arr[j+1]);
        }
    }
}
//------------------------------------------------------

void EsperarPrompt() {
    printf("\nEnter para voltar ao menu...\n");
    getchar();
    getchar();
    system("clear");
}