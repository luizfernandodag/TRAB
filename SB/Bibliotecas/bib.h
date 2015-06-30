/*Bibliotecas*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*Definicões para tamanho dos arrays de leitura de dados*/
#define tokensize		101
#define linesize		312		/* tkn + " : COPY " + tkn + " , " + tkn + '\0'*/

/*Caso o usuário não informe o nome que ele quer para o arquivo de saida*/
#define DEFAULT_OUTPUT_NAME		"arquivo.s"
#define TEMP_OUTPUT				"temp.asm"
#define PREPROCESSED_FILE		"prep.asm"

/*************************************** DEFINIÇÕES TABELAS E ESTRUTURAS DE DADOS *****************************************/
typedef struct SYMBOL_TABLE_CELL SYMBOL_TABLE;

struct SYMBOL_TABLE_CELL {

	struct SYMBOL_TABLE_CELL *prox;
	char *rotulo;						/*identificador*/
	int endereco;						/*endereço base*/
	int tamanho;						/*suporte à vetores*/
	bool constante;						/*se for constante não posso alterar depois*/
	bool executavel;					/*posso executar (pular para) ou nao*/
};

/*************************************************** DEFINIÇÕES FUNÇÕES ***************************************************/

/*ALTO NÍVEL*/
bool valida_linhacomando (int argc, char **argv);
void pre_processamento (char *entrada, char *saida);
void analise (char *src_name, SYMBOL_TABLE *comeco_tabela);
void sintese_linguagem (char *src_name, char *dst_name);
void relata_erros (int codigo, char *aux, char *aux2);

/*Pre-processamento*/
void processa_equ (char *src_name, char *dst_name);
void processa_if (char *src_name, char *dst_name);

/*Scanner*/
bool get_linha (FILE *src, char *buffer);
bool get_token (char *linha, char *token);
void ignora_linha (FILE *src );
bool fim_token (char c);

/*Parser*/
bool separa_linha (char *buffer, char *rotulo, char *instr, char *dir, char *opr1, int *mod1, char *opr2, int *mod2);
bool eh_instr (char *token);
bool eh_diretiva (char *token);
bool eh_num (char *token);
bool rotulo_valido (char *token);
bool eh_modificador_valido (char *buffer);

/*Utilização da tabela de símbolos*/
SYMBOL_TABLE* add_tab_simbolos (char *rotulo, bool eh_constante, bool eh_executavel, int tamanho);
void apaga_tab_simbolos (SYMBOL_TABLE *prim);
bool tem_tabela_simbolos (SYMBOL_TABLE *prim, char *alvo);
SYMBOL_TABLE* busca_tabela (SYMBOL_TABLE *prim, char *alvo);

/*Tradução IA-32*/
void traduzADD(FILE *arquivo, char *arg1, int offset1);
void traduzSUB(FILE *dst, char *arg1, int offset1);
void traduzMULT(FILE *dst, char *arg1, int offset1);
void traduzDIV(FILE *dst, char *arg1, int offset1);
void traduzJMP(FILE *dst, char *arg1);
void traduzJMPN(FILE *dst, char *arg1);
void traduzJMPP(FILE *dst, char *arg1);
void traduzJMPZ(FILE *dst, char *arg1);
void traduzCOPY(FILE *dst, char *arg1, int offset1, char *arg2, int offset2);
void traduzLOAD(FILE *dst, char *arg1, int offset1);
void traduzSTORE(FILE *dst, char *arg1, int offset1);
void traduzINPUT(FILE *dst, char *arg1, int offset1);
void traduzOUTPUT(FILE *dst, char *arg1, int offset1);
void traduzSTOP (FILE *dst);
void traduzSECTION (FILE *dst, char *arg1);
void traduzCONST(FILE *dst, char *arg1, int offset1);
void traduzSPACE(FILE *dst, char *arg1, int offset1);
void escreveFuncaoEscreverInteiro(FILE * arq);
void escreveFuncaoLerInteiro(FILE * arq);

/*Outros*/
int converte_int (char *token);					/*Converte uma sequencia de caracteres em 1 número*/
bool copy_file (char *src, char *dst);			/*Copia um arquivo para outro lugar*/

/*Linguagem*/
int tam_instr (char *instr, int mod1, char *dir);
int retorna_opcode (char *instr);

/**************************************** VARIÁVEIS GLOBAIS: ESTADO DE EXECUÇÃO *******************************************/
int error_count = 0;
int passagem = 1;
int line_counter = 0;
int mem_counter = 0;
int section = 0;
bool tem_codigo = false;
bool usa_input = false;
bool usa_output = false;
bool globalstart_defined = false;
