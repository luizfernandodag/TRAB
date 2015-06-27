
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
