#ifndef UTILIDADE_H
#define UTILIDADE_H

#include <stdbool.h>
#include <stdio.h>

//----Strings-------------------------------------------
bool string_IsEqual(const char *str1, const char *str2);
bool string_IsEmpty(const char *str);
//------------------------------------------------------

//----Ficheiros-----------------------------------------
size_t Ficheiro_GetTamanhoBytes(FILE*);
size_t Ficheiro_GetTamanhoBytesLinha(FILE* f, int linha);
int    Ficheiro_GetLinhas(FILE*);
char*  Ficheiro_GetLinha(FILE*, int linha);
char*  Ficheiro_GetChars(FILE*);
void   Ficheiro_Limpar(const char* ficheiro);
void   Ficheiro_ApagarLinha(const char* ficheiro, int linha);
//------------------------------------------------------

//----Input/Prompt--------------------------------------
int   GetInputInt();
char* GetInputString();
void  EsperarPrompt();
//------------------------------------------------------

//----Algoritmos Sorting--------------------------------
void Bubblesort(int* arr, int n);
//------------------------------------------------------

#endif // UTILIDADE_H
