
void traduzADD(FILE *dst, char *arg1, int offset1) {
	fprintf(dst, "\nADD EAX, DWORD [%s + %d]\n", arg1, 4*offset1);
}

void traduzSUB(FILE *dst, char *arg1, int offset1) {
	fprintf(dst, "\nSUB EAX, DWORD [%s + %d]\n", arg1, 4*offset1);
}

void traduzMULT(FILE *dst, char *arg1, int offset1) {
	fprintf(dst, "\nMUL DWORD [%s + %d]\n", arg1, 4*offset1 );
}

void traduzDIV(FILE *dst, char *arg1, int offset1) {
	fprintf(dst, "\nDIV DWORD [%s + %d]\n", arg1, 4*offset1 );
}

void traduzJMP(FILE *dst, char *arg1) {
	fprintf(dst,"\nJMP %s\n", arg1);
}

void traduzJMPN(FILE *dst, char *arg1) {
	fprintf(dst,"\nCMP EAX, 0\nJB %s\n", arg1);/*JMP LABEL*/
}

void traduzJMPP(FILE *dst, char *arg1) {
	fprintf(dst,"\nCMP EAX, 0\nJA %s\n", arg1);/*JMP LABEL*/
}

void traduzJMPZ(FILE *dst, char *arg1) {
	fprintf(dst,"\nCMP EAX, 0\nJE %s\n", arg1);/*JMP LABEL*/
}

void traduzCOPY(FILE *dst, char *arg1, int offset1, char *arg2, int offset2) {
	fprintf(dst,"\nPUSH EBX");
	fprintf(dst,"\nMOV EBX, DWORD [%s + %d]", arg2, 4*offset2);
	fprintf(dst,"\nMOV DWORD [%s + %d],  EBX ", arg1, 4*offset1 );
	fprintf(dst,"\nPOP EBX\n");
}

void traduzLOAD(FILE *dst, char *arg1, int offset1) {
	fprintf(dst,"\nMOV EAX, [%s + %d]\n", arg1, 4*offset1);
}

void traduzSTORE(FILE *dst, char *arg1, int offset1) {
	fprintf(dst, "\nMOV DWORD [%s + %d], EAX\n", arg1, 4*offset1 );
}

void traduzINPUT(FILE *dst, char *arg1, int offset1) {
	fprintf(dst,"\nCALL lerInteiro" );
	fprintf(dst,"\nPUSH  EDX" );
	fprintf(dst,"\nMOV EDX, DWORD [numEntrada]");
	fprintf(dst,"\nMOV DWORD [%s + %d], EDX", arg1, 4*offset1 );
	fprintf(dst,"\nPOP EDX\n");
}

void traduzOUTPUT(FILE *dst, char *arg1, int offset1) {
	fprintf(dst,"\nPUSH  EDX" );
	fprintf(dst,"\nMOV EDX, DWORD [%s + %d]",arg1, 4*offset1);
	fprintf(dst,"\nMOV DWORD [numSaida], EDX" );
	fprintf(dst,"\nCALL escreverInteiro" );
	fprintf(dst,"\nPOP EDX\n");
}

void traduzSTOP (FILE *dst) {
	fprintf (dst, "\nMOV EAX, 1\nMOV EBX, 0\nint 80h\n");
}

void traduzSECTION (FILE *dst, char *arg1){
	if (!strcmp(arg1, "data"))
		fprintf (dst, "\nsection .data\n");
	else
		fprintf (dst, "\nsection .text\n");
}

void traduzCONST(FILE *dst, char *rotulo, int constante) {
	fprintf(dst, "\n%s: dd %d", rotulo, constante );
}

void traduzSPACE(FILE *dst, char *rotulo, int quantidade) {
	fprintf(dst, "\n%s dd 0", rotulo);

	while (--quantidade>0)
		fprintf (dst, ", 0");
}

void escreveFuncaoEscreverInteiro(FILE * arq) {

	fprintf(arq, "\n\nsection .text");
	fprintf(arq, "%s","\nescreverInteiro:\n");
	fprintf(arq, "%s","enter 0,0\n");
	fprintf(arq, "%s","push eax\n");
	fprintf(arq, "%s","push ebx\n");
	fprintf(arq, "%s","push ecx\n");
	fprintf(arq, "%s","push edx\n");
	fprintf(arq, "%s","mov eax, dword [numSaida]\n");

	fprintf(arq, "%s","xor ecx, ecx\n");
	fprintf(arq, "%s","xor edx, edx\n");
	fprintf(arq, "%s","xor ebx, ebx\n");

	fprintf(arq, "%s","loop1:\n");
	fprintf(arq, "%s","inc ecx\n");
	fprintf(arq, "%s","xor edx, edx\n");
	fprintf(arq, "%s","div dword [_dez_]\n");
	fprintf(arq, "%s","cmp eax, 0\n");
	fprintf(arq, "%s","jle fim\n");

	fprintf(arq, "%s","mov byte [numSaidaStringAux + ebx], dl\n");
	fprintf(arq, "%s","add byte [numSaidaStringAux + ebx], 0x30\n");
	fprintf(arq, "%s","inc ebx\n");


	fprintf(arq, "%s","jmp loop1\n");

	fprintf(arq, "%s","xor ebx, ebx\n");

	fprintf(arq, "%s","fim:\n");
	fprintf(arq, "%s","mov byte [numSaidaStringAux + ebx], dl\n");
	fprintf(arq, "%s","add byte [numSaidaStringAux + ebx], 0x30\n");

	fprintf(arq, "%s","xor ebx, ebx\n");
	fprintf(arq, "%s","loop2:\n");

	fprintf(arq, "%s","mov al, byte [numSaidaStringAux + ecx];\n");
	fprintf(arq, "%s","mov byte [numSaidaString + ebx], al\n");

	fprintf(arq, "%s","cmp ecx, 0\n");
	fprintf(arq, "%s","je fim2\n");

	fprintf(arq, "%s","inc ebx\n");
	fprintf(arq, "%s","dec ecx\n");
	fprintf(arq, "%s","jmp loop2\n");

	fprintf(arq, "%s","fim2:\n");

	fprintf(arq, "%s","mov eax, 4\n");
	fprintf(arq, "%s","mov ebx, 1\n");
	fprintf(arq, "%s","mov ecx, numSaidaString\n");
	fprintf(arq, "%s","mov edx, 11\n");
	fprintf(arq, "%s","int 80h\n");

/**Adicionei um enter no fim do número aqui*/
	fprintf (arq, "mov eax, 4\n");
	fprintf (arq, "mov ebx, 1\n");
	fprintf (arq, "mov ecx, _enter_\n");
	fprintf (arq, "mov edx, 2\n");
	fprintf(arq, "%s","int 80h\n");
/**/
	fprintf(arq, "%s","pop edx\n");
	fprintf(arq, "%s","pop ecx\n");
	fprintf(arq, "%s","pop ebx\n");
	fprintf(arq, "%s","pop eax\n");
	fprintf(arq, "%s","leave\n");
	fprintf(arq, "%s","ret\n");
}

/*Funcao para escrever no dst do assembly ia32 a funcao lerInteiro*/
void escreveFuncaoLerInteiro(FILE * arq) {

	fprintf(arq, "\n\nsection .text");
	fprintf(arq, "%s","\nlerInteiro:\n");
	fprintf(arq, "%s","\nenter 0, 0");
	fprintf(arq, "%s","\npush eax");
	fprintf(arq, "%s","\npush ebx");
	fprintf(arq, "%s","\npush ecx");
	fprintf(arq, "%s","\npush edx");
	fprintf(arq, "%s","\npush esi");

	fprintf(arq, "%s","\n; ler numero");
	fprintf(arq, "%s","\nmov eax, 3");
	fprintf(arq, "%s","\nmov ebx, 0");
	fprintf(arq, "%s","\nmov ecx, _leitura_");
	fprintf(arq, "%s","\nmov edx, 10");
	fprintf(arq, "%s","\nint 80h");

	fprintf(arq, "%s","\nmov eax, 0; comecar as posicoes da string");
	fprintf(arq, "%s","\nmov [numEntrada], DWORD 0; inicializacao do numero de saida");
	fprintf(arq, "%s","\nmov esi, _leitura_");

	fprintf(arq, "%s","\nloop_soma:");
	fprintf(arq, "%s","\nxor ebx, ebx");
	fprintf(arq, "%s","\nmov bl, byte [esi]");

	fprintf(arq, "%s","\ncmp bl, 10 ; 10 = /n na tabela ASCII");
	fprintf(arq, "%s","\nje SAI");
	fprintf(arq, "%s","\nsub  bl, 0x30; conserta char");
	fprintf(arq, "%s","\nmov eax, dword [numEntrada]");
	fprintf(arq, "%s","\nmul dword [_dez_]");
	fprintf(arq, "%s","\nmov dword [numEntrada], eax ");
	fprintf(arq, "%s","\nadd dword [numEntrada], ebx");

	fprintf(arq, "%s","\ninc esi");

	fprintf(arq, "%s","\nloop loop_soma");

	fprintf(arq, "%s","\nSAI:");

	fprintf(arq, "%s","\npop esi");
	fprintf(arq, "%s","\npop edx");
	fprintf(arq, "%s","\npop ecx");
	fprintf(arq, "%s","\npop ebx");
	fprintf(arq, "%s","\npop eax");
	fprintf(arq, "%s","\nleave");
	fprintf(arq, "%s","\nret\n");
}

void sintese_linguagem (char *src_name, char *dst_name) {

	FILE *src, *dst;

	char buffer[linesize];

	/*Tokens separados*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	src = fopen(src_name, "r");
	dst = fopen(dst_name, "w");

	/*Esse pedaço é necessário em qualquer arquivo*/
	fprintf(dst, "global _start");

	while (get_linha(src, buffer)){

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		separa_linha(buffer, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		/*Anotando aqui os rótulos no código que eu tava ignorando*/
		if (*rotulo != '\0' && *dir == '\0'){
			fprintf (dst, "\n%s: ", rotulo);
		}

		if (!strcmp(instr, "add") || !strcmp(instr, "ADD"))
			traduzADD(dst, opr1, mod1);

		else if (!strcmp(instr, "sub") || !strcmp(instr, "SUB"))
			traduzSUB(dst, opr1, mod1);

		else if (!strcmp(instr, "mult") || !strcmp(instr, "MULT"))
			traduzMULT(dst, opr1, mod1);

		else if (!strcmp(instr, "div") || !strcmp(instr, "DIV"))
			traduzDIV(dst, opr1, mod1);

		else if (!strcmp(instr, "jmp") || !strcmp(instr, "JMP"))
			traduzJMP(dst, opr1);

		else if (!strcmp(instr, "jmpp") || !strcmp(instr, "JMPP"))
			traduzJMPP(dst, opr1);

		else if (!strcmp(instr, "jmpn") || !strcmp(instr, "JMPN"))
			traduzJMPN(dst, opr1);

		else if (!strcmp(instr, "jmpz") || !strcmp(instr, "JMPZ"))
			traduzJMPZ(dst, opr1);

		else if (!strcmp(instr, "copy") || !strcmp(instr, "COPY"))
			traduzCOPY(dst, opr1, mod1, opr2, mod2);

		else if (!strcmp(instr, "load") || !strcmp(instr, "LOAD"))
			traduzLOAD(dst, opr1, mod1);

		else if (!strcmp(instr, "store") || !strcmp(instr, "STORE"))
			traduzSTORE(dst, opr1, mod1);

		else if (!strcmp(instr, "input") || !strcmp(instr, "INPUT")) {
			traduzINPUT(dst, opr1, mod1);
			usa_input = true;
		}

		else if (!strcmp(instr, "output") || !strcmp(instr, "OUTPUT")){
			traduzOUTPUT(dst, opr1, mod1);
			usa_output = true;
		}

		else if (!strcmp(instr, "stop") || !strcmp(instr, "STOP"))
			traduzSTOP(dst);

		else if (!strcmp(dir, "section") || !strcmp(instr, "SECTION")){
			traduzSECTION(dst, opr1);

			/*Definindo o global start*/
			if (globalstart_defined==false && !strcmp(opr1, "text")) {
				fprintf (dst, "_start:");
				globalstart_defined = true;
			}
		}

		else if (!strcmp(dir, "space"))
			traduzSPACE(dst, rotulo, mod1);

		else if (!strcmp(dir, "const"))
			traduzCONST(dst, rotulo, mod1);
	}

	/*Adicionando as bibliotecas ao arquivo executável*/

	if (usa_input || usa_output){
		fprintf (dst, "\nsection .data");
		fprintf (dst, "\n_enter_ db 0ah, 0dH");
		fprintf (dst, "\nnumEntrada dd 0");
		fprintf (dst, "\n_leitura_ dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaida dd 0");
		fprintf (dst, "\n_dez_ dd 10");
		fprintf (dst, "\nnumSaidaString dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
		fprintf (dst, "\nnumSaidaStringAux dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");

		if (usa_input)
			escreveFuncaoLerInteiro(dst);

		if (usa_output)
			escreveFuncaoEscreverInteiro(dst);

	}

	fclose(src);
	fclose(dst);
}

void traduzADDOp(FILE *dst, char *enderecoVariavel) {
	fprintf(dst, "03 05 %s ", enderecoVariavel );
}

void traduzSUBOp(FILE *dst, char *enderecoVariavel) {
	fprintf(dst, "2b 05 %s ", enderecoVariavel );
}

void traduzMULTOp(FILE *dst, char *enderecoVariavel) {
	fprintf(dst, "F7 25 %s ", enderecoVariavel );
}

void traduzDIVOp(FILE *dst, char *enderecoVariavel) {
	fprintf(dst, "2b 05 %s ", enderecoVariavel );
}

void traduzCOPYOp(FILE *dst, char *enderecoVariavel1, char *enderecoVariavel2 ) {
	fprintf(dst, "53 " );
	fprintf(dst, "8b 1d %s ", enderecoVariavel2);
	fprintf(dst, "89 1d %s 5b ",enderecoVariavel1 );
	fprintf(dst, "5b " );
}

void traduzLOADOp(FILE *dst, char *enderecoVariavel) {
	fprintf(dst, "a1 %s ", enderecoVariavel );
}

void traduzStoreOp(FILE *dst, char *enderecoVariavel) {
	fprintf(dst, "a3 %s ", enderecoVariavel );
}

void traduzINPUTOp(FILE *dst, char *enderecoFuncaoInput, char * endereconumEntrada char * enderecoVariavel) {
	fprintf(dst, "e8 %s ", enderecoFuncaoInput );
	fprintf(dst, "52 ");
	fprintf(dst, "8b 15  %s ", endereconumEntrada );
	fprintf(dst, "89 15 %s ", enderecoVariavel );
	fprintf(dst, "52 ");

}

void traduzOUTPUTOp(FILE *dst, char *enderecoFuncaoOutput, char * endereconumSaida char * enderecoVariavel) {
	fprintf(dst, "52 ");
	fprintf(dst, "8b 15  %s ", enderecoVariavel );
	fprintf(dst, "89 15 %s ", endereconumSaida );
	fprintf(dst, "52 ");
	fprintf(dst, "e8 %s ", enderecoFuncaoInput );
	fprintf(dst, "5a " );

}

void traduzSTOPOp (FILE *dst) {
	fprintf (dst, "b8 01 00 00 00 bb 00 00 00 00 cd 80");
}

void traduzCONSTOp(FILE *dst, char *numeroInvertido) {
	fprintf(dst, "%s ", numeroInvertido );
}


void traduzSPACEOp(FILE *dst,  int quantidade) {
	
	while (--quantidade>0)
		fprintf (dst, "00 00");
}

void escreveFuncaoEscreverInteiroOp(FILE * arq, char * numSaida, char *enderecoDez, char * numSaidaStringAuxEndereco, char * numSaidaStringEndereco, char * enterString) {

	fprintf(arq, "c8 00 00 00 ");//fprintf(arq, "%s","enter 0,0\n");
	fprintf(arq, "50 ");//fprintf(arq, "%s","push eax\n");
	fprintf(arq, "53 ");//fprintf(arq, "%s","push ebx\n");
	fprintf(arq, "51 ");//fprintf(arq, "%s","push ecx\n");
	fprintf(arq, "52 ");//fprintf(arq, "%s","push edx\n");
	fprintf(arq, "a2 %s ", numSaida );//fprintf(arq, "%s","mov eax, dword [numSaida]\n");

	fprintf(arq, "31 c9 ");//fprintf(arq, "%s","xor ecx, ecx\n");
	fprintf(arq, "31 d2 ");//fprintf(arq, "%s","xor edx, edx\n");
	fprintf(arq, "31 db ");//fprintf(arq, "%s","xor ebx, ebx\n");

	//fprintf(arq, "%s","loop1:\n");
	fprintf(arq, "41 ");//fprintf(arq, "%s","inc ecx\n");
	fprintf(arq, "31 d2 ");//fprintf(arq, "%s","xor edx, edx\n");
	fprintf(arq, "f7 35 %s ", enderecoDez);//fprintf(arq, "%s","div dword [_dez_]\n");
	fprintf(arq, "83 f8 00");//fprintf(arq, "%s","cmp eax, 0\n");
	
	//PULO OPCODE INDEFINIDO//
	//fprintf(arq, "7e ", );//fprintf(arq, "%s","jle fim\n");
    //*****************************//
	
	fprintf(arq, "88 93 %s", numSaidaStringAuxEndereco);//fprintf(arq, "%s","mov byte [numSaidaStringAux + ebx], dl\n");
	fprintf(arq, "80 83 %s 30", numSaidaStringAuxEndereco);//fprintf(arq, "%s","add byte [numSaidaStringAux + ebx], 0x30\n");
	fprintf(arq, "43 " )//fprintf(arq, "%s","inc ebx\n");

	//PULO OPCODE INDEFINIDO//
	//fprintf(arq, "%s","jmp loop1\n");
    //***********************//

	fprintf(arq, "31 d2 ");//fprintf(arq, "%s","xor ebx, ebx\n");

	//fprintf(arq, "%s","fim:\n");
	fprintf(arq, "88 93 %s", numSaidaStringAuxEndereco);//fprintf(arq, "%s","mov byte [numSaidaStringAux + ebx], dl\n");
	fprintf(arq, "80 83 %s 30", numSaidaStringAuxEndereco);//fprintf(arq, "%s","add byte [numSaidaStringAux + ebx], 0x30\n");

	fprintf(arq, "31 db ");//fprintf(arq, "%s","xor ebx, ebx\n");
	//fprintf(arq, "%s","loop2:\n");

	fprintf(arq, "8a 81 %s ", numSaidaStringAuxEndereco);//fprintf(arq, "%s","mov al, byte [numSaidaStringAux + ecx];\n");
	fprintf(arq, "88 93 %s ", numSaidaStringEndereco);////fprintf(arq, "%s","mov byte [numSaidaString + ebx], al\n");

	fprintf(arq, "83 f9 00 ");//fprintf(arq, "%s","cmp ecx, 0\n");
	//PULO OPCODE INDEFINIDO
	//fprintf(arq, "%s","je fim2\n");
    /********************************/
	fprintf(arq, "43 ");//fprintf(arq, "%s","inc ebx\n");
	fprintf(arq, "49 ");////fprintf(arq, "%s","dec ecx\n");
	
	// PULO OPCODE INDEFINIDO
	//fprintf(arq, "%s","jmp loop2\n");
	// *********************************//
	//fprintf(arq, "%s","fim2:\n");

	fprintf(arq, "b8 04 00 00 00 ");////fprintf(arq, "%s","mov eax, 4\n");
	fprintf(arq, "bb 01 00 00 00 ");//fprintf(arq, "%s","mov ebx, 1\n");
	fprintf(arq, "b9 %s ", numSaidaStringEndereco);//fprintf(arq, "%s","mov ecx, numSaidaString\n");
	fprintf(arq, "ba 0b 00 00 00 ");//fprintf(arq, "%s","mov edx, 11\n");
	fprintf(arq, "cd 80 ");//fprintf(arq, "%s","int 80h\n");

/**Adicionei um enter no fim do número aqui*/
	fprintf(arq, "b8 04 00 00 00 ");//fprintf (arq, "mov eax, 4\n");
	fprintf(arq, "bb 01 00 00 00 ");//fprintf (arq, "mov ebx, 1\n");
	fprintf(arq, "b9 %s ", enterString);//fprintf (arq, "mov ecx, _enter_\n");
	fprintf(arq, "ba 02 00 00 00 ");//fprintf (arq, "mov edx, 2\n");
	fprintf(arq, "cd 80 ");//fprintf(arq, "%s","int 80h\n");
/**/
	fprintf(arq, "5a ");//fprintf(arq, "%s","pop edx\n");
	fprintf(arq, "59 ");//fprintf(arq, "%s","pop ecx\n");
	fprintf(arq, "5b ");//fprintf(arq, "%s","pop ebx\n");
	fprintf(arq, "58");//fprintf(arq, "%s","pop eax\n");
	fprintf(arq, "c9 ");////fprintf(arq, "%s","leave\n");
	fprintf(arq, "c3 ");//fprintf(arq, "%s","ret\n");
}

/*Funcao para escrever no dst do assembly ia32 a funcao lerInteiro*/
void escreveFuncaoLerInteiroOp(FILE * arq, char * enderecoStringLeitura, char * enderecoStringnumEntrada, char * enderecoStringDez) {

	//fprintf(arq, "\n\nsection .text");
	fprintf(arq, "c8 00 00 00 ");//fprintf(arq, "%s","enter 0,0\n");
	fprintf(arq, "50 ");//fprintf(arq, "%s","push eax\n");
	fprintf(arq, "53 ");//fprintf(arq, "%s","push ebx\n");
	fprintf(arq, "51 ");//fprintf(arq, "%s","push ecx\n");
	fprintf(arq, "52 ");//fprintf(arq, "%s","push edx\n");
	fprintf(arq, "56 ");//fprintf(arq, "%s","\npush esi");

	//fprintf(arq, "%s","\n; ler numero");
	fprintf(arq, "b8 03 00 00 00 ");//fprintf(arq, "%s","\nmov eax, 3");
	fprintf(arq, "bb 00 00 00 00 ");//fprintf(arq, "%s","\nmov ebx, 0");
	fprintf(arq, "b9 %s ", enderecoStringLeitura);//fprintf(arq, "%s","\nmov ecx, _leitura_");
	fprintf(arq, "ba 0a 00 00 00 ");//fprintf(arq, "%s","\nmov edx, 10");
	fprintf(arq, "cd 80 ");//fprintf(arq, "%s","\nint 80h");liza

	fprintf(arq, "b8 00 00 00 ");////fprintf(arq, "%s","\nmov eax, 0; comecar as posicoes da string");
	fprintf(arq, "c7 05 %s ", enderecoStringnumEntrada);////fprintf(arq, "%s","\nmov [numEntrada], DWORD 0; inicializacao do numero de saida");
	fprintf(arq, "be %s ", enderecoStringLeitura);//fprintf(arq, "%s","\nmov esi, _leitura_");

	//fprintf(arq, "%s","\nloop_soma:");
	fprintf(arq, "31 db ");//fprintf(arq, "%s","\nxor ebx, ebx");
	fprintf(arq, "8a 1e ");//fprintf(arq, "%s","\nmov bl, byte [esi]");

	fprintf(arq, "80 fb 0a ");//fprintf(arq, "%s","\ncmp bl, 10 ; 10 = /n na tabela ASCII");
	// Pulo Opcode Indeterminado 
	//fprintf(arq, "%s","\nje SAI");
	// *******************************8/


	fprintf(arq, "80 eb 30  ");//fprintf(arq, "%s","\nsub  bl, 0x30; conserta char");
	fprintf(arq, "a1 %s ", enderecoStringnumEntrada);//fprintf(arq, "%s","\nmov eax, dword [numEntrada]");
	fprintf(arq, "f7 25 %s ", enderecoStringDez);//fprintf(arq, "%s","\nmul dword [_dez_]");
	fprintf(arq, "A3 %s ", enderecoStringnumEntrada);//fprintf(arq, "%s","\nmov dword [numEntrada], eax ");
	fprintf(arq, "01 1d %s ", enderecoStringnumEntrada);//fprintf(arq, "%s","\nadd dword [numEntrada], ebx");

	fprintf(arq, "46 ");//fprintf(arq, "%s","\ninc esi");
    
    // Utizando offset ate o loop_soma
	//fprintf(arq, "%s","\nloop loop_soma");
    //fprintf(arq, "E2 offset ");//
	//fprintf(arq, "%s","\nSAI:");

	fprintf(arq, "5e ");//fprintf(arq, "%s","\npop esi");
	fprintf(arq, "5a ");//fprintf(arq, "%s","\npop edx");
	fprintf(arq, "59 ");//fprintf(arq, "%s","\npop ecx");
	fprintf(arq, "5b ");//fprintf(arq, "%s","\npop ebx");
	fprintf(arq, "58 ");//fprintf(arq, "%s","\npop eax");
	fprintf(arq, "c9 ");//fprintf(arq, "%s","\nleave");
	fprintf(arq, "c3 ");//fprintf(arq, "%s","\nret\n");
}

/*
int main()
{
FILE * arq = fopen("teste.txt", "w");

char ** teste = (char **) malloc(4*sizeof(char *));
teste[0] = (char *)malloc(20* sizeof(char));
teste[1] = (char *)malloc(20* sizeof(char));

teste[0] = "ARG1";
teste[1] = "4";
teste[2] = "ARG2";
teste[3] = "0";

traduzADD(arq, teste, 2);
traduzSUB(arq, teste, 2);
traduzMULT(arq, teste, 2);
traduzDIV(arq, teste, 2);

traduzJMP(arq, teste, 1);
traduzJMPZ(arq, teste, 1);
traduzJMPP(arq, teste, 1);
traduzJMPN(arq, teste, 1);

traduzCOPY(arq, teste, 4);
traduzSTORE(arq, teste, 2);
traduzLOAD(arq, teste, 2);
traduzSPACE(arq, teste, 2);
traduzCONST(arq, teste, 2);
traduzINPUT(arq, teste, 2);
traduzOUTPUT(arq, teste, 2);



escreveFuncaoLerInteiro(arq);
escreveFuncaoEscreverInteiro(arq);




fclose(arq);
return 0;
}
*/
