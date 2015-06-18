#include "Bibliotecas/bib.h"
#include "Bibliotecas/preproc.h"
#include "Bibliotecas/parser.h"
#include "Bibliotecas/scanner.h"

#include "Bibliotecas/traducaoIA32.h"

/*Função que eu quero construir antes de mover para uma biblioteca*/
void sintese_IA32 (char *src, char *dst);

int main (int argc,char *argv[]) {

	SYMBOL_TABLE *comeco_tabela=NULL;

	if (!valida_linhacomando(argc, argv))
		return 0;

	/*Pré-processa e salva o arquivo com o nome PREPROCESSED_FILE*/
	pre_processamento(argv[1], PREPROCESSED_FILE);

	/*Faz a análise do código já pré-processado*/
	analise(PREPROCESSED_FILE, comeco_tabela);

	/*Se encontrar erros nem passa para a próxima fase*/
	if (error_count != 0){
		remove(PREPROCESSED_FILE);
		return 0;
	}

	/*
	sintese (linguagem IA32)
	sintese (codigo maquina IA32)
	sintese ELF
	*/

	/*DEPOIS QUE JÁ FEZ O QUE TINHA QUE FAZER*/
	remove(PREPROCESSED_FILE);
	return 0;
}

void sintese_linguagem (char *src_name, char *dst_name) {

	FILE *src, *dst;

	src = fopen(src_name, "r");
	dst = fopen(dst_name, "w");

	fclose(src);
	fclose(dst);

}
