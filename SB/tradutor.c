#include "bib.h"
#include "preproc.h"
#include "parser.h"
#include "scanner.h"
#include "traducaoIA32.h"

int main (int argc,char *argv[]) {

	FILE *entrada=NULL, *saida=NULL;
	SYMBOL_TABLE *comeco_tabela=NULL;

	if (!valida_linhacomando(argc, argv))
		return 0;

	/*Pr�-processa e salva o arquivo com o nome PREPROCESSED_FILE*/
	pre_processamento(argv[1], PREPROCESSED_FILE);

	/*Faz a an�lise do c�digo j� pr�-processado*/
	entrada = fopen(PREPROCESSED_FILE, "r");
	analise(entrada, comeco_tabela);

	/*
	sintese (codigo m�quina inventado)
	sintese (codigo IA32)
	sintese ELF
	*/

	/*DEPOIS QUE J� FEZ O QUE TINHA QUE FAZER*/
	//remove(PREPROCESSED_FILE);
	return 0;
}
