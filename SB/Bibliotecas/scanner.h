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
