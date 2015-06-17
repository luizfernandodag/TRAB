/*Bibliotecas*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*Definicões para tamanho dos arrays de leitura de dados*/
#define tokensize		101
#define linesize		312		/* tkn + " : COPY " + tkn + " , " + tkn + '\0'*/

/*Caso o usuário não informe o nome que ele quer para o arquivo de saida*/
#define DEFAULT_OUTPUT_NAME		"default.asm"
#define TEMP_OUTPUT				"temp.asm"
#define PREPROCESSED_FILE		"prep.txt"

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
bool abre_arquivos (int argc, char *argv[], FILE *entrada, FILE *saida);
void pre_processamento (char *entrada);
void analise (FILE *entrada, SYMBOL_TABLE *comeco_tabela);
void sintese (FILE *entrada, FILE *saida, SYMBOL_TABLE *comeco_fila);
void relata_erros (int codigo, char *aux, char *aux2);

/*Abre os arquivos independente da terminação*/
FILE* open_sourcefile (char *name);
FILE* open_exitfile (char *name);

/*Pre-processamento*/
void processa_equ (char *src_name);
void processa_if (FILE *entrada, FILE *saida);

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

/*Outros*/
int converte_int (char *token);
bool copy_file (char *src, char *dst);

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

/**************************************** DEFINIÇAO DE FUNÇOES ***********************************************************/

/*Retorna true se o usuário usou a linha de comando certinho e falso se o usuario fez cagada na linha de comando*/
bool valida_linhacomando (int argc, char **argv) {

	FILE *file_pointer;

	/*0 argumentos, nem tem o que fazer*/
	if (argc<2){
		relata_erros(100, NULL, NULL);
		return false;
	}

	/*Se tiver mais que 1 argumento tambem pare o usuário*/
	if (argc>2){
		relata_erros(101, NULL, NULL);
		return false;
	}

	/*Tente abrir o arquivo que o usuário pediu*/
	file_pointer = open_sourcefile(argv[1]);

	if (file_pointer == NULL){
		relata_erros(102, NULL, NULL);
		return false;
	}

	fclose(file_pointer);
	return true;
}

/*Faz a análise do código, procurando por erros e mexendo na variável global que mede erros*/
void analise (FILE *entrada, SYMBOL_TABLE *comeco_tabela ) {

	/*Buffers*/
	char linha[linesize];

	/*Tokens separados*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	/*Tabelas*/
	SYMBOL_TABLE *ultimo_fila, *aux;
	comeco_tabela = ultimo_fila = aux = NULL;

	for (; get_linha(entrada, linha) == true ; mem_counter += tam_instr(instr, mod1, dir)) {

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		/*Separa os tokens encontrados nas caixinhas certas*/
		separa_linha(linha, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		/*Entrando na seção text*/
		if (!strcmp("section", dir) && !strcmp("text", opr1)){
			tem_codigo = true;
			section = 1;
		}

		/*Ou entrando na seção data*/
		else if (!strcmp("section", dir) && !strcmp("data", opr1))
			section = 2;

		/*Ou já está na seção text*/
		else if (section == 1) {

			/*Nenhuma diretiva pode vir aqui dentro*/
			if (*dir != '\0')
				relata_erros(214, dir, NULL);

			/*Se cair aqui, eh uma instrucao comum. Se tiver rótulo vai para a tabela de simbolos, se não tiver tudo certo*/
			else if (*rotulo != '\0' && rotulo_valido(rotulo)) {
				if ((aux=busca_tabela(comeco_tabela,rotulo)) == NULL) {
					if (comeco_tabela == NULL)
						comeco_tabela = ultimo_fila = add_tab_simbolos(rotulo, false, true, 1);
					else {
						ultimo_fila->prox = add_tab_simbolos(rotulo, false, true, 1);
						ultimo_fila = ultimo_fila->prox;
					}
				}
				/*Rótulo redefinido*/
				else
					relata_erros(215, opr1, NULL);
			}
		}

		/*Ou já está na seção data*/
		else if (section == 2) {

			/*Nenhuma instrução pode vir aqui dentro*/
			if (*instr != '\0')
				relata_erros(213, dir, NULL);

			if (!strcmp(dir, "const")) {
				if (*rotulo != '\0') {
					if ((aux=busca_tabela(comeco_tabela,rotulo)) == NULL) {
						if (comeco_tabela == NULL)
							comeco_tabela = ultimo_fila = add_tab_simbolos(rotulo, true, false, 1);
						else {
							ultimo_fila->prox = add_tab_simbolos(rotulo, true, false, 1);
							ultimo_fila = ultimo_fila->prox;
						}
					}
					/*Rótulo redefinido*/
					else
						relata_erros(215, opr1, NULL);
				}
			}

			else if (!strcmp(dir, "space")){
				if (*rotulo != '\0') {
					if ((aux=busca_tabela(comeco_tabela,rotulo)) == NULL) {
						if (comeco_tabela == NULL)
							comeco_tabela = ultimo_fila = add_tab_simbolos(rotulo, false, false, mod1);
						else {
							ultimo_fila->prox = add_tab_simbolos(rotulo, false, false, mod1);
							ultimo_fila = ultimo_fila->prox;
						}
					}
					/*Rótulo redefinido*/
					else
						relata_erros(215, opr1, NULL);
				}
			}
		}

		/*Tem alguma coisa válida (não pre-processada) que póde vir fora das seções data e text? não consigo pensar em nada */
		else {
			if (*instr == '\0')
				relata_erros(213, instr, NULL);
			else
				relata_erros(214, dir, NULL);
		}
	}

	/*PREPARA PARA A SEGUNDA PASSAGEM*/
	line_counter = 0;
	section = 0;
	rewind(entrada);
	passagem++;

	/*PROCURA POR PROBLEMAS DE ROTULOS NAO DEFINIDOS*/
	while (get_linha(entrada, linha) == true) {

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		/*Separa as instrucoes e vai contando os erros encontrados*/
		separa_linha(linha, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		/*Entrando na seção text*/
		if (!strcmp("section", dir) && !strcmp("text", opr1))
			section = 1;

		/*Ou já está na seção text*/
		else if (section == 1) {

			/*Sem erros de diretivas para identificar nessa passagem*/
			if (*dir != '\0')
				continue;

			/*Stop não tem argumentos para se preocupar*/
			else if (!strcmp("stop", instr))
				continue;

			/*Se não achar o argumento na tabela, deu ruim*/
			else if ((aux = busca_tabela(comeco_tabela, opr1)) == NULL){

				/*Pode ser que o usuário não tenha informado um argumento, aí o programa iria reclamar que o rotulo '' não foi definido...*/
				if (*opr1 != '\0')
					relata_erros(300, opr1, NULL);
			}

			/*Proibo pulo para rotulos na seção data*/
			else if ((!strcmp(instr, "jmp") || !strcmp(instr, "jmpp") || !strcmp(instr, "jmpn")) && !aux->executavel )
				relata_erros(301, aux->rotulo, NULL);

			/*Proibo acesso a dados fora do vetor*/
			else if ((mod1<0) || (mod1>=aux->tamanho)){

				/*Se for um rotulo da seção de dados eu trato como vetor*/
				if (!aux->executavel)
					relata_erros(303, aux->rotulo, NULL);

				/*Se for da seção de texto eu trato diferente*/
				else
					relata_erros(304, NULL, NULL);
			}

			/*Proibo alterar o valor de uma variável constante*/
			else if (!strcmp(instr, "store") && aux->constante)
				relata_erros(302, aux->rotulo, NULL);

			/*COPY tem que verificar o segundo argumento*/
			else if (!strcmp(instr, "copy")){

				if ((aux = busca_tabela(comeco_tabela, opr2)) == NULL){

					if (*opr2 != '\0')
						relata_erros(300, opr2, NULL);
				}

				/*Proibo acesso a dados fora do vetor*/
				else if ((mod1<0) || (mod1>=aux->tamanho))
					relata_erros(303, aux->rotulo, NULL);
			}
		}
	}

	if (tem_codigo == false)
		relata_erros (216, NULL, NULL);
}

/*Retorna falso caso não consiga copiar o arquivo e true caso consiga*/
bool copy_file (char *src, char *dst) {

	FILE *input, *output;
	char c;

	input = fopen(src, "r");
	output = fopen(dst, "w+");

	if (input == NULL || output == NULL){
		fclose (input);
		fclose (output);
		return false;
	}

	while ((c = fgetc(input)) != EOF )
		fputc (c, output);

	fclose (input);
	fclose (output);
	return true;
}

void pre_processamento(char *entrada){

    processa_equ(entrada);

    /*Saída eh PREPROCESSED_FILE*/
	/*processa_if*/

}

/**************************************************************/

/*Abre os arquivos complementando a terminação .asm caso o usuário não tenha informado na linha de comando*/
FILE* open_sourcefile (char *name) {

	FILE *temp;
	char buffer[tokensize];

	if (name[strlen(name)-4] == '.' && name[strlen(name)-3] == 'a' && name[strlen(name)-2] == 's' && name[strlen(name)-1] == 'm') {
		temp = fopen(name, "r");
		return temp;
	}
	else{
		strcpy(buffer, name);
		strcat(buffer, ".asm");
		temp = fopen(buffer, "r");
		return temp;
	}
}

/*Abre os arquivos complementando a terminação .o caso o usuário não tenha informado na linha de comando*/
FILE* open_exitfile (char *name) {

	FILE *temp;
	char buffer[tokensize];

	if (name[strlen(name)-2] == '.' && name[strlen(name)-1] == 'o') {
		temp = fopen(name, "w+");
		return temp;
	}
	else{
		strcpy(buffer, name);
		strcat(buffer, ".o");
		temp = fopen(buffer, "w+");
		return temp;
	}
}


/** Recebe um token que com certeza eh numero (decimal ou hex) e retorna o valor*/
int converte_int (char *token) {
	char *aux;
	int cont=0, mult=1;

	/*Vai até o último caractere*/
	for (aux = token; *aux != '\0'; aux++)
		;
	aux--;

	/*Se for hexadecimal*/
	if (*token == '0' && *(token+1) == 'x') {
		for (; aux != token+1; mult *= 16, aux--) {
			if (*aux >= '0' && *aux<='9')
				cont += (*aux - '0')*mult;
			else
				cont += (*aux + 10 - 'a')*mult;
		}
		return cont;
	}
	/*Se não for hexadecimal*/
	for (;aux != token-1; aux--, mult *=10)
		cont += (*aux - '0')*mult;
	return cont;
}

SYMBOL_TABLE* add_tab_simbolos (char *rotulo, bool eh_constante, bool eh_executavel, int tamanho) {
	SYMBOL_TABLE *novo;

	/*Alocando a memória*/
	novo = malloc (sizeof(SYMBOL_TABLE));
	novo->rotulo = malloc(strlen(rotulo)+1);

	strcpy(novo->rotulo, rotulo);

	novo->endereco = mem_counter;
	novo->constante = eh_constante;
	novo->tamanho = tamanho;
	novo->prox = NULL;
	novo->executavel = eh_executavel;
	return novo;
}

void apaga_tab_simbolos (SYMBOL_TABLE *prim) {
	SYMBOL_TABLE *aux;

	for (; prim != NULL; prim = aux){
		aux = prim->prox;
		free(prim->rotulo);
		free(prim);
	}
}

bool tem_tabela_simbolos (SYMBOL_TABLE *prim, char *alvo) {
	if (*alvo == '\0')
		return true;

	for (;prim != NULL; prim = prim->prox) {
		if (!strcmp(prim->rotulo, alvo))
			return true;
	}
	return false;
}

SYMBOL_TABLE* busca_tabela (SYMBOL_TABLE *prim, char *alvo) {

	if (*alvo == '\0')
		return NULL;

	for (;prim != NULL; prim = prim->prox) {
		if (!strcmp(prim->rotulo, alvo))
			return prim;
	}
	return NULL;
}

int tam_instr (char *instr, int mod1, char *dir){

	if(!strcmp(instr, "copy"))
		return 3;

	else if (!strcmp(instr, "stop") || !strcmp(dir, "const"))
		return 1;

	else if (!strcmp(dir, "space"))
		return mod1;

	/*Aqui são os casos de diretivas fora space, const*/
	else if (*instr != '\0')
		return 2;

	return 0;
}

void sintese (FILE *entrada, FILE *saida, SYMBOL_TABLE *comeco_fila ) {

	/*Buffer*/
	char linha[linesize];

	/*Partes da instrução*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	/*Tabelas*/
	SYMBOL_TABLE *aux;
	int help;

	for (; get_linha(entrada, linha) == true ; mem_counter += tam_instr(instr, mod1, dir)) {

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		/*Separa as instrucoes e nao precisa contar os erros, já contei eles uma vez*/
		separa_linha(linha, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		/*Entra na seção text*/
		if (section == 0 && !strcmp("section", dir) && !strcmp("text", opr1)){
			section = 1;
			continue;
		}

		/*Ou seção text */
		else if (section == 1) {

			/*Chegamos na secao data*/
			if (!strcmp(dir, "section") && !strcmp(opr1, "data")){
				section = 2;
				continue;
			}

			/*Diretiva não gera código*/
			if (*dir != '\0')
				continue;

			/*Primeira coisa: opcode da instrucao*/
			if (*instr != '\0') {
				help = retorna_opcode(instr);
				fprintf (saida, "%d ", help);
			}

			/*Se for stop, não tem argumentos*/
			if (!strcmp(instr, "stop"))
				continue;

			/*Qualquer outra opção precisa de pelo menos um argumento*/
			aux = busca_tabela(comeco_fila, opr1);
			fprintf (saida, "%d ", (aux->endereco)+mod1);

			if (!strcmp(instr, "copy")){
				aux = busca_tabela(comeco_fila, opr2);
				fprintf (saida, "%d ", aux->endereco+mod2);
			}
		}

		else if (section == 2) {

			if (!strcmp(dir, "space"))
				for (;mod1>0; mod1--)
					fprintf (saida, "0 ");
			else if (!strcmp(dir, "const"))
				fprintf (saida, "%d ", mod1);
		}
	}
}

int retorna_opcode (char *instr) {

	if (!strcmp(instr, "add"))
		return 1;
	else if (!strcmp(instr, "sub"))
		return 2;
	else if (!strcmp(instr, "mult"))
		return 3;
	else if (!strcmp(instr, "mul"))
		return 3;
	else if (!strcmp(instr, "div"))
		return 4;
	else if (!strcmp(instr, "jmp"))
		return 5;
	else if (!strcmp(instr, "jmpn"))
		return 6;
	else if (!strcmp(instr, "jmpp"))
		return 7;
	else if (!strcmp(instr, "jmpz"))
		return 8;
	else if (!strcmp(instr, "copy"))
		return 9;
	else if (!strcmp(instr, "load"))
		return 10;
	else if (!strcmp(instr, "store"))
		return 11;
	else if (!strcmp(instr, "input"))
		return 12;
	else if (!strcmp(instr, "output"))
		return 13;
	else if (!strcmp(instr, "stop"))
		return 14;
	return 0;
}


/*****************************************************/

/*Faz todo o log de erros.

#100+ são erros de linha de comando
#200+ são erros léxico/sintáticos de primeira passagem
#300+ são erros semânticos encontrados na segunda passagem

*/
void relata_erros (int codigo, char *aux, char *aux2) {

	/*Erros de primeira passagem não precisam ser relatados de novo na segunda passagem*/
	if (passagem > 1 && codigo<300)
		return;

	printf (" *** %d) ", line_counter);

	switch (codigo) {
		case 100:
			printf (" Falta um codigo fonte.\n");
			break;

		case 101:
			printf (" Argumentos demais.\n");
			break;

		case 102:
			printf (" Nao consegui abrir o arquivo de entrada.\n");
			break;

		case 200:
			printf ("(Lex) A linha eh muito grande.\n");
			break;

		case 201:
			printf ("(Lex) O token '%s' eh muito grande.\n",aux);
			break;

		case 202:
			printf ("(Sint) Argumentos demais para a instrucao '%s'.\n", aux);
			break;

		case 203:
			printf ("(Sint) Faltam argumentos para a instrucao '%s'.\n", aux);
			break;

		case 204:
			printf ("(Lex) O argumento '%s' invalido para a instrucao '%s'.\n", aux, aux2);
			break;

		case 205:
			printf ("(Sint/Lex) Falta a separacao dos operandos por virgula.\n");
			break;

		case 206:
			printf ("(Sint) Dois rotulos definidos na mesma linha.\n");
			break;

		case 207:
			printf ("(Lex) Rotulo '%s' invalido.\n", aux);
			break;

		case 208:
			printf ("(Sint/Lex) Falta o simbolo ':' depois do rotulo '%s'.\n", aux);
			break;

		case 209:
			printf ("(Sint) Faltam argumentos para a diretiva '%s'.\n", aux);
			break;

		case 210:
			printf ("(Sint) Argumentos em excesso para a diretiva '%s'.\n", aux);
			break;

		case 211:
			printf ("(Lex) So existem as secoes 'text' e 'data'.\n");
			break;

		case 212:
			printf ("(Lex/Sint) A diretiva '%s' soh aceita argumento numerico positivo.\n", aux);
			break;

		case 213:
			printf ("(Sem) A instrucao '%s' tem que vir na secao 'text'.\n", aux);
			break;

		case 214:
			printf ("(Sem) A diretiva '%s' tem que vir na secao 'data'.\n", aux);
			break;

		case 215:
			printf ("(Sem) O rotulo '%s' foi previamente definido.\n", aux);
			break;

		case 216:
			printf (" (Sem) O codigo precisa de uma secao 'text'.\n");

		case 300:
			printf ("(Sem) O rotulo '%s' nunca foi definido.\n", aux);
			break;

		case 301:
			printf ("(Sem) O rotulo '%s' nao pode ser um alvo de pulo.\n", aux);
			break;

		case 302:
			printf ("(Sem) Tentando alterar o valor da constante '%s'.\n", aux);
			break;

		case 303:
			printf ("(Sem) A instrucao '%s' tenta acessar um dado fora do vetor.\n", aux);
			break;

		case 304:
			printf ("(Sem) Labels dentro da secao texto nao podem ser usadas como vetores.\n");
			break;

		default:
			printf ("Erro desconhecido.\n");
	}
	error_count++;
}
