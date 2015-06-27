#include "Bibliotecas/bib.h"					/*Defini��o das fun��es*/
#include "Bibliotecas/misc.h"					/*Fun��es de arquivos e hex -> decimal*/
#include "Bibliotecas/preproc.h"
#include "Bibliotecas/parser.h"
#include "Bibliotecas/scanner.h"
#include "Bibliotecas/analise.h"

#include "Bibliotecas/traducaoIA32n_beta.h"

bool usa_input = false;
bool usa_output = false;
bool globalstart_defined = false;

void sintese_linguagem (char *src_name, char *dst_name);

int main (int argc,char *argv[]) {

	SYMBOL_TABLE *comeco_tabela=NULL;

	if (!valida_linhacomando(argc, argv))
		return 0;

	/*Pr�-processa e salva o arquivo com o nome PREPROCESSED_FILE*/
	pre_processamento(argv[1], PREPROCESSED_FILE);

	/*Faz a an�lise do c�digo j� pr�-processado*/
	analise(PREPROCESSED_FILE, comeco_tabela);

	/*Se encontrar erros nem passa para a pr�xima fase*/
	if (error_count > 0){
		remove(PREPROCESSED_FILE);
		return 0;
	}

	/*TRADUZ PARA ASSEMBLY DE VERDADE*/
	sintese_linguagem(PREPROCESSED_FILE, DEFAULT_OUTPUT_NAME);

	/*DEPOIS QUE J� FEZ O QUE TINHA QUE FAZER*/
	remove(PREPROCESSED_FILE);
	return 0;
}

void sintese_linguagem (char *src_name, char *dst_name) {

	FILE *src, *dst;

	char buffer[linesize];

	/*Tokens separados*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	src = fopen(src_name, "r");
	dst = fopen(dst_name, "w");

	/*Esse peda�o � necess�rio em qualquer arquivo*/

        fprintf (dst, "\nsection .data");
		fprintf (dst, "\nnumEntrada dd 0");
		fprintf (dst, "\nleitura dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaida dd 0");
		fprintf (dst, "\ndez dd 10");
		fprintf (dst, "\nnumSaidaString dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaidaStringAux dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");



	fprintf(dst, "global _start\n");

escreveFuncaoEscreverInteiro(dst);
escreveFuncaoLerInteiro(dst);

	while (get_linha(src, buffer)){

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		separa_linha(buffer, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);
printf("rotulo = %s instr = %s dir = %s \n", rotulo,  instr, dir );
		if (!strcmp(instr, "add") || !strcmp(instr, "ADD"))
			traduzADD(dst, opr1, mod1);

		else if (!strcmp(instr, "sub") || !strcmp(instr, "SUB"))
			traduzSUB(dst, opr1, mod1);

		else if (!strcmp(instr, "mult") || !strcmp(instr, "MULT"))
			traduzMULT(dst, opr1, mod1);

		else if (!strcmp(instr, "div") || !strcmp(instr, "DIV"))
			traduzDIV(dst, opr1, mod1);

		else if (!strcmp(instr, "jmp") || !strcmp(instr, "JMP"))
			traduzJMP(dst, opr1);

		else if (!strcmp(instr, "jmpp") || !strcmp(instr, "JMPP"))
			traduzJMP(dst, opr1);

		else if (!strcmp(instr, "jmpn") || !strcmp(instr, "JMPN"))
			traduzJMP(dst, opr1);

		else if (!strcmp(instr, "jmpz") || !strcmp(instr, "JMPZ"))
			traduzJMP(dst, opr1);

		else if (!strcmp(instr, "copy") || !strcmp(instr, "COPY"))
			traduzCOPY(dst, opr1, mod1, opr2, mod2);

		else if (!strcmp(instr, "load") || !strcmp(instr, "LOAD"))
			traduzLOAD(dst, opr1, mod1);

		else if (!strcmp(instr, "store") || !strcmp(instr, "STORE"))
			traduzSTORE(dst, opr1, mod1);

		else if (!strcmp(instr, "input") || !strcmp(instr, "INPUT")) {
			traduzINPUT(dst, opr1, mod1);
printf("AQUI traduz input \n");
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

	if (usa_input)
{
		escreveFuncaoLerInteiro(dst);
printf("AQUI escreve inteiro entrei \n");
}
	if (usa_output)
		escreveFuncaoEscreverInteiro(dst);

	if (usa_input || usa_output) {
		fprintf (dst, "\nsection data");
		fprintf (dst, "\nnumEntrada dd 0");
		fprintf (dst, "\nleitura dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaida dd 0");
		fprintf (dst, "\ndez dd 10");
		fprintf (dst, "\nnumSaidaString dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaidaStringAux dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");
	}

	fclose(src);
	fclose(dst);

}
