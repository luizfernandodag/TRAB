#include "Bibliotecas/bib.h"					/*Definição das funções*/
#include "Bibliotecas/misc.h"					/*Funções de arquivos e hex -> decimal*/
#include "Bibliotecas/preproc.h"
#include "Bibliotecas/parser.h"
#include "Bibliotecas/scanner.h"
#include "Bibliotecas/analise.h"
#include "Bibliotecas/traducaoIA32.h"

bool debug1 = true;

int main (int argc,char *argv[]) {

	SYMBOL_TABLE *comeco_tabela=NULL, *aux;

	if (!valida_linhacomando(argc, argv))
		return 0;

	/*Pré-processa e salva o arquivo com o nome PREPROCESSED_FILE*/
	pre_processamento(argv[1], PREPROCESSED_FILE);

	/*Faz a análise do código já pré-processado*/
	comeco_tabela = analise(PREPROCESSED_FILE);

	/*Mudando a flag de debug, observe os rótulos*/
	if (debug1){
		printf ("\n\nRotulo\t\t\tEndereco\n");
		for (aux=comeco_tabela; aux != NULL; aux = aux->prox)
			printf ("%s\t\t\t\t%d\n", aux->rotulo, aux->endereco);
	}

	/*Se encontrar erros nem passa para a próxima fase*/
	if (error_count > 0){
		remove(PREPROCESSED_FILE);
		return 0;
	}

	/*TRADUZ PARA ASSEMBLY DE VERDADE*/
	sintese_linguagem(PREPROCESSED_FILE, DEFAULT_OUTPUT_NAME);

	/*FAZENDO O CÓDIGO COM OS OPCODES*/
	sintese_codigo (PREPROCESSED_FILE, DEFAULT_OUTPUT_CODE, comeco_tabela);

	/*DEPOIS QUE JÁ FEZ O QUE TINHA QUE FAZER*/
	remove(PREPROCESSED_FILE);
	return 0;
}
