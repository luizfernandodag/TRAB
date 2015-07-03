#include "Bibliotecas/bib.h"					/*Definição das funções*/
#include "Bibliotecas/misc.h"					/*Funções de arquivos e hex -> decimal*/
#include "Bibliotecas/preproc.h"
#include "Bibliotecas/parser.h"
#include "Bibliotecas/scanner.h"
#include "Bibliotecas/analise.h"

#include "Bibliotecas/traducaoIA32.h"

bool debug1 = false;

void sintese_codigo (char *in_name, char *out_name, SYMBOL_TABLE *comeco_tabela);

int main (int argc,char *argv[]) {

	SYMBOL_TABLE *comeco_tabela=NULL, *aux;

	if (!valida_linhacomando(argc, argv))
		return 0;

	/*Pré-processa e salva o arquivo com o nome PREPROCESSED_FILE*/
	pre_processamento(argv[1], PREPROCESSED_FILE);

	/*Faz a análise do código já pré-processado*/
	comeco_tabela = analise(PREPROCESSED_FILE);

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
	/*sintese_codigo (PREPROCESSED_FILE, DEFAULT_OUTPUT_CODE_NAME);*/

	/*DEPOIS QUE JÁ FEZ O QUE TINHA QUE FAZER*/
	remove(PREPROCESSED_FILE);
	return 0;
}

void sintese_codigo (char *in_name, char *out_name, SYMBOL_TABLE *comeco_tabela){

	FILE *in, *dst;

	/*Buffer para a linha*/
	char buffer[linesize];

	/*Tokens separados*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	/*Busca na tabela de simbolos*/
	SYMBOL_TABLE *aux, *aux2;

	in = fopen(in_name, "r");
	dst = fopen(out_name, "w+");

	mem_counter_code = mem_counter_data = 0;
	while (get_linha(in, buffer)){

		mem_counter_code += tam_instr(instr, mod1);
		mem_counter_data += tam_dir(dir, mod1);

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		separa_linha(buffer, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		aux = busca_tabela(comeco_tabela, opr1);

		if (!strcmp(instr, "add") || !strcmp(instr, "ADD"))
			fprintf (dst, " 03 05 %#010x\n", converte_littleendian(aux->endereco));

		else if (!strcmp(instr, "sub") || !strcmp(instr, "SUB"))
			fprintf (dst, " 2B 05 %#010x\n", converte_littleendian(aux->endereco));

		else if (!strcmp(instr, "mult") || !strcmp(instr, "MULT"))
			fprintf (dst, " F7 25 %#010x\n", converte_littleendian(aux->endereco));

		else if (!strcmp(instr, "div") || !strcmp(instr, "DIV"))
			fprintf (dst, " F7 35 %#010x\n", converte_littleendian(aux->endereco));

		else if (!strcmp(instr, "jmp") || !strcmp(instr, "JMP"))
			fprintf (dst, " E9 %#010x\n", converte_littleendian(aux->endereco - mem_counter_code));

		else if (!strcmp(instr, "jmpp") || !strcmp(instr, "JMPP")){
			fprintf (dst, "83 F8 00 ");
			fprintf (dst, " 0F 87 %#010x\n", converte_littleendian(aux->endereco - mem_counter_code));
		}

		else if (!strcmp(instr, "jmpn") || !strcmp(instr, "JMPN")){
			fprintf (dst, "83 F8 00 ");
			fprintf (dst, " 0F 82 %#010x\n", converte_littleendian(aux->endereco - mem_counter_code));
		}

		else if (!strcmp(instr, "jmpz") || !strcmp(instr, "JMPZ")){
			fprintf (dst, "83 F8 00 ");
			fprintf (dst, " 0F 84 %#010x\n", converte_littleendian(aux->endereco - mem_counter_code));
		}

		else if (!strcmp(instr, "copy") || !strcmp(instr, "COPY")) {
			aux2 = busca_tabela(comeco_tabela, opr2);

			fprintf (dst, "53 ");
			fprintf (dst, "8b 1d ", converte_littleendian(aux->endereco - mem_counter_code));
			fprintf (dst, "89 1d ", converte_littleendian(aux2->endereco - mem_counter_code));
			fprintf (dst, "5b");
		}

		else if (!strcmp(instr, "load") || !strcmp(instr, "LOAD"))
			fprintf (dst, "A1 %#010x\n", converte_littleendian(aux->endereco));

		else if (!strcmp(instr, "store") || !strcmp(instr, "STORE"))
			;

		else if (!strcmp(instr, "input") || !strcmp(instr, "INPUT")) {
			;
		}

		else if (!strcmp(instr, "output") || !strcmp(instr, "OUTPUT")){
			;
		}

		else if (!strcmp(instr, "stop") || !strcmp(instr, "STOP"))
			;

		else if (!strcmp(dir, "section") || !strcmp(instr, "SECTION")){
			;

			/*Definindo o global start*/
			if (globalstart_defined==false && !strcmp(opr1, "text")) {
				;
			}
		}

		else if (!strcmp(dir, "space"))
			;

		else if (!strcmp(dir, "const"))
			;
	}

	/*Adicionando as bibliotecas ao arquivo executável*/

	if (usa_input || usa_output){
		fprintf (dst, "\nsection .data");
		fprintf (dst, "\n_enter_ db ah");
		fprintf (dst, "\nnumEntrada dd 0");
		fprintf (dst, "\n_leitura_ dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaida dd 0");
		fprintf (dst, "\n_dez_ dd 10");
		fprintf (dst, "\nnumSaidaString dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaidaStringAux dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");


		if (usa_input) {
			;
		}

		if (usa_output)
			;

	}

	fclose(in);
	fclose(dst);

}

