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

	/*Pré-processa e salva o arquivo com o nome PREPROCESSED_FILE*/
	pre_processamento(argv[1], PREPROCESSED_FILE);

	/*Faz a análise do código já pré-processado*/
	entrada = fopen(PREPROCESSED_FILE, "r");
	analise(entrada, comeco_tabela);

	/*
	sintese (codigo máquina inventado)
	sintese (codigo IA32)
	sintese ELF
	*/

	/*DEPOIS QUE JÁ FEZ O QUE TINHA QUE FAZER*/
	//remove(PREPROCESSED_FILE);
	return 0;
}
