
void pre_processamento(char *entrada, char *saida){

    processa_equ(entrada, saida);
    processa_if(saida, TEMP_OUTPUT);

    copy_file(TEMP_OUTPUT, saida);
    remove(TEMP_OUTPUT);
}

/*Retorna true se processou equ e false se não processou equ.
Se ele processar equ, pode ser que tenha algum outro equ para processar depois
*/
void processa_equ (char *src_name, char *dst_name) {

	char linha[linesize];
	char buff1[tokensize], buff2[tokensize], buff3[tokensize], buff4[tokensize];

	FILE *entrada, *saida;

	/*Copiando o arquivo de entrada para a saída temporária*/
	copy_file (src_name, dst_name);

	entrada = fopen (dst_name, "r");

	/*pego uma linha*/
	while (get_linha(entrada, linha) == true) {

		/*separo a linha em partes menores*/
		get_token(linha, buff1);
		get_token(linha, buff2);
		get_token(linha, buff3);
		get_token(linha, buff4);

		/*MEU ALVO EH:
		<ALGUMA COISA> : EQU <OUTRA COISA>*/
		if (*buff2 == ':' && !strcmp(buff3, "equ")) {

			saida = fopen(TEMP_OUTPUT, "w+");

			/*Se entrou aqui vai ter que processar!
				-> buff1: palavra a ser substituida
				-> buff4: palavra a substituir
			*/

            fprintf (saida, "%s\n", linha);
			/*se o usuario inventar de botar mais argumentos, eles continuam no arquivo de saída
			ex: "<alguma coisa> : equ <outra coisa> <esses manos vao ignorar essa parada aqui?>" */


			/*Agora lê o arquivo palavra por palavra e faz o replace
				buff1 = palavra que será tirada
				buff4 = palavra que será posta no lugar */
			while (!feof(entrada)){

				get_linha(entrada, linha);

				while (get_token(linha, buff2)){
					if (!strcmp(buff2, buff1))
						fprintf (saida, "%s ", buff4);
					else
						fprintf (saida, "%s ", buff2);
				}
				fprintf (saida, "\n");
			}

				/*Fecha os arquivos*/
				fclose(entrada);
				fclose(saida);

				/*Saida vira entrada para testar outro equ*/
				copy_file(TEMP_OUTPUT,dst_name);
				entrada = fopen(dst_name, "r");
				remove(TEMP_OUTPUT);
		}
		/*O que mais pode vir antes da seção texto?
		Se não for um equ, será a seção texto e sendo a seção texto o arquivo esta certo*/
		else{
			fclose(entrada);
			return;
		}
	}

	return;
}

void processa_if (char *src_name, char *dst_name) {

	FILE *entrada, *saida;

	entrada = fopen(src_name, "r");
	saida = fopen(dst_name, "w");

	/*Buffer*/
	char linha[linesize];
	char buff1[tokensize], buff2[tokensize];

	while (get_linha(entrada, linha) == true) {

		/*Limpa os buffers*/
		*buff1 = *buff2 = '\0';

		get_token(linha, buff1);	/*Pega o primeiro token*/

		/*Se for um if*/
		if (!strcmp(buff1, "if")){

			get_token(linha, buff1);
			if (eh_num(buff1)) {

				/*se for 0, puxe a linha e não copie para a saida*/
				if (!converte_int(buff1)){
					get_linha(entrada, buff1);
				}
			}
			/*Se não for numero, deu ruim*/
			else
				relata_erros(0, NULL, NULL);

		}

		else
			fprintf (saida, "%s %s\n", buff1, linha);
	}

	fclose(entrada);
	fclose(saida);
}
