#include "Bibliotecas/bib.h"					/*Defini��o das fun��es*/
#include "Bibliotecas/misc.h"					/*Fun��es de arquivos e hex -> decimal*/
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

	/*Pr�-processa e salva o arquivo com o nome PREPROCESSED_FILE*/
	pre_processamento(argv[1], PREPROCESSED_FILE);

	/*Faz a an�lise do c�digo j� pr�-processado*/
	comeco_tabela = analise(PREPROCESSED_FILE);

	if (debug1){
		printf ("\n\nRotulo\t\t\tEndereco\n");
		for (aux=comeco_tabela; aux != NULL; aux = aux->prox)
			printf ("%s\t\t\t\t%d\n", aux->rotulo, aux->endereco);
	}

	/*Se encontrar erros nem passa para a pr�xima fase*/
	if (error_count > 0){
		remove(PREPROCESSED_FILE);
		return 0;
	}

	/*TRADUZ PARA ASSEMBLY DE VERDADE*/
	sintese_linguagem(PREPROCESSED_FILE, DEFAULT_OUTPUT_NAME);

	/*FAZENDO O C�DIGO COM OS OPCODES*/
	/*sintese_codigo (PREPROCESSED_FILE, DEFAULT_OUTPUT_CODE_NAME);*/

	/*DEPOIS QUE J� FEZ O QUE TINHA QUE FAZER*/
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
	SYMBOL_TABLE *aux;

	in = fopen(in_name, "r");
	dst = fopen(out_name, "w+");

	while (get_linha(in, buffer)){

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		separa_linha(buffer, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		aux = busca_tabela(comeco_tabela, opr1);

		if (!strcmp(instr, "add") || !strcmp(instr, "ADD"))
			fprintf (dst, "05 03 %#010x\n", converte_littleendian(aux->endereco));

		else if (!strcmp(instr, "sub") || !strcmp(instr, "SUB"))
			fprintf (dst, "2B 05 %#010x\n", converte_littleendian(aux->endereco));

		else if (!strcmp(instr, "mult") || !strcmp(instr, "MULT"))
			fprintf (dst, "F7 25 %#010x\n", converte_littleendian(aux->endereco));

		else if (!strcmp(instr, "div") || !strcmp(instr, "DIV"))
			fprintf (dst, "F7 35 %#010x\n", converte_littleendian(aux->endereco));

/**/
		else if (!strcmp(instr, "jmp") || !strcmp(instr, "JMP"))
			fprintf (dst, "EB %d\n", mod1);

		else if (!strcmp(instr, "jmpp") || !strcmp(instr, "JMPP"))
			traduzJMP(dst, opr1);

		else if (!strcmp(instr, "jmpn") || !strcmp(instr, "JMPN"))
			traduzJMP(dst, opr1);

		else if (!strcmp(instr, "jmpz") || !strcmp(instr, "JMPZ"))
			traduzJMP(dst, opr1);

/**/
		else if (!strcmp(instr, "copy") || !strcmp(instr, "COPY"))
			traduzCOPY(dst, opr1, mod1, opr2, mod2);

		else if (!strcmp(instr, "load") || !strcmp(instr, "LOAD"))
			traduzLOAD(dst, opr1, mod1);

		else if (!strcmp(instr, "store") || !strcmp(instr, "STORE"))
			traduzSTORE(dst, opr1, mod1);

		else if (!strcmp(instr, "input") || !strcmp(instr, "INPUT")) {
			traduzINPUT(dst, opr1, mod1);
			usa_input = true;
		}

		else if (!strcmp(instr, "output") || !strcmp(instr, "OUTPUT")){
			traduzOUTPUT(dst, opr1, mod1);
			usa_output = true;
		}

		else if (!strcmp(instr, "stop") || !strcmp(instr, "STOP"))
			traduzSTOP(dst);

		else if (!strcmp(dir, "section") || !strcmp(instr, "SECTION")){
			traduzSECTION(dst, opr1);

			/*Definindo o global start*/
			if (globalstart_defined==false && !strcmp(opr1, "text")) {
				fprintf (dst, "\n_start:\n");
				globalstart_defined = true;
			}
		}

		else if (!strcmp(dir, "space"))
			traduzSPACE(dst, rotulo, mod1);

		else if (!strcmp(dir, "const"))
			traduzCONST(dst, rotulo, mod1);
	}

	/*Adicionando as bibliotecas ao arquivo execut�vel*/

	if (usa_input || usa_output){
		fprintf (dst, "\nsection .data");
		fprintf (dst, "\n_enter_ db ah");
		fprintf (dst, "\nnumEntrada dd 0");
		fprintf (dst, "\n_leitura_ dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaida dd 0");
		fprintf (dst, "\n_dez_ dd 10");
		fprintf (dst, "\nnumSaidaString dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaidaStringAux dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");

		if (usa_input)
			escreveFuncaoLerInteiro(dst);

		if (usa_output)
			escreveFuncaoEscreverInteiro(dst);

	}

	fclose(in);
	fclose(dst);

}

