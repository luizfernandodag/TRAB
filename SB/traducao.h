#ifndef TRADUCAO
#define TRADUCAO
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void traduzADD(FILE *arquivo, char **args, int numArgs);
void traduzSUB(FILE *arquivo, char **args, int numArgs);
void traduzMULT(FILE *arquivo, char **args, int numArgs);
void traduzDIV(FILE *arquivo, char **args, int numArgs);
void traduzJMP(FILE *arquivo, char **args, int numArgs);
void traduzJMPN(FILE *arquivo, char **args, int numArgs);
void traduzJMPP(FILE *arquivo, char **args, int numArgs);
void traduzJMPZ(FILE *arquivo, char **args, int numArgs);
void traduzCOPY(FILE *arquivo, char **args, int numArgs);
void traduzLOAD(FILE *arquivo, char **args, int numArgs);
void traduzSTORE(FILE *arquivo, char **args, int numArgs);
void traduzINPUT(FILE *arquivo, char **args, int numArgs);
void traduzOUTPUT(FILE *arquivo, char **args, int numArgs);
void traduzCONST(FILE *arquivo, char **args, int numArgs);
void traduzSPACE(FILE *arquivo, char **args, int numArgs);
void escreveFuncaoEscreverInteiro(FILE * arq);
void escreveFuncaoLerInteiro(FILE * arq);




#endif