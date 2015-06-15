#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/*Definicões para tamanho dos arrays de leitura de dados*/
#define tokensize		101
#define numbersize		6

#define STOP_CODE		14
#define COPY_CODE		9

#define TEMPORARY_FILE		"temp.txt"

bool debug1 = false;

typedef struct DEFINITION_TABLE DEFINITION_TABLE_CELL;

struct DEFINITION_TABLE {

	char rotulo[tokensize];
	int end;
	struct DEFINITION_TABLE *prox;
};

/*GOTO cada seção do código*/
void goto_code (FILE *src);
void goto_deftable (FILE *src);
void goto_usetable (FILE *src);

/*Abre arquivos ignorando as terminações*/
FILE* open_sourcefile (char *filename);
FILE* open_targetfile (char *filename);

int code_size (FILE *src);
void copy_adjust_text (FILE *src, FILE *output, int fator_correcao);
void apaga_trailing_space (FILE *src);
bool precisa_ligar (FILE *src);
void copy_text (FILE *src, FILE *output);

void referencias_cruzadas (FILE *src1, FILE *src2, FILE *saida_temp, FILE *saida_def, DEFINITION_TABLE_CELL *head, int fator_corretivo);
DEFINITION_TABLE_CELL* monta_def_table (FILE *src1, FILE *src2, int fator_corretivo);

int main (int argc, char *argv[]) {

	FILE *input1, *input2, *saida, *output;
	DEFINITION_TABLE_CELL *aux, *head;
	int fator_corretivo;

	if (argc < 4) {
		printf ("Faltam argumentos para o programa.\n");
		return 0;
	}

	/*Abre os arquivos de entrada de dados, independente da terminaçao .o*/
	input1 = open_sourcefile(argv[1]);
	input2 = open_sourcefile(argv[2]);
	output = open_targetfile(argv[3]);
	saida = fopen(TEMPORARY_FILE, "w+");

	/*Se algum arquivo não abrir, temos um problema*/
	if (input1 == NULL || input2 == NULL || output == NULL || saida == NULL){
		printf ("Não foi possivel aprir algum arguivo.\n");
		return 0;
	}

	/*Apaga possíveis espaços no fim do arquivo que me dão uma dor de cabeça enorme*/
	apaga_trailing_space(input1);
	apaga_trailing_space(input2);

	/*Calcula o fator corretivo para deslocar o segundo arquivo*/
	fator_corretivo = code_size(input1);

	if (debug1)
		printf ("Fator corretivo: %d\n", fator_corretivo);

	/*Se os arquivos forem independentes*/
	if (precisa_ligar(input1) && (precisa_ligar(input2))) {

		/*Monta a primeira parte da tabela de definições */
		head = monta_def_table(input1, input2, fator_corretivo);

		/*Comentário para acompanhar a tabela de definições 1*/
		if (debug1) {
			printf ("\nTABELA DE DEFINICOES\n");
			for (aux = head; aux != NULL; aux = aux->prox)
				printf ("ROTULO: '%s'\t'END: '%d'\n", aux->rotulo, aux->end);
		}

		/*COPIAR O CODIGO 1*/
		goto_code(input1);
		copy_text(input1, saida);

		/*COPIA COM FATOR DE CORREÇÃO O CÓDIGO 2*/
		goto_code(input2);
		copy_adjust_text (input2, saida, fator_corretivo);

		/*ESCREVE NO ARQUIVO TEMPORÁRIO, REMOVE O ' ' NO FIM QUE CAUSA PROBLEMAS*/
		fclose(saida);
		saida = fopen(TEMPORARY_FILE, "r+");
		apaga_trailing_space(saida);

		/*AGORA TENHO QUE AJUSTAR AS REFERÊNCIAS CRUZADAS*/
		referencias_cruzadas(input1, input2, saida, output, head, fator_corretivo);
	}

	apaga_trailing_space(output);
	fclose(input1);
	fclose(input2);
	fclose(saida);
	fclose(output);
	remove(TEMPORARY_FILE);

	return 0;
}

/*Abre os arquivos complementando a terminação .o caso o usuário não tenha informado na linha de comando*/
FILE* open_sourcefile (char *filename){

	FILE *temp;
	char buffer[tokensize];

	if (filename[strlen(filename)-1] == '.' && filename[strlen(filename)] == 'o'){
		temp = fopen(filename, "r+");
		return temp;
	}
	else{
		strcpy(buffer, filename);
		strcat(buffer, ".o");
		temp = fopen(buffer, "r+");
		return temp;
	}
}

FILE* open_targetfile (char *filename){

	FILE *temp;
	char buffer[tokensize];

	if (filename[strlen(filename)-1] == '.' && filename[strlen(filename)] == 'e'){
		temp = fopen(filename, "w+");
		return temp;
	}
	else{
		strcpy(buffer, filename);
		strcat(buffer, ".e");
		temp = fopen(buffer, "w+");
		return temp;
	}
}

/*Retorna true(1) or false(0) se o primeiro caractere eh T (de table definition, table use)*/
bool precisa_ligar (FILE *src) {
	char c = fgetc(src);
	rewind(src);
	return c == 'T';
}

/*Copia de onde o arquivo fonte começar a apontar até o fim dele para aonde output apontar*/
void copy_text (FILE *src, FILE *output) {
	int c;
	while (!feof(src)){
		fscanf(src, "%d", &c);
		fprintf (output, "%d ", c);
	}
}

/*Copia de onde o arquivo fonte começar a apontar até o fim dele para aonde output apontar*/
void copy_adjust_text (FILE *src, FILE *output, int fator_correcao) {
	int c;
	while (!feof(src)){
		fscanf(src, "%d", &c);
		fprintf (output, "%d ", c);

		/*Ao chegar no stop, copia toda a seção de dados igualzinha*/
		if (c == STOP_CODE) {
			copy_text (src, output);
			continue;
		}

		/*Se for um copy, pega um argumento extra*/
		if (c == COPY_CODE) {
			fscanf(src, "%d", &c);
			fprintf (output, "%d ", c+fator_correcao);
		}

		/*Se for qualquer instrução != STOP peque 1 argumento (copy está pegando o segundo)*/
		fscanf(src, "%d", &c);
		fprintf (output, "%d ", c+fator_correcao);
	}
}

/*Espaços no fim do programa podem dar um bug de ele ler ' ' e me retornar um zero*/
void apaga_trailing_space (FILE *src) {

	fseek(src, -1, SEEK_END);
	if (fgetc(src) == ' ') {
		fseek(src, -1, SEEK_END);
		ftruncate(fileno(src), ftell(src));
	}
	rewind(src);
}

/*Dado um arquivo fonte, ele vai do começo, passando batido por tudo ate encontrar a palavra 'CODE'*/
void goto_code (FILE *src) {
	char buffer[tokensize+1+numbersize];

	rewind(src);
	do	fscanf(src, "%s", buffer);
	while (strcmp(buffer, "CODE"));

	buffer[0] = fgetc (src);	/*vai ter um \n*/
}

/*Dado um arquivo fonte, ele passa batido até encontrar a palavra 'DEFINITION'*/
void goto_deftable (FILE *src) {
	char buffer[tokensize+1+numbersize];

	rewind(src);
	do	fscanf(src, "%s", buffer);
	while (strcmp(buffer, "DEFINITION"));

	buffer[0] = fgetc (src);	/*vai ter um \n*/
}

/*Dado um arquivo fonte, ele passa batido for tudo até encontrar a palavra USE*/
void goto_usetable (FILE *src) {
	char buffer[tokensize+1+numbersize];

	rewind(src);
	do	fscanf(src, "%s", buffer);
	while (strcmp(buffer, "USE"));

	buffer[0] = fgetc (src);	/*vai ter um \n*/
}


/*Dado um arquivo com apenas numeros separados por um espaço, ele conta a quantidade de numeros*/
int code_size (FILE *src) {
	int num, quant;
	for (goto_code(src), quant = 0; !feof(src); quant++)
		fscanf(src, "%d", &num);
	rewind(src);
	return quant;
}

/*Dado um codigo de output já feito com as referências por ajustar*/
void referencias_cruzadas (FILE *src1, FILE *src2, FILE *saida_temp, FILE *saida_def, DEFINITION_TABLE_CELL *head, int fator_corretivo) {

	char buffer[tokensize];
	int mem_count=0, n, temp1;
	DEFINITION_TABLE_CELL *aux;

	/*Acertando as posições dos arquivos*/
	goto_usetable(src1);
	goto_usetable(src2);
	rewind(saida_temp);

	/*RESOLVENDO AS REFERÊNCIAS CRUZADAS DO PRIMEIRO ARQUIVO*/
	for (buffer[0] = '\0'; fscanf(src1, "%s %d", buffer, &n);){

		/*Ao acabarem as referencias cruzadas do arquivo 1 saia dessa estrutura*/
		if(!strcmp(buffer, "TABLE"))
			break;

		/*Busca na tabela de definições*/
		for (aux = head; aux != NULL; aux = aux->prox)
			if (!strcmp(aux->rotulo, buffer))
				break;

		if (debug1){
			printf ("\nQuero resolver a referencia cruzada na posicao %d.\n", n);
			printf ("Quero adicionar: '%d', pois eh '%s'.", aux->end, aux->rotulo);
			getchar();
		}

		/*Enquanto nao chegar na primeira referencia cruzada, vai copiando igualzinho*/
		while (mem_count++ < n){
			fscanf(saida_temp, "%d", &temp1);
			fprintf(saida_def, "%d ", temp1);
		}

		/*Cheguei na referência cruzada*/
		fscanf(saida_temp, "%d", &temp1);
		fprintf(saida_def, "%d ", temp1+aux->end);

		if (debug1){
			printf ("Li do arquivo temporario: '%d' e vou alterar para '%d'\n", temp1, temp1+aux->end);
			getchar();
		}
	}

	/*Continua escrevendo até o fim do primeiro arquivo*/
	while (mem_count++ < fator_corretivo){
		fscanf(saida_temp, "%d", &temp1);
		fprintf(saida_def, "%d ", temp1);
	}

	/*RESOLVENDO AS REFERÊNCIAS CRUZADAS DO SEGUNDO ARQUIVO*/
	for (buffer[0] = '\0', mem_count--; fscanf(src2, "%s %d", buffer, &n);) {

		if(!strcmp(buffer, "TABLE")){
			break;
		}

		/*Busca na tabela de definições*/
		for (aux = head; aux != NULL; aux = aux->prox)
			if (!strcmp(aux->rotulo, buffer))
				break;

		if (debug1){
			printf ("\n\nQuero resolver a referencia cruzada na posicao %d.\n", n+fator_corretivo);
			printf ("Quero adicionar: '%d', pois eh '%s'.\n", aux->end, aux->rotulo);
			getchar();
		}

		/*Enquanto nao chegar na primeira referencia cruzada, vai copiando igualzinho*/
		while (mem_count++ < n+fator_corretivo){
			fscanf(saida_temp, "%d", &temp1);
			fprintf(saida_def, "%d ", temp1);
		}

		/*Cheguei na referência cruzada*/
		fscanf(saida_temp, "%d", &temp1);
		fprintf(saida_def, "%d ", temp1 + aux->end);
	}

	/*Agora que resolvemos todos as referências cruzadas, continua copiando as informações*/
	while (!feof(saida_temp)) {
		fscanf(saida_temp, "%d", &temp1);
		fprintf(saida_def, "%d ", temp1);
	}
}

/*Monta a tabela de definições de um arquivo fonte, em qualquer ponto dele ele vai procurar a palavra DEFINITION e montar a tabela
com as linhas que seguem até encontrar uma linha vazia */
DEFINITION_TABLE_CELL* monta_def_table (FILE *src1, FILE *src2, int fator_corretivo) {

	DEFINITION_TABLE_CELL *new_cell, *head, *current;
	char buffer[tokensize];

	/*Vá até o ponto certinho da tabela de definições*/
	rewind(src1);
	goto_deftable(src1);

	/*O primeiro eu faço separado por que ele é diferente dos outros (cabeça da fila)*/
	head = current = new_cell = malloc(sizeof(DEFINITION_TABLE_CELL));

	fscanf(src1, "%s", new_cell->rotulo);
	fscanf(src1, "%d", &(new_cell->end));
	new_cell-> prox = NULL;

	fscanf(src1, "%s", buffer);
	while (strcmp(buffer, "CODE")) {

		/*Fazendo um novo elemento e colocando as informações*/
		new_cell = malloc(sizeof(DEFINITION_TABLE_CELL));
		strcpy(new_cell->rotulo, buffer);
		fscanf(src1, "%d", &(new_cell->end));

		/*Fazendo as ligações da lista*/
		current->prox = new_cell;
		new_cell->prox = NULL;

		/*Atualizando o atual*/
		current = new_cell;

		/*Pegando o proximo termo, se for linha vazia vambora*/
		fscanf(src1, "%s", buffer);
	}

	/*AGORA VOU ADICIONAR AS INFORMAÇES DO SEGUNO ARQUIVO PARA A TABELA DE DEFINIÇÕES*/

	/*Vá até o ponto certinho da tabela de definições*/
	rewind(src2);
	goto_deftable(src2);

	/*Agora vai pegando as informacoes*/
	fscanf(src2, "%s", buffer);
	while (strcmp(buffer, "CODE")) {

		/*Fazendo um novo elemento e colocando as informações*/
		new_cell = malloc(sizeof(DEFINITION_TABLE_CELL));
		strcpy(new_cell->rotulo, buffer);
		fscanf(src2, "%d", &(new_cell->end));
		new_cell->end += fator_corretivo;

		/*Fazendo as ligações da lista*/
		current->prox = new_cell;
		new_cell->prox = NULL;

		/*Atualizando o atual*/
		current = new_cell;

		/*Pegando o proximo termo, se for linha vazia vambora*/
		fscanf(src2, "%s", buffer);
	}

	return head;
}
