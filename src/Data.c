#include "Data.h"
#include <stdio.h>
#include <time.h>
#include "Util.h"

typedef struct {
    unsigned int dia, mes, ano;
} Data;

//--------private---------
static Data GetData();
static struct tm* GetTempo();
//------------------------

unsigned int GetDia() {
    return GetData().dia;
}

unsigned int GetMes() {
    return GetData().mes;
}

unsigned int GetAno() {
    return GetData().ano;
}

unsigned int GetSegundo() {
    int sec = GetTempo()->tm_sec;
    return sec;
}

unsigned int GetMinuto() {
    int min = GetTempo()->tm_min;
    return min;
}

unsigned int GetHora() {
    int hour = GetTempo()->tm_hour;
    return hour;
}



const char* Horas_ToString() {
    char buffer[25];

    unsigned int sec = GetSegundo();
    unsigned int min = GetMinuto();
    unsigned int hora = GetHora();

    snprintf(buffer, 25, "%.2d:%.2d:%.2d", hora, min, sec);
    const char* retval = buffer;
    return retval;
}

const char* Data_ToString() {
    char buffer[25];
    Data data = GetData();
    snprintf(buffer, 25, "%.2d/%.2d/%d", data.dia, data.mes, data.ano);
    const char* retval = buffer;
    return retval;
}


//--------private--------
Data GetData() {
    Data data;
    char buffer[10];

    // Processar dia
    snprintf(buffer, 3, "%c%c", __DATE__[4], __DATE__[5]);
    data.dia = atoi(buffer);

    // Processar mes
    snprintf(buffer, 4, "%c%c%c", __DATE__[0], __DATE__[1], __DATE__[2]);
    if(string_IsEqual(buffer, "Jan"))       data.mes = 1;
    else if(string_IsEqual(buffer, "Feb"))  data.mes = 2;
    else if(string_IsEqual(buffer, "Mar"))  data.mes = 3;
    else if(string_IsEqual(buffer, "Apr"))  data.mes = 4;
    else if(string_IsEqual(buffer, "May"))  data.mes = 5;
    else if(string_IsEqual(buffer, "Jun"))  data.mes = 6;
    else if(string_IsEqual(buffer, "Jul"))  data.mes = 7;
    else if(string_IsEqual(buffer, "Ago"))  data.mes = 8;
    else if(string_IsEqual(buffer, "Sep"))  data.mes = 9;
    else if(string_IsEqual(buffer, "Oct"))  data.mes = 10;
    else if(string_IsEqual(buffer, "Nov"))  data.mes = 11;
    else if(string_IsEqual(buffer, "Dec"))  data.mes = 12;

    // Processar ano
    snprintf(buffer, 5, "%c%c%c%c", __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10]);
    data.ano = atoi(buffer);

    //printf("Data atualizada\n");
    return data;

}

struct tm* GetTempo() {
    time_t now = time(NULL);
    struct tm* now_tm = localtime(&now);
    return now_tm;
}
//--------------------------
