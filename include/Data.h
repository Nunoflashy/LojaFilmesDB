#ifndef DATA_H
#define DATA_H

//-----Getters Data-----
unsigned int GetDia();
unsigned int GetMes();
unsigned int GetAno();
//----------------------

//----Getters Horas-----
unsigned int GetSegundo();
unsigned int GetMinuto();
unsigned int GetHora();
//----------------------

const char* Horas_ToString();
const char* Data_ToString();

#endif // DATA_H
