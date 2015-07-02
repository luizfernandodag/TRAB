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
	file_pointer = fopen(argv[1], "r");

	if (file_pointer == NULL){
		relata_erros(102, NULL, NULL);
		return false;
	}

	fclose(file_pointer);
	return true;
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

/*Recebe um inteiro (32 bits) e retorna o endereço como um endereço em hex
0xABcdEF01 -> "0x01EFcdAB"
*/
int converte_littleendian (int i) {

	int swapped;

	swapped =	((i>>24)&0xff      ) | 	/* move byte 3 to byte 0*/
				((i<< 8)&0xff0000  ) |	/* move byte 1 to byte 2*/
				((i>> 8)&0xff00    ) |	/* move byte 2 to byte 1*/
				((i<<24)&0xff000000);	/* byte 0 to byte 3*/
	return swapped;
}
