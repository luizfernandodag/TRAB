
/*Definicões para tamanho dos arrays de leitura de dados*/
#define tokensize		101
#define linesize		312		/* tkn + " : COPY " + tkn + " , " + tkn + '\0'*/

/*Caso o usuário não informe o nome que ele quer para o arquivo de saida*/
#define DEFAULT_OUTPUT_NAME		"a.out"
#define SECOND_OUTPUT_NAME		"affe.out"

/*Bibliotecas*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*************************************** DEFINIÇÕES TABELAS E ESTRUTURAS DE DADOS *****************************************/

typedef struct SYMBOL_TABLE_CELL SYMBOL_TABLE;

struct SYMBOL_TABLE_CELL {

	struct SYMBOL_TABLE_CELL *prox;
	char *rotulo;
	int endereco;
	int tamanho;		/*so eh relevante para o space*/
	int valor;
	bool externo;
	bool publico;
	bool constante;
	bool in_text;
};

/**************************************** VARIÁVEIS GLOBAIS: ESTADO DE EXECUÇÃO *******************************************/
int line_counter = 0;
int mem_counter = 0;
int error_count = 0;
int section = 0;
int prog_size = 0;
int passagem = 1;
bool tem_stop = 0;
bool eh_modulo = false;
bool encontrei_begin = false;
bool auto_suficiente = true;

bool debug_mode1 = false;

/**************************************** VARIÁVEIS GLOBAIS: PREGUIÇA DE PASSAR ARGUMENTOS *********************************/
SYMBOL_TABLE *first_symbol;
FILE *src, *output;

/*************************************************** DEFINIÇÕES FUNÇÕES ***************************************************/

/*Arquivo fonte -> linha -> token*/
bool get_linha (char *buffer);
void ignora_linha ( );
bool get_token (char *linha, char *token);
bool fim_token (char c);

/*Classificação token*/
bool eh_diretiva (char *token);
bool eh_instr (char *token);
bool eh_num (char *token);
bool rotulo_valido (char *src);
bool eh_modificador_valido (char *buffer);

/*Módulos grandes*/
void analise ();
void sintese ();
void write_tables_output ( );

/*Modulos internos aos modulos grandes*/
bool separa_linha (char *linha, char *rotulo, char *instr, char *dir, char *opr1, int* mod1, char *opr2, int* mod2);
int tam_instr (char *instr, int mod1, char *dir);

/*Utilização da tabela de símbolos*/
SYMBOL_TABLE* add_tab_simbolos (char *rotulo, bool eh_externo, bool eh_publico, bool eh_constante, bool eh_in_text, int valor, int tamanho);
void apaga_tab_simbolos (SYMBOL_TABLE *prim);
bool tem_tabela_simbolos (SYMBOL_TABLE *prim, char *alvo);
SYMBOL_TABLE* busca_tabela (SYMBOL_TABLE *prim, char *alvo);

/*Sintese de codigo*/
int retorna_opcode (char *instr);

/*geral*/
void relata_erros (int codigo, char *aux, char *aux2);
int converte_int (char *token);
void str_caseup(char *in_str, char *out_str);
FILE* open_sourcefile (char *filename);
FILE* open_exitfile (char *name);

/********************************************************* MAIN ***********************************************************/
int main (int argc, char *argv[]) {

	if (argc==1){
		relata_erros(90, NULL, NULL);
		return 0;
	}
	else {
		src = open_sourcefile (argv[1]);
		if (src == NULL) {
			relata_erros(91, NULL, NULL);
			return 0;
		}
	}

	analise(src);

	if (error_count == 0) {

		/*Se o usuário tiver informado apenas 1 argumento para o programa (codigo fonte) o objeto sera a.out*/
		if (argc == 2) {
			output = fopen(DEFAULT_OUTPUT_NAME, "w+");
			if (output == NULL)
				output = fopen(SECOND_OUTPUT_NAME, "w+");
		}
		else
			output = open_exitfile(argv[2]);

		/*Se não der pra abrir o arquivo de saída...*/
		if (output == NULL) {
				relata_erros(92, NULL, NULL);
				return 0;
		}

		if (auto_suficiente == false)
			write_tables_output ( );

		rewind(src);
		mem_counter = 0;
		passagem++;
		section = 0;

		sintese();

		fclose(output);
	}

	fclose(src);
    return 0;
}

/******************************************************** FUNÇÕES *********************************************************/

/** Lê uma linha do arquivo e deixa no buffer a linha lida.

	* Já corrige o line_counter
    * Ignora linha só com whitespace ou comentários
    * Ignora whitespace depois de um whitespace
    * Acaba a linha com \0
    * Transforma maiúscula em minúscula

Retorna true se o arquivo-fonte ainda não acabou
Retorna false se o arquivo-fonte acabar
*/
bool get_linha (char *buffer){
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
            relata_erros(100, NULL, NULL);
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
		else {
			token[tokensize -1] = '\0';
			relata_erros(101, token, NULL);
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

/** True se o caractere está na lista dos finalizadores de token false se nao*/
bool fim_token (char c) {
	char finalizadores[] = {'\0', ' ', '\t', '.', ',', '(' , ')' , ':', '+', '-'};
	int i;
	for (i = sizeof(finalizadores)-1; i>=0 ; i--)
		if (c == finalizadores[i])
			return true;
	return false;
}

/** Lê o arquivo até o fim da linha ou fim do arquivo e retorna*/
void ignora_linha ( ) {
	while (fgetc(src) != '\n' && !feof(src))
		;
	return;
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

/** Retorna true se o token recebido for uma das instruções definidas no roteiro*/
bool eh_instr (char *token) {

	return !strcmp(token,"add")
	|| !strcmp(token,"sub")
	|| !strcmp(token,"mult")
	|| !strcmp(token,"mul")
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
	|| !strcmp(token,"const")
	|| !strcmp(token,"public")
	|| !strcmp(token,"extern")
	|| !strcmp(token,"begin")
	|| !strcmp(token,"end");
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

/** Recebe um buffer com uma instrução, separa cada token em sua função sintática

	Para mim a linha completa eh da forma:

	rotulo: INSTR OPERANDO1 (+/- MOD1) , OPERANDO2 (+/- MOD2) ; COMENTARIO

Retorna true se não foi encontrado nenhum erro sintático ou léxico
Retorna false se foi encontrado algum erro sintático
*/
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

			tem_stop = true;

			/*Se tiver mais alguma coisa na linha eh erro de argumentos demais!*/
			if (get_token(buffer, aux)) {
				relata_erros(102, instr, NULL);
				return false;
			}
			return true;
		}

		/*Todos as outras operações tem que ter pelo menos um operando*/
		else if (!get_token(buffer, opr1)) {
			relata_erros(103, instr, NULL);
			return false;
		}

		/*Se o argumento nao for rotulo válido, erro léxico*/
		else if (!rotulo_valido(opr1)){
			relata_erros(104, opr1, instr);
			*opr1 = '\0';
		}

		/*Chegou aqui então eh uma instrução (diferente de stop) e o primeiro argumento eh válido*/

		/*Se tiver um modificador, anoto. Se não tiver, tá de boa tb*/
		if (eh_modificador_valido(buffer)) {
			get_token(buffer, aux);

			if (*aux == '+'){
				get_token(buffer, aux);
				*mod1 = converte_int(aux);
			}
			else {
				get_token(buffer, aux);
				*mod1 = -1*converte_int(aux);
			}
		}

		/*Se for copy, tem que pegar mais um operando!*/
		if (!strcmp(instr, "copy")) {

			/*Tem que ter dois tokens para puxar, um pra ser a vírgula e outro o opr2*/
			if (!get_token(buffer, aux)) {
				relata_erros(103, "copy", NULL);
				return false;
			}

			/*Se não for a vírgula, reclame e peça a vírgula*/
			else if (*aux != ',') {
				relata_erros(105, NULL, NULL);

				if (get_token(buffer, aux))
					relata_erros(102, instr, NULL);

				return false;
			}

			/*Se faltar o segundo argumento, tá errado*/
			else if(!get_token(buffer, opr2)) {
				relata_erros(103, "copy", NULL);
				return false;
			}

			/*Se nao for rotulo válido, erro léxico*/
			else if (!rotulo_valido(opr2)){
				relata_erros(104, opr2, instr);
				*opr2 = '\0';
				return false;
			}

			/*Se tiver um modificador a seguir, pode puxar*/
			else if (eh_modificador_valido(buffer)) {
				get_token(buffer, aux);

				if (*aux == '+'){
					get_token(buffer, aux);
					*mod2 = converte_int(aux);
				}
				else {
					get_token(buffer, aux);
					*mod2 = -1*converte_int(aux);
				}
			}
		}

		/*Se tiver mais alguma coisa para buscar na linha, eh erro!*/
		if (get_token(buffer, aux)) {
			relata_erros (102, instr, NULL);
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
				relata_erros(109, dir, NULL);
				return false;
			}

			/*Se não for text ou data, a seção não é reconhecida pelas regras do roteiro*/
			else if (strcmp(opr1, "text") && strcmp(opr1, "data")) {
				relata_erros(111, dir, NULL);
				return false;
			}
		}

		else if (!strcmp(dir, "public")){

			/*Tenta pegar o operador, se não tiver nada na linha eh erro de falta de argumentos*/
			if (!get_token(buffer, opr1)) {
				relata_erros(109, dir, NULL);
				return false;
			}

			/*Se nao for rotulo válido, erro léxico*/
			else if (!rotulo_valido(opr1)){
				relata_erros(104, opr1, instr);
				*opr1 = '\0';
				return false;
			}
		}

		else if (!strcmp(dir, "space")) {

			/*Se tiver um numero a seguir, pode puxar.*/
			if (get_token(buffer, aux)) {

				if (eh_num(aux)) {
					*mod1 = converte_int(aux);

					/*Se o valor do modificador for negativo, tenho que parar o programador*/
					if (*mod1 < 0) {
						*mod1 = 1;
						relata_erros(112, NULL, NULL);
						return false;
					}
				}
				else {
					relata_erros(112, NULL, NULL);
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
				relata_erros(109, dir, NULL);
				return false;
			}

			/*Se o argumento no for numérico, teste se pelo menos eh porque o usuário botou -algo*/
			if (eh_num(aux))
				*mod1 = converte_int(aux);
			else if (*aux == '-') {
				get_token(buffer, aux);

				if (eh_num(aux))
					*mod1 = -1*converte_int(aux);
				else {
					relata_erros(113, dir, NULL);
					return false;
				}
			}
			else {
				relata_erros(113, dir, NULL);
				return false;
			}
		}

		/*Tem um problema específico com extern, que eh a falta de um rotulo para botar na tabela*/
		else if (!strcmp(dir, "extern") && (*rotulo == '\0')){
			relata_erros(123, NULL, NULL);
		}

		/*NÃO SEI SE EU DEVO RECLAMAR COM O USUÁRIO OU ASSUMIR QUE ELE SABE O QUE FAZ... POIS PARECE MUITA BURRICE
		if (!strcmp(dir, 'begin' && *label == '\0') {
				relata_erros(118, dir, NULL);
				error_count++
				continue;
		}*/

		/*Se tiver mais alguma coisa na linha, tem que identificar como erro*/
		if (get_token(buffer, aux)) {
			relata_erros(110, dir, NULL);
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
		relata_erros(106, NULL, NULL);
		return false + 0*separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);;
	}

	/*Se o rotulo não for válido, erro léxico!*/
	else if (!rotulo_valido(aux)){
		relata_erros(107, aux, NULL);
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

		relata_erros(107, aux, NULL);

		if (*buffer != ':')
			relata_erros(108, aux, NULL);
		else
			get_token(buffer, aux);

		return false + 0*separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);;
	}

	/*Erro de definição do rótulo! Faltou o ':'*/
	else if (*buffer != ':') {
		relata_erros(108, aux, NULL);
		strcpy(rotulo, aux);
		return false + 0*separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);;
	}

	/*Separo o rotulo do resto linha e chamo a função novamente*/
	strcpy(rotulo, aux);
	get_token(buffer, aux);
	return separa_linha (buffer, rotulo, instr, dir, opr1, mod1, opr2, mod2);
}

/** Verifico se no buffer os tokens às seguir são operação e numero */
bool eh_modificador_valido (char *buffer) {

	char buffer_local[linesize], num[tokensize], op[tokensize];

	strcpy(buffer_local, buffer);

	return get_token(buffer_local, op)
	&& get_token(buffer_local, num)
	&& (*op=='+' || *op=='-')
	&& (eh_num(num));
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

/*Retorna o tamanho da instrução: 0-3 bytes*/
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

SYMBOL_TABLE* add_tab_simbolos (char *rotulo, bool eh_externo, bool eh_publico, bool eh_constante, bool eh_in_text, int valor, int tamanho) {
	SYMBOL_TABLE *novo;

	/*Alocando a memória*/
	novo = malloc (sizeof(SYMBOL_TABLE));
	novo->rotulo = malloc(strlen(rotulo)+1);

	strcpy(novo->rotulo, rotulo);

	if (eh_publico || eh_externo)
		novo->endereco = 0;
	else
		novo->endereco = mem_counter;

	novo->publico = eh_publico;
	novo->externo = eh_externo;
	novo->constante = eh_constante;
	novo->tamanho = tamanho;

	if (eh_constante)
		novo->valor = valor;
	else
		novo->valor = 0;

	novo->prox = NULL;
	novo->in_text = eh_in_text;
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

SYMBOL_TABLE* busca_tabela (SYMBOL_TABLE *prim, char *alvo) {

	if (*alvo == '\0')
		return NULL;

	for (;prim != NULL; prim = prim->prox) {
		if (!strcmp(prim->rotulo, alvo))
			return prim;
	}
	return NULL;
}

void analise ( ) {

	/*Buffers*/
	char linha[linesize];

	/*Tokens separados*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	/*Tabelas*/
	SYMBOL_TABLE *last_symbol, *aux;
	first_symbol = last_symbol = aux = NULL;

	for (; get_linha(linha) == true ; mem_counter += tam_instr(instr, mod1, dir)) {

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		/*Separa as instrucoes e vai contando os erros encontrados*/
		separa_linha(linha, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		/*Mudança para a seção 1 da primeira vez*/
		if (section == 0 && !strcmp("section", dir) && !strcmp("text", opr1))
			section = 1;

		/*Ou seção text*/
		else if (section == 1) {

			/*Diretiva section: quebra para a seção data*/
			if (!strcmp(dir, "section") && !strcmp(opr1, "data")) {
				section = 2;
				continue;
			}

			/*Essas diretivas não podem vir aqui dentro, tem que vir na seção data*/
			else if (!strcmp(dir, "const") || !strcmp(dir, "space"))
				relata_erros(117, dir, NULL);

			else if (!strcmp(dir, "begin") || !strcmp(dir, "end") ) {

				if (!strcmp(dir, "begin") && encontrei_begin == true)
					relata_erros(116, NULL, NULL);
				else if (!strcmp(dir, "begin") && encontrei_begin == false)
					encontrei_begin = true;
				else if (!strcmp(dir, "end") && encontrei_begin == false)
					relata_erros(119, NULL, NULL);
				else
					encontrei_begin = false;

				if (!strcmp(dir, "begin") && mem_counter!=0)
					relata_erros(129, NULL, NULL);

				/*Adicionando o rótulo junto as diretivas begin/end na tabela de simbolos*/
				if ( (aux=busca_tabela(first_symbol,rotulo)) == NULL ) {
						if (first_symbol == NULL)
							first_symbol = last_symbol = add_tab_simbolos(rotulo, false, false, false, true, 0, 0);
						else {
							last_symbol->prox = add_tab_simbolos(rotulo, false, false, false, true, 0, 0);
							last_symbol = last_symbol->prox;
						}
				}
				else {
					if (aux->publico || aux->externo){
						aux->endereco = mem_counter;
						aux->in_text = true;
					}
					else
						relata_erros(121, opr1, NULL);
				}
			}

			/*Adicionando variáveis ou modificando o parâmetro 'public' delas*/
			else if (!strcmp(dir, "public")){
				if (*opr1 != '\0' && rotulo_valido(opr1)) {
					if ((aux=busca_tabela(first_symbol,opr1)) == NULL) {
						if (first_symbol == NULL)
							first_symbol = last_symbol = add_tab_simbolos(opr1, false, true, false, false, 0, 0);
						else {
							last_symbol->prox = add_tab_simbolos(opr1, false, true, false, false, 0, 0);
							last_symbol = last_symbol->prox;
						}
					}
					else
						aux->publico = true;
				}
			}

			/*Adicionando variáveis ou modificando o parâmetro 'extern' delas*/
			else if (!strcmp(dir, "extern")){
				if (*rotulo != '\0' && rotulo_valido(rotulo)) {
					if ((aux=busca_tabela(first_symbol,rotulo)) == NULL) {
						if (first_symbol == NULL)
							first_symbol = last_symbol = add_tab_simbolos(rotulo, true, false, false, true, 0, 0);
						else {
							last_symbol->prox = add_tab_simbolos(rotulo, true, false, false, true, 0, 0);
							last_symbol = last_symbol->prox;
						}
					}
					else
						aux->externo = true;
				}
			}

			/*Se cair aqui, eh uma instrucao comum. Se tiver rótulo vai para a tabela de simbolos*/
			else if (*rotulo != '\0' && rotulo_valido(rotulo)) {
				if ((aux=busca_tabela(first_symbol,rotulo)) == NULL) {
					if (first_symbol == NULL)
						first_symbol = last_symbol = add_tab_simbolos(rotulo, false, false, false, true, 0, 0);
					else {
						last_symbol->prox = add_tab_simbolos(rotulo, false, false, false, true, 0, 0);
						last_symbol = last_symbol->prox;
					}
				}
				else {
					if (aux->publico || aux->externo){
						aux->endereco = mem_counter;
						aux->in_text = true;
					}
					else
						relata_erros(121, opr1, NULL);
				}
			}
		}

		/*Ou seção data*/
		else if (section == 2) {

			/*Diretiva section: quebra para a seção text*/
			if (!strcmp(dir, "section")){
				if (!strcmp(opr1, "text"))
					section = 1;
			}
			else if (!strcmp(dir, "const")) {
				if (*rotulo != '\0') {
					if ((aux=busca_tabela(first_symbol,rotulo)) == NULL) {
						if (first_symbol == NULL)
							first_symbol = last_symbol = add_tab_simbolos(rotulo, false, false, true, false, mod1, 1);
						else {
							last_symbol->prox = add_tab_simbolos(rotulo, false, false, true, false, mod1, 1);
							last_symbol = last_symbol->prox;
						}
					}
					else {
						if (aux->publico || aux->externo)
							aux->endereco = mem_counter;
						else
							relata_erros(121, opr1, NULL);
					}
				}
			}
			else if (!strcmp(dir, "space")){
				if (*rotulo != '\0') {
					if ((aux=busca_tabela(first_symbol,rotulo)) == NULL) {
						if (first_symbol == NULL)
							first_symbol = last_symbol = add_tab_simbolos(rotulo, false, false, false, false, 0, mod1);
						else {
							last_symbol->prox = add_tab_simbolos(rotulo, false, false, false, false, 0, mod1);
							last_symbol = last_symbol->prox;
						}
					}
					else {
						if (aux->publico){
							aux->endereco = mem_counter;
							aux->tamanho = mod1;
						}
						else if (aux->externo == true)
							relata_erros(130, rotulo, NULL);
						else
							relata_erros(121, opr1, NULL);
					}
				}
			}
			else if (!strcmp(dir, "public")){
				if (*opr1 != '\0') {
					if ((aux=busca_tabela(first_symbol,rotulo)) == NULL) {
						if (first_symbol == NULL)
							first_symbol = last_symbol = add_tab_simbolos(opr1, false, true, false, false, 0, 0);
						else {
							last_symbol->prox = add_tab_simbolos(opr1, false, true, false, false, 0, 0);
							last_symbol = last_symbol->prox;
						}
					}
					else
						aux->publico = true;
				}
			}
			else if (!strcmp(dir, "extern")){
				if (*rotulo != '\0') {
					if ((aux=busca_tabela(first_symbol,rotulo)) == NULL) {
						if (first_symbol == NULL)
							first_symbol = last_symbol = add_tab_simbolos(rotulo, true, false, false, false, 0, 0);
						else {
							last_symbol->prox = add_tab_simbolos(rotulo, true, false, false, false, 0, 0);
							last_symbol = last_symbol->prox;
						}
					}
					else
						aux->externo = true;
				}
			}
			else if (!strcmp(dir, "end")) {
				if (encontrei_begin == true)
					encontrei_begin = false;
				else
					relata_erros(119, NULL, NULL);
			}
			else
				relata_erros(120, dir, instr);
		}

		/*Ou estamos no começo do código, nem entramos em seção nenhuma. Nesse caso não consigo pensar em nada valido */
		else
			relata_erros(114, NULL, NULL);
	}

	/*Se tem um 'begin' aberto, tem que dar erro pois ele deveria estar fechado.*/
	if (encontrei_begin)
		relata_erros(115, NULL, NULL);

	if (!tem_stop)
		relata_erros(132, NULL, NULL);

	/*Procura por simbolos publicos que não foram definidos em lugar nenhum*/
	for (aux = first_symbol; aux != NULL; aux = aux->prox){
		if (aux->publico == true && (aux->endereco == -1)){
			relata_erros(124, aux->rotulo, NULL);
			auto_suficiente = false;
		}
		if (aux->externo)
			auto_suficiente = false;
	}

	/*PREPARA PARA A SEGUNDA PASSAGEM*/
	prog_size = mem_counter;
	mem_counter = line_counter = 0;
	section = 0;
	rewind(src);
	passagem++;

	/*PROCURA POR PROBLEMAS DE ROTULOS (NAO DEFINIDOS, MODIFICAR CONSTANTE, DIVISÃO POR ZERO*/
	/*APROVEITO E FAÇO A TABELA DE USO DE MODULOS EXTERNOS AO ARQUIVO FONTE*/
	for (; get_linha(linha) == true ; mem_counter += tam_instr(instr, mod1, dir)) {

		if (debug_mode1 == true){
			printf ("LINHA: '%s'\n", linha);
		}

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		/*Separa as instrucoes e vai contando os erros encontrados*/
		separa_linha(linha, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		if (debug_mode1 == true) {
			printf ("ROTULO: '%s'\n", rotulo);
			printf ("INSTR: '%s'\n", instr);
			printf ("DIR: '%s'\n", dir);
			printf ("OPR1: '%s'\n", opr1);
			printf ("MOD1: '%d'\n", mod1);
			printf ("OPR2: '%s'\n", opr2);
			printf ("MOD2: '%d'\n\n", mod2);
		}

		/*Mudança para a seção 1 da primeira vez*/
		if (section == 0 && !strcmp("section", dir) && !strcmp("text", opr1))
			section = 1;

		/*Ou seção text*/
		else if (section == 1) {

			/*Diretiva section: quebra para a seção data*/
			if (!strcmp(dir, "section") && !strcmp(opr1, "data")) {
				section = 2;
				continue;
			}

			/*Qualquer outra diretiva não tem erros para serem verificados nessa passagem*/
			if (*dir != '\0')
				continue;

			/*Stop não tem erros para verificar nessa passagem*/
			if (!strcmp(instr, "stop"))
				continue;

			/*Se não achar o argumento na tabela, deu ruim*/
			aux = busca_tabela(first_symbol, opr1);
			if (aux == NULL){
				relata_erros(131, opr1, NULL);
				continue;
			}

			/*Proibo pulo para rotulos na seção data*/
			if ((!strcmp(instr, "jmp") || !strcmp(instr, "jmpp") || !strcmp(instr, "jmpn")) && !aux->in_text )
				relata_erros(126, aux->rotulo, NULL);

			/*Proibo acesso a dados fora do vetor*/
			else if (!aux->externo && ((mod1<0) || (mod1>aux->tamanho)))
				relata_erros(128, aux->rotulo, NULL);

			/*Proibo alterar o valor de uma variável constante*/
			else if (!strcmp(instr, "store") && aux->constante)
				relata_erros(127, aux->rotulo, NULL);

			/*Proibe divisão por zero*/
			else if (!strcmp(instr, "div") && aux != NULL && !aux->in_text && aux->valor==0 )
				relata_erros(125, NULL, NULL);

			/*COPY tem alguns erros específicos no segundo argumento*/
			else if (!strcmp(instr, "copy")){
				aux = busca_tabela(first_symbol, opr2);
				if (aux == NULL){
					relata_erros(131, opr2, NULL);
					continue;
				}

				/*Proibo acesso a dados fora do vetor*/
				if (!aux->externo && ((mod1<0) || (mod1>aux->tamanho)))
					relata_erros(128, aux->rotulo, NULL);
			}
		}
	}
}

void write_tables_output ( ) {

	/*Buffer*/
	char linha[linesize], uppercase[tokensize];

	/*Partes da instrução*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	/*Tabelas*/
	SYMBOL_TABLE *aux;

	mem_counter = 0;
	section = 0;
	rewind(src);
	passagem++;

	fprintf (output, "TABLE USE\n");

	for (; get_linha(linha) == true ; mem_counter += tam_instr(instr, mod1, dir)) {

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		/*Separa as instrucoes*/
		separa_linha(linha, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		/*Se for alguma diretiva, não tem como usar argumentos externos*/
		if (*dir != '\0')
			continue;

		/*Se a instrução não for stop, ache na tabela o argumento e veja se eh externo*/
		if (strcmp(instr, "stop")) {

			aux = busca_tabela(first_symbol, opr1);
			if (aux != NULL && aux->externo == true) {
				str_caseup(aux->rotulo, uppercase);
				fprintf (output, "%s %d\n", uppercase, mem_counter+1);
			}
		}

		/*Se a instrução for copy pegue o seguinte e faça a mesma coisa*/
		if (!strcmp(instr, "copy")) {

			aux = busca_tabela(first_symbol, opr2);
			if (aux != NULL && aux->externo == true) {
				str_caseup(aux->rotulo, uppercase);
				fprintf (output, "%s %d\n", uppercase, mem_counter+2);
			}
		}
	}

	fprintf (output, "\nTABLE DEFINITION\n");
	for (aux = first_symbol; aux != NULL; aux = aux->prox)
		if (aux->publico == true){
			str_caseup(aux->rotulo, uppercase);
			fprintf (output, "%s %d\n", uppercase, aux->endereco);
		}
}

void sintese ( ) {

	/*Buffer*/
	char linha[linesize];

	/*Partes da instrução*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	/*Tabelas*/
	SYMBOL_TABLE *aux;
	int help;

	if (!auto_suficiente)
		fprintf (output, "\nCODE\n");

	for (; get_linha(linha) == true ; mem_counter += tam_instr(instr, mod1, dir)) {

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
				fprintf (output, "%d ", help);
			}

			/*Se for stop, não tem argumentos*/
			if (!strcmp(instr, "stop"))
				continue;

			/*Qualquer outra opção precisa de pelo menos um argumento*/
			aux = busca_tabela(first_symbol, opr1);
			fprintf (output, "%d ", (aux->endereco)+mod1);

			if (!strcmp(instr, "copy")){
				aux = busca_tabela(first_symbol, opr2);
				fprintf (output, "%d ", aux->endereco+mod2);
			}
		}

		else if (section == 2) {

			if (!strcmp(dir, "space"))
				for (;mod1>0; mod1--)
					fprintf (output, "0 ");
			else if (!strcmp(dir, "const"))
				fprintf (output, "%d ", mod1);
		}
	}
}

void str_caseup(char *in_str, char *out_str){

	while (*in_str != '\0'){
		*out_str = *in_str;
		if (*in_str >= 'a' && *in_str <= 'z')
			*out_str -= 32;

		in_str++;
		out_str++;
	}

	*out_str = '\0';
}

/** Se der um erro conhecido, eu aviso ao programador*/
void relata_erros (int codigo, char *aux, char *aux2) {

	/*PARA NÃO AVISAR O MESMO ERRO DUAS VEZES :D*/
	int qual_grupo = 0;

	/*Erros de segunda passagem*/
	if (codigo==122 || codigo==125 || codigo==126 || codigo==127 || codigo==128 || codigo==131 || codigo==132)
		qual_grupo = 1;

	/*Erros de primeira passagem não precisam ser reportados de novo na segunda passagem*/
	if (passagem > 1 && qual_grupo == 0)
		return;

	printf (" *** %d) ", line_counter);
	if (qual_grupo == 0){
		switch (codigo) {

			case 90:
				printf (" Falta um codigo fonte.\n");
				break;

			case 91:
				printf (" Codigo fonte nao encontrado.\n");
				break;

			case 92:
				printf (" Nao conseui abrir o arquivo de saida.\n");
				break;

			case 100:
				printf ("(Lex) A linha eh muito grande.\n");
				break;

			case 101:
				printf ("(Lex) O token '%s' eh muito grande.\n",aux);
				break;

			case 102:
				printf ("(Sint) Argumentos demais para a instrucao '%s'.\n", aux);
				break;

			case 103:
				printf ("(Sint) Faltam argumentos para a instrucao '%s'.\n", aux);
				break;

			case 104:
				printf ("(Lex) O argumento '%s' inválido para a instrucao '%s'.\n", aux, aux2);
				break;

			case 105:
				printf ("(Sint/Lex) Falta a separacao dos operandos por virgula.\n");
				break;

			case 106:
				printf ("(Sint) Dois rotulos definidos na mesma linha.\n");
				break;

			case 107:
				printf ("(Lex) Rotulo '%s' inválido.\n", aux);
				break;

			case 108:
				printf ("(Sint/Lex) Falta o simbolo ':' depois do rotulo '%s'.\n", aux);
				break;

			case 109:
				printf ("(Sint) Faltam argumentos para a diretiva '%s'.\n", aux);
				break;

			case 110:
				printf ("(Sint) Argumentos em excesso para a diretiva '%s'.\n", aux);
				break;

			case 111:
				printf ("(Lex) So existem as secoes 'text' e 'data'.\n");
				break;

			case 112:
				printf ("(Lex/Sint) A diretiva 'space' soh aceita argumento numerico positivo.\n");
				break;

			case 113:
				printf ("(Lex/Sint) A diretiva '%s' soh aceita argumento numerico.\n", aux);
				break;

			case 114:
				printf ("(Sem) O primeiro comando valido eh 'section text'.\n");
				break;

			case 115:
				printf ("(Sem) Programa termina com um 'begin' aberto.\n");
				break;

			case 116:
				printf ("(Sem) Sem aninhamento de 'begin'.\n");
				break;

			case 117:
				printf ("(Sem) A diretiva '%s' soh pode estar na secao 'data'.\n", aux);
				break;

			case 118:
				printf ("(Sint) A diretiva '%s' necessita de um rotulo.\n", aux);
				break;

			case 119:
				printf ("(Sem) Nenhum 'begin' para ser fechado pelo 'end'.\n");
				break;

			case 120:
				if (*aux == '\0')
					printf ("(Sem) A instrucao '%s' nao pode vir na secao 'data'.\n", aux2);
				else
					printf ("(Sem) A diretiva '%s' nao pode vir na secao 'data'.\n", aux);
				break;

			case 121:
				printf ("(Sem) O rotulo '%s' foi previamente definido.\n", aux);
				break;

			case 123:
				printf ("(Sint) A diretiva 'extern' precisa de um rotulo na mesma linha.\n");
				break;

			case 124:
				printf ("(Sem) O rotulo publico '%s' nao foi definido.\n", aux);
				break;

			case 129:
				printf ("(Sem) A diretiva 'begin' deve estar no comeco do arquivo.\n");
				break;

			default:
				printf ("Erro desconhecido.\n");
		}
	}

	else {
		switch (codigo) {
			case 122:
				printf ("(Sem) O rotulo '%s' nunca foi definido.\n", aux);
				break;

			case 125:
				printf ("(Sem) Divisao por zero.\n");
				break;

			case 126:
				printf ("(Sem) O rotulo '%s' nao pode ser um alvo de pulo.\n", aux);
				break;

			case 127:
				printf ("(Sem) Tentando alterar o valor da constante '%s'.\n", aux);
				break;

			case 128:
				printf ("(Sem) A instrucao '%s' tenta acessar um dado fora do vetor.\n", aux);
				break;

			case 130:
				printf ("(Sem) Definicao local do rotulo '%s' externo.\n", aux);
				break;

			case 131:
				printf ("(Sem) O rotulo '%s' nao foi definido.\n", aux);
				break;

			case 132:
				printf ("(sem) O programa nao tem um 'stop' em lugar nenhum.\n");
				break;

			default:
				;
		}
	}
	error_count++;
}

/*Abre os arquivos complementando a terminação .asm caso o usuário não tenha informado na linha de comando*/
FILE* open_sourcefile (char *name){

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
FILE* open_exitfile (char *name){

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
