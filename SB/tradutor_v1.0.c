/*Bibliotecas*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*Definicões para tamanho dos arrays de leitura de dados*/
#define tokensize		101
#define linesize		312		/* tkn + " : COPY " + tkn + " , " + tkn + '\0'*/

/*Caso o usuário não informe o nome que ele quer para o arquivo de saida*/
#define DEFAULT_OUTPUT_NAME		"default.o"

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

/**************************************** VARIÁVEIS GLOBAIS: ESTADO DE EXECUÇÃO *******************************************/
int error_count = 0;
int passagem = 1;
int line_counter = 0;
int mem_counter = 0;
int section = 0;
int prog_size = 0;
bool eh_modulo = false;
bool auto_suficiente = true;

bool debug_mode1 = false;

/*************************************************** DEFINIÇÕES FUNÇÕES ***************************************************/

/*Grandes módulos que fazem o grosso do trabalho do programa*/
bool valida_linhacomando (int argc);
bool abre_arquivos (int argc, char *argv[], FILE *entrada, FILE *saida);
void analise (FILE *entrada, SYMBOL_TABLE *comeco_tabela);
void sintese (FILE *entrada, FILE *saida, SYMBOL_TABLE *comeco_fila);
void relata_erros (int codigo, char *aux, char *aux2);

/*Abre os arquivos independente da terminação*/
FILE* open_sourcefile (char *name);
FILE* open_exitfile (char *name);

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
int tam_instr (char *instr, int mod1, char *dir);
int retorna_opcode (char *instr);

/********************************************************* MAIN ***********************************************************/

int main (int argc,char *argv[]) {

	FILE *entrada=NULL, *saida=NULL;
	SYMBOL_TABLE *comeco_tabela=NULL;

	/** Para o programa final a gente usa essas funções, para testar direto no codeblocks eh soh usar as linhas abaixo
	if (!valida_linhacomando(argc))
		return 0;

	if (!abre_arquivos(argc, argv, entrada, saida));
		return 0;
	*/

	/*entrada = fopen("ArquivosTeste/dir.asm", "r");*/
	entrada = fopen("ArquivosTeste/instr.asm", "r");

	analise(entrada, comeco_tabela);

	return 0;

	rewind(entrada);
	mem_counter = 0;
	passagem++;
	section = 0;

	/*sintese(entrada, saida, comeco_tabela);*/

	return 0;

}

/*Retorna true se o numero de argumentos estiver certo e false senão*/
bool valida_linhacomando (int argc) {

	/*Se for 1 argumento, nem tem o que fazer*/
	if (argc<2){
		relata_erros(100, NULL, NULL);
		return false;
	}
	return true;
}

/*Retorna true se ambos os arquivos foram abertos com sucesso e falso caso contrário */
bool abre_arquivos (int argc, char *argv[], FILE *entrada, FILE *saida) {

	entrada = open_sourcefile(argv[1]);
	saida = (argc == 2) ? open_exitfile(DEFAULT_OUTPUT_NAME) : open_exitfile(argv[2]);

	if (entrada == NULL)
		relata_erros(101, NULL, NULL);

	if (saida == NULL)
		relata_erros(102, NULL, NULL);

	return (saida != NULL && entrada != NULL)? true : false;
}

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
		if (!strcmp("section", dir) && !strcmp("text", opr1))
			section = 1;

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
					relata_erros(216, opr1, NULL);
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
						relata_erros(216, opr1, NULL);
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
						relata_erros(216, opr1, NULL);
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
	prog_size = mem_counter;
	mem_counter = line_counter = 0;
	section = 0;
	rewind(entrada);
	passagem++;

	/*PROCURA POR PROBLEMAS DE ROTULOS NAO DEFINIDOS*/
	for (; get_linha(entrada, linha) == true ; mem_counter += tam_instr(instr, mod1, dir)) {

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

			/*Se não achar o argumento na tabela, deu ruim*/
			if ((aux = busca_tabela(comeco_tabela, opr1)) == NULL)
				relata_erros(300, opr1, NULL);

			/*Proibo pulo para rotulos na seção data*/
			else if ((!strcmp(instr, "jmp") || !strcmp(instr, "jmpp") || !strcmp(instr, "jmpn")) && !aux->executavel )
				relata_erros(302, aux->rotulo, NULL);

			/*Proibo acesso a dados fora do vetor*/
			else if ((mod1<0) || (mod1>=aux->tamanho))
				relata_erros(304, aux->rotulo, NULL);

			/*Proibo alterar o valor de uma variável constante*/
			else if (!strcmp(instr, "store") && aux->constante)
				relata_erros(303, aux->rotulo, NULL);

			/*COPY tem que verificar o segundo argumento*/
			else if (!strcmp(instr, "copy")){
				aux = busca_tabela(comeco_tabela, opr2);
				if (aux == NULL)
					relata_erros(300, opr2, NULL);

				/*Proibo acesso a dados fora do vetor*/
				else if ((mod1<0) || (mod1>=aux->tamanho))
					relata_erros(304, aux->rotulo, NULL);
			}
		}
	}
}

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

/** Lê uma linha do arquivo e deixa no buffer a linha lida.

	* Já corrige o line_counter
    * Ignora linha só com whitespace ou comentários
    * Ignora whitespace depois de um whitespace
    * Acaba a linha com \0
    * Transforma maiúscula em minúscula

Retorna true se o arquivo-fonte ainda não acabou
Retorna false se o arquivo-fonte acabar
*/
bool get_linha (FILE *src, char *buffer){
    int c;
    int i=0;
    bool in_whitespace = false;

    /*Ignora o whitespace e comentários até um caractere válido*/
    for ( c=fgetc(src) ; c==' ' || c=='\t' || c=='\n' || c==';' || c=='\r' ; c=fgetc(src) ) {

        /*Fim de linha tem que aumentar o contador de linha*/
        if (c == '\n') {
            line_counter++;
			continue;
		}

        /*Em caso de comentário, vá até o fim dele*/
        if (c == ';') {
			ignora_linha (src);
			line_counter++;
			continue;
        }
    }

	/*Se acabou o arquivo e nada de válido foi encontrado retorna falso*/
    if (feof(src))
        return false;

    /*Se cheguei aqui tem algo de valor na linha. Vai copiando a linha até ela ou o arquivo acabar.*/
    for (line_counter++; c !='\n' && !feof(src); c = fgetc(src) ) {

		/*Se já estivermos em whitespace, nao precisa adicionar mais whitespace*/
        if (c == ' ' || c == '\t' || c == '\r') {
			if (in_whitespace == false ){
				in_whitespace = true;
				buffer[i++] = ' ';
			}
			continue;
		}
		in_whitespace = false;

		/*Se for comentário ignore o resto da linha*/
        if (c == ';') {
            ignora_linha(src);
			break;
        }

		/*Copiando o token e passando de maiúscula para minuscula*/
        if (c >= 'A' && c<= 'Z')
			buffer[i++] = c + 32;
		else
			buffer[i++] = c;

        /*reporta erro de linha muito grande*/
        if (i >= linesize) {
            relata_erros(200, NULL, NULL);
            ignora_linha(src);
            i--;
            break;
        }
    }
    buffer[i] = '\0';

	/*Apagando um possível espaço desnecessário no fim da linha*/
    if (buffer[i-1]==' ')
		buffer[i-1] ='\0';
	return true;
}

/** Dada uma linha inteira, ele separa o primeiro token e coloca no 'token' e deixa o resto da linha em 'linha'

Retorna true se a linha não recebeu uma linha vazia
Retorna false se recebeu uma linha vazia
*/
bool get_token (char *linha, char *token) {
    int i=1;

	/*Caso de linha vazia*/
	if (linha[0] == '\0')
		return false;

	/*Caso de um token finalizador no primeiro espaco*/
	if (fim_token(linha[0])) {
		*token = *linha;
		*(token+1) = '\0';

		if (linha[1] == ' ')
			strcpy(linha, &linha[i+1]);
		else
			strcpy(linha, &linha[i]);

		return true;
	}

	/*Caso de um token nao finalizador no primeiro espaco*/
    for (*token = *linha; !fim_token(linha[i]); i++) {
		if (i < tokensize)									/*Enquanto a linha estiver pequena vai copiando*/
			token[i] = linha[i];

		/*caso o token seja muito grande*/
		else {
			token[tokensize -1] = '\0';
			relata_erros(201, token, NULL);
			break;
		}
    }
	token[i] = '\0';

	if (linha[i] == ' ') {
		strcpy(linha, &linha[i+1]);
		return true;
	}
	strcpy(linha, &linha[i]);
	return true;
}

/** Lê o arquivo até o fim da linha ou fim do arquivo e retorna*/
void ignora_linha (FILE *src) {
	while (fgetc(src) != '\n' && !feof(src))
		;
	return;
}

/** True se o caractere está na lista dos finalizadores de token false se nao*/
bool fim_token (char c) {
	char finalizadores[] = {'\0', ' ', '\t', '.', ',', '(' , ')' , ':', '+'};
	int i;
	for (i = sizeof(finalizadores)-1; i>=0 ; i--)
		if (c == finalizadores[i])
			return true;
	return false;
}

bool separa_linha (char *buffer, char *rotulo, char *instr, char *dir, char *opr1, int *mod1, char *opr2, int *mod2) {
	char aux[tokensize];

	/*Se não tiver nada pode retornar 1 que nao tem erros*/
	if (!get_token(buffer, aux))
		return true;

	/*Opção 1: eh instr. STOP (0), COPY (2) E O RESTO (1)*/
	if (eh_instr(aux)) {

		/*Copia o aux para o campo instr*/
		strcpy(instr, aux);

		/*stop nao tem argumentos*/
		if (!strcmp(instr, "stop")) {

			/*Se tiver mais alguma coisa na linha eh erro de argumentos demais!*/
			if (get_token(buffer, aux)) {
				relata_erros(202, instr, NULL);
				return false;
			}
			return true;
		}

		/*Todos as outras operações tem que ter pelo menos um operando*/
		else if (!get_token(buffer, opr1)) {
			relata_erros(203, instr, NULL);
			return false;
		}

		/*Se o argumento nao for rotulo válido, erro léxico*/
		else if (!rotulo_valido(opr1)){
			relata_erros(204, opr1, instr);
			*opr1 = '\0';
		}

		/*Chegou aqui então eh uma instrução (diferente de stop) e o primeiro argumento eh válido*/

		/*Se tiver um modificador, anoto. Se não tiver, tá de boa tb*/
		if (eh_modificador_valido(buffer)) {
			get_token(buffer, aux);				/*pega o '+' ou '-'*/

			if (*aux == '-' )
				relata_erros(212, instr, NULL);
			else {
				get_token(buffer, aux);				/*pega o numero*/
				*mod1 = converte_int(aux);
			}
		}

		/*Se for copy, tem que pegar mais um operando!*/
		if (!strcmp(instr, "copy")) {

			/*Tem que ter dois tokens para puxar, um pra ser a vírgula e outro o opr2*/
			if (!get_token(buffer, aux)) {
				relata_erros(203, "copy", NULL);
				return false;
			}

			/*Se não for a vírgula, reclame e peça a vírgula*/
			else if (*aux != ',') {
				relata_erros(205, NULL, NULL);

				if (get_token(buffer, aux))
					relata_erros(202, instr, NULL);

				return false;
			}

			/*Se faltar o segundo argumento, tá errado*/
			else if(!get_token(buffer, opr2)) {
				relata_erros(203, "copy", NULL);
				return false;
			}

			/*Se nao for rotulo válido, erro léxico*/
			else if (!rotulo_valido(opr2)){
				relata_erros(204, opr2, instr);
				*opr2 = '\0';
				return false;
			}

			/*Se tiver um modificador a seguir, pode puxar*/
			else if (eh_modificador_valido(buffer)) {
				get_token(buffer, aux);

				if (*aux == '-')
					relata_erros (212, instr, NULL);
				else {
					get_token(buffer, aux);
					*mod2 = -1*converte_int(aux);
				}
			}
		}

		/*Se tiver mais alguma coisa para buscar na linha, eh erro!*/
		if (get_token(buffer, aux)) {
			relata_erros (202, instr, NULL);
			return false;
		}

		return true;
	}

/* section (1), public (1), begin(0), end(0), extern(0), space(mod), const(mod) */
	else if (eh_diretiva(aux)) {

		/*OK, descobri a diretiva. Agora tem que separar os possíveis argumentos*/
		strcpy(dir, aux);

		if (!strcmp(dir, "section")) {

			/*Tenta pegar o operador, se não tiver nada na linha eh erro de falta de argumentos*/
			if (!get_token(buffer, opr1)){
				relata_erros(209, dir, NULL);
				return false;
			}

			/*Se não for text ou data, a seção não é reconhecida pelas regras do roteiro*/
			else if (strcmp(opr1, "text") && strcmp(opr1, "data")) {
				relata_erros(211, dir, NULL);
				return false;
			}
		}

		else if (!strcmp(dir, "space")) {

			/*Se tiver um numero a seguir, pode puxar.*/
			if (get_token(buffer, aux)) {

				if (eh_num(aux)) {
					*mod1 = converte_int(aux);

					/*Se o valor do modificador for negativo, tenho que parar o programador*/
					if (*mod1 <= 0) {
						*mod1 = 1;
						relata_erros(212, dir, NULL);
						return false;
					}
				}
				else {
					relata_erros(212, dir, NULL);
					return false;
				}
			}
			else {
				/*Se não tiver, pode voltar que tá de boa */
				*mod1 = 1;
				return true;
			}
		}

		else if (!strcmp(dir, "const")) {

			/*Precisa de um 'argumento'*/
			if (!get_token(buffer, aux)) {
				relata_erros(209, dir, NULL);
				return false;
			}

			/*Se o argumento no for numérico deu ruim*/
			if (eh_num(aux))
				*mod1 = converte_int(aux);
			else {
				relata_erros(209, dir, NULL);
				return false;
			}
		}

		/*Se tiver mais alguma coisa na linha, tem que identificar como erro*/
		if (get_token(buffer, aux)) {
			relata_erros(210, dir, NULL);
			return false;
		}

		return true;
	}

	/*Se não for instrução ou diretiva, só pode ser rotulo.
	Vou fazer de forma recursiva:
		nao tem rotulo na linha: separa o rotulo e chama a função de novo (com o rotulo marcado)
		tem rotulo: rotulo foi definido em uma chamada anterior e estamos fazendo de novo!

	O objetivo eh identificar a situação 'rotulo1: rotulo2: instr ...' */

	/*Tentando definir um segundo rótulo em uma linha!*/
	if (*rotulo != '\0') {
		relata_erros(206, NULL, NULL);
		return false + 0*separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);;
	}

	/*Se o rotulo não for válido, erro léxico!*/
	else if (!rotulo_valido(aux)){
		relata_erros(207, aux, NULL);
		strcpy(rotulo, aux);
		return false + 0*separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);;
	}

	/*Não aceite modificadores para os rótulos*/
	else if (eh_modificador_valido(buffer)) {
		get_token(buffer, opr1);
		get_token(buffer, opr2);

		strcat(aux, opr1);
		strcat(aux, opr2);
		strcpy(rotulo, aux);
		*opr1 = *opr2 = '\0';

		relata_erros(207, aux, NULL);

		if (*buffer != ':')
			relata_erros(208, aux, NULL);
		else
			get_token(buffer, aux);

		return 0*separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);;
	}

	/*Erro de definição do rótulo! Faltou o ':'*/
	else if (*buffer != ':') {
		relata_erros(208, aux, NULL);
		strcpy(rotulo, aux);
		return 0*separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);;
	}

	/*Separo o rotulo do resto linha e chamo a função novamente*/
	strcpy(rotulo, aux);
	get_token(buffer, aux);
	return separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);
}

/** Retorna true se o token recebido for uma das instruções definidas no roteiro*/
bool eh_instr (char *token) {

	return !strcmp(token,"add")
	|| !strcmp(token,"sub")
	|| !strcmp(token,"mult")
	|| !strcmp(token,"div")
	|| !strcmp(token,"jmp")
	|| !strcmp(token,"jmpn")
	|| !strcmp(token,"jmpp")
	|| !strcmp(token,"jmpz")
	|| !strcmp(token,"copy")
	|| !strcmp(token,"load")
	|| !strcmp(token,"store")
	|| !strcmp(token,"input")
	|| !strcmp(token,"output")
	|| !strcmp(token,"stop");
}

/** Retorna true se o token recebido for uma das diretivas definidas no roteiro*/
bool eh_diretiva (char *token) {

	return !strcmp(token,"section")
	|| !strcmp(token,"space")
	|| !strcmp(token,"const");
}

/** Retorna true se o token recebido for um numero normal ou hexadecimal*/
bool eh_num (char *token) {

	char *aux;

	if (*token == '\0')
		return false;

	/*Se for hexadecimal*/
	if (*token == '0' && *(token+1) == 'x')
		for (aux = token+2; *aux != '\0'; aux++) {
			if ((*aux >= '0'&&*aux<='9') || (*aux>='a'&&*aux<='e') )
				continue;
			return false;
		}

	/*Se não for hexadecimal*/
	else
		for (aux = token; *aux != '\0'; aux++) {
			if (*aux >= '0'&&*aux<='9')
				continue;
			return false;
		}

	return true;
}

/** Retorna true se o token segue as regras de escrita da linguagem e false se nao*/
bool rotulo_valido (char *token) {

	/*Nao pode comecar com numero*/
	if (*token >= '0' && *token <= '9')
		return false;

	/*Tem que ser numero, '_' ou letra minuscula pois troquei as maiusculas por minusculas*/
	for (; *token != '\0' ;token++) {
		if ( (*token >='0' && *token<='9') || (*token=='_') || (*token>='a' && *token<='z') )
			continue;
		return false;
	}
	return true;
}

/** Verifico se no buffer os tokens às seguir são operação e numero */
bool eh_modificador_valido (char *buffer) {
	char buffer_local[linesize], num[tokensize], op[tokensize];
	strcpy(buffer_local, buffer);

	return get_token(buffer_local, op) && *op=='+' && get_token(buffer_local, num) && eh_num(num);
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

void pre_processamento (FILE *entrada) {

	/*Buffer*/
	char linha[linesize];

	/*Partes da instrução*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	for (; get_linha(entrada, linha) == true ; mem_counter += tam_instr(instr, mod1, dir)) {

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		/*Separa as instrucoes e nao precisa contar os erros, já contei eles uma vez*/
		separa_linha(linha, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

	}
}

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
			printf (" Nao consegui abrir o arquivo de entrada.\n");
			break;

		case 102:
			printf (" Nao consegui abrir o arquivo de saida.\n");
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
			printf ("(Lex) O argumento '%s' inválido para a instrucao '%s'.\n", aux, aux2);
			break;

		case 205:
			printf ("(Sint/Lex) Falta a separacao dos operandos por virgula.\n");
			break;

		case 206:
			printf ("(Sint) Dois rotulos definidos na mesma linha.\n");
			break;

		case 207:
			printf ("(Lex) Rotulo '%s' inválido.\n", aux);
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
			printf ("(Sint) A diretiva '%s' necessita de um rotulo.\n", aux);
			break;

		case 216:
			printf ("(Sem) O rotulo '%s' foi previamente definido.\n", aux);
			break;

		case 300:
			printf ("(Sem) O rotulo '%s' nunca foi definido.\n", aux);
			break;

		case 301:
			printf ("(Sem) Divisao por zero.\n");
			break;

		case 302:
			printf ("(Sem) O rotulo '%s' nao pode ser um alvo de pulo.\n", aux);
			break;

		case 303:
			printf ("(Sem) Tentando alterar o valor da constante '%s'.\n", aux);
			break;

		case 304:
			printf ("(Sem) A instrucao '%s' tenta acessar um dado fora do vetor.\n", aux);
			break;

		case 306:
			printf ("(Sem) O rotulo '%s' nao foi definido.\n", aux);
			break;

		default:
			printf ("Erro desconhecido.\n");
	}
	error_count++;
}
