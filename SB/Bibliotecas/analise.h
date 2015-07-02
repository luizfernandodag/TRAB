/*Faz a análise do código, procurando por erros e mexendo na variável global que mede erros*/
SYMBOL_TABLE* analise (char *src_name) {

	FILE *entrada;

	/*Buffers*/
	char linha[linesize];

	/*Tokens separados*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	/*Tabelas*/
	SYMBOL_TABLE *comeco_tabela, *ultimo_fila, *aux;
	comeco_tabela = ultimo_fila = aux = NULL;

	entrada = fopen(src_name, "r");
	for (; get_linha(entrada, linha) == true ; mem_counter_code += tam_instr(instr, mod1), mem_counter_data += tam_dir(dir, mod1)) {

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
	fclose(entrada);

	return comeco_tabela;
}

SYMBOL_TABLE* add_tab_simbolos (char *rotulo, bool eh_constante, bool eh_executavel, int tamanho) {
	SYMBOL_TABLE *novo;

	/*Alocando a memória*/
	novo = malloc (sizeof(SYMBOL_TABLE));
	novo->rotulo = malloc(strlen(rotulo)+1);

	strcpy(novo->rotulo, rotulo);

	novo->endereco = mem_counter_data;
	novo->constante = eh_constante;
	novo->tamanho = tamanho;
	novo->prox = NULL;
	novo->executavel = eh_executavel;

	if (eh_executavel)
		novo->endereco = mem_counter_code;

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

int tam_instr (char *instr, int mod1){

	/*Operações básicas tem 6 bytes*/
	if (!strcmp(instr, "add")||!strcmp(instr, "sub")||!strcmp(instr, "mult")||!strcmp(instr, "div"))
		return 6;

	/*jmp tem 2 bytes*/
	else if (!strcmp(instr, "jmp"))
		return 2;

	/*jmps com comparação tem 3 bytes para a comparação + 2 para o jump*/
	else if (!strcmp(instr, "jmpn")||!strcmp(instr, "jmpp")||!strcmp(instr, "jmpz"))
		return 5;

	/*Se a gente manter o push/pop então 14 bytes e caso contrário 12 bytes*/
	else if (!strcmp(instr, "copy"))
		return 12 + 2;

	/*Load/store com o eax precisa de 5 bytes*/
	else if (!strcmp(instr, "load")||!strcmp(instr, "store"))
		return 5;

	/*Input e output*/
	else if (!strcmp(instr, "input")||!strcmp(instr, "output"))
		return 19;

	/*Stop*/
	else if (!strcmp(instr, "stop"))
		return 12;

	return 0;
}

int tam_dir (char *dir, int mod1){

	/*const precisa de 4 bytes pra guardar o dado*/
	if (!strcmp(dir, "const"))
		return 4;

	/*space precisa de 4* o numero de espaços*/
	else if (!strcmp(dir, "space"))
		return 4*mod1;

	return 0;
}

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
