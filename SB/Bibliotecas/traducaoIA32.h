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

void traduzADDOp(FILE *dst, int endereco) {
	fprintf(dst, " 03 05 %#010x", converte_littleendian(endereco));
}

void traduzSUBOp(FILE *dst, int endereco) {
	fprintf(dst, " 2B 05 %#010x", converte_littleendian(endereco));
}

void traduzMULTOp(FILE *dst, int endereco) {
	fprintf(dst, " F7 25 %#010x", converte_littleendian(endereco));
}

void traduzDIVOp(FILE *dst, int endereco) {
	fprintf(dst, " F7 35 %#010x", converte_littleendian(endereco));
}

void traduzJMPOp(FILE *dst, int endereco) {
	fprintf (dst, " E9 %#010x", converte_littleendian(endereco));
}

void traduzJMPPOp(FILE *dst, int endereco) {
	fprintf (dst, " 83 F8 00");
	fprintf (dst, " 0F 87 %#010x", converte_littleendian(endereco));
}

void traduzJMPNOp (FILE *dst, int endereco) {
	fprintf (dst, " 83 F8 00");
	fprintf (dst, " 0F 82 %#010x", converte_littleendian(endereco));
}

void traduzJMPZOp (FILE *dst, int endereco) {
	fprintf (dst, " 83 F8 00");
	fprintf (dst, " 0F 84 %#010x", converte_littleendian(endereco));
}

void traduzCOPYOp(FILE *dst, int enderecoVariavel1, int enderecoVariavel2 ) {
	fprintf(dst, " 53" );
	fprintf(dst, " 8b 1d %#010x", converte_littleendian(enderecoVariavel2));
	fprintf(dst, " 89 1d %#010x 5b", converte_littleendian(enderecoVariavel2));
	fprintf(dst, " 5b" );
}

void traduzLOADOp(FILE *dst, int enderecoVariavel) {
	fprintf(dst, " a1 %#010x", converte_littleendian(enderecoVariavel));
}

void traduzStoreOp(FILE *dst, int enderecoVariavel) {
	fprintf(dst, " a3 %#010x", converte_littleendian(enderecoVariavel));
}

void traduzINPUTOp(FILE *dst, int enderecoFuncaoInput, int endereconumEntrada, int enderecoVariavel) {
	fprintf(dst, " e8 %#010x", converte_littleendian(enderecoFuncaoInput));
	fprintf(dst, " 52");
	fprintf(dst, " 8b 15 %#010x", converte_littleendian(endereconumEntrada));
	fprintf(dst, " 89 15 %#010x", converte_littleendian(enderecoVariavel));
	fprintf(dst, " 52");
}

void traduzOUTPUTOp(FILE *dst, int enderecoFuncaoOutput, int endereconumSaida, int enderecoVariavel) {
	fprintf(dst, " 52");
	fprintf(dst, " 8b 15 %#010x", converte_littleendian(enderecoVariavel));
	fprintf(dst, " 89 15 %#010x", converte_littleendian(endereconumSaida));
	fprintf(dst, " 52");
	fprintf(dst, " e8 %#010x", converte_littleendian(enderecoFuncaoOutput));
	fprintf(dst, " 5a" );
}

void traduzSTOPOp (FILE *dst) {
	fprintf (dst, " b8 01 00 00 00 bb 00 00 00 00 cd 80");
}

void traduzCONSTOp(FILE *dst, int numeroInvertido) {
	fprintf(dst, " %#010x", converte_littleendian(numeroInvertido));
}

void traduzSPACEOp(FILE *dst,  int quantidade) {

	while (--quantidade>0)
		fprintf (dst, " 00 00");
}

void escreveFuncaoEscreverInteiroOp(FILE * arq, int numSaida, int enderecoDez, int numSaidaStringAuxEndereco, int numSaidaStringEndereco, int enterString) {

	fprintf(arq, " c8 00 00 00");//fprintf(arq, "%#010x","enter 0,0\n");
	fprintf(arq, " 50");//fprintf(arq, "%#010x","push eax\n");
	fprintf(arq, " 53");//fprintf(arq, "%#010x","push ebx\n");
	fprintf(arq, " 51");//fprintf(arq, "%#010x","push ecx\n");
	fprintf(arq, " 52");//fprintf(arq, "%#010x","push edx\n");
	fprintf(arq, " a2 %#010x", converte_littleendian(numSaida));//fprintf(arq, "%#010x","mov eax, dword [numSaida]\n");

	fprintf(arq, " 31 c9");//fprintf(arq, "%#010x","xor ecx, ecx\n");
	fprintf(arq, " 31 d2");//fprintf(arq, "%#010x","xor edx, edx\n");
	fprintf(arq, " 31 db");//fprintf(arq, "%#010x","xor ebx, ebx\n");

	//fprintf(arq, "%#010x","loop1:\n");
	fprintf(arq, " 41");//fprintf(arq, "%#010x","inc ecx\n");
	fprintf(arq, " 31 d2");//fprintf(arq, "%#010x","xor edx, edx\n");
	fprintf(arq, " f7 35 %#010x", converte_littleendian(enderecoDez));//fprintf(arq, "%#010x","div dword [_dez_]\n");
	fprintf(arq, " 83 f8 00");//fprintf(arq, "%#010x","cmp eax, 0\n");

	//PULO OPCODE INDEFINIDO//
	//fprintf(arq, "7e ", );//fprintf(arq, "%#010x","jle fim\n");
    //*****************************//

	fprintf(arq, " 88 93 %#010x", converte_littleendian(numSaidaStringAuxEndereco));//fprintf(arq, "%#010x","mov byte [numSaidaStringAux + ebx], dl\n");
	fprintf(arq, " 80 83 %#010x 30", converte_littleendian(numSaidaStringAuxEndereco));//fprintf(arq, "%#010x","add byte [numSaidaStringAux + ebx], 0x30\n");
	fprintf(arq, " 43" );//fprintf(arq, "%#010x","inc ebx\n");

	//PULO OPCODE INDEFINIDO//
	//fprintf(arq, "%#010x","jmp loop1\n");
    //***********************//

	fprintf(arq, " 31 d2");//fprintf(arq, "%#010x","xor ebx, ebx\n");

	//fprintf(arq, "%#010x","fim:\n");
	fprintf(arq, " 88 93 %#010x", converte_littleendian(numSaidaStringAuxEndereco));//fprintf(arq, "%#010x","mov byte [numSaidaStringAux + ebx], dl\n");
	fprintf(arq, " 80 83 %#010x 30", converte_littleendian(numSaidaStringAuxEndereco));//fprintf(arq, "%#010x","add byte [numSaidaStringAux + ebx], 0x30\n");

	fprintf(arq, " 31 db");//fprintf(arq, "%#010x","xor ebx, ebx\n");
	//fprintf(arq, "%#010x","loop2:\n");

	fprintf(arq, " 8a 81 %#010x", converte_littleendian(numSaidaStringAuxEndereco));//fprintf(arq, "%#010x","mov al, byte [numSaidaStringAux + ecx];\n");
	fprintf(arq, " 88 93 %#010x", converte_littleendian(numSaidaStringEndereco));////fprintf(arq, "%#010x","mov byte [numSaidaString + ebx], al\n");

	fprintf(arq, " 83 f9 00");//fprintf(arq, "%#010x","cmp ecx, 0\n");
	//PULO OPCODE INDEFINIDO
	//fprintf(arq, "%#010x","je fim2\n");
    /********************************/
	fprintf(arq, " 43");//fprintf(arq, "%#010x","inc ebx\n");
	fprintf(arq, " 49");////fprintf(arq, "%#010x","dec ecx\n");

	// PULO OPCODE INDEFINIDO
	//fprintf(arq, "%#010x","jmp loop2\n");
	// *********************************//
	//fprintf(arq, "%#010x","fim2:\n");

	fprintf(arq, " b8 04 00 00 00");////fprintf(arq, "%#010x","mov eax, 4\n");
	fprintf(arq, " bb 01 00 00 00");//fprintf(arq, "%#010x","mov ebx, 1\n");
	fprintf(arq, " b9 %#010x", converte_littleendian(numSaidaStringEndereco));//fprintf(arq, "%#010x","mov ecx, numSaidaString\n");
	fprintf(arq, " ba 0b 00 00 00");//fprintf(arq, "%#010x","mov edx, 11\n");
	fprintf(arq, " cd 80");//fprintf(arq, "%#010x","int 80h\n");

/**Adicionei um enter no fim do número aqui*/
	fprintf(arq, " b8 04 00 00 00");//fprintf (arq, "mov eax, 4\n");
	fprintf(arq, " bb 01 00 00 00");//fprintf (arq, "mov ebx, 1\n");
	fprintf(arq, " b9 %#010x", converte_littleendian(enterString));//fprintf (arq, "mov ecx, _enter_\n");
	fprintf(arq, " ba 02 00 00 00");//fprintf (arq, "mov edx, 2\n");
	fprintf(arq, " cd 80");//fprintf(arq, "%#010x","int 80h\n");
/**/
	fprintf(arq, " 5a");//fprintf(arq, "%#010x","pop edx\n");
	fprintf(arq, " 59");//fprintf(arq, "%#010x","pop ecx\n");
	fprintf(arq, " 5b");//fprintf(arq, "%#010x","pop ebx\n");
	fprintf(arq, " 58");//fprintf(arq, "%#010x","pop eax\n");
	fprintf(arq, " c9");////fprintf(arq, "%#010x","leave\n");
	fprintf(arq, " c3");//fprintf(arq, "%#010x","ret\n");
}

/*Funcao para escrever no dst do assembly ia32 a funcao lerInteiro*/
void escreveFuncaoLerInteiroOp(FILE * arq, int enderecoStringLeitura, int enderecoStringnumEntrada, int enderecoStringDez) {

	//fprintf(arq, "\n\nsection .text");
	fprintf(arq, " c8 00 00 00");//fprintf(arq, "%#010x","enter 0,0\n");
	fprintf(arq, " 50");//fprintf(arq, "%#010x","push eax\n");
	fprintf(arq, " 53");//fprintf(arq, "%#010x","push ebx\n");
	fprintf(arq, " 51");//fprintf(arq, "%#010x","push ecx\n");
	fprintf(arq, " 52");//fprintf(arq, "%#010x","push edx\n");
	fprintf(arq, " 56");//fprintf(arq, "%#010x","\npush esi");

	//fprintf(arq, "%#010x","\n; ler numero");
	fprintf(arq, " b8 03 00 00 00");//fprintf(arq, "%#010x","\nmov eax, 3");
	fprintf(arq, " bb 00 00 00 00");//fprintf(arq, "%#010x","\nmov ebx, 0");
	fprintf(arq, " b9 %#010x", converte_littleendian(enderecoStringLeitura));//fprintf(arq, "%#010x","\nmov ecx, _leitura_");
	fprintf(arq, " ba 0a 00 00 00");//fprintf(arq, "%#010x","\nmov edx, 10");
	fprintf(arq, " cd 80");//fprintf(arq, "%#010x","\nint 80h");liza

	fprintf(arq, " b8 00 00 00");////fprintf(arq, "%#010x","\nmov eax, 0; comecar as posicoes da string");
	fprintf(arq, " c7 05 %#010x", converte_littleendian(enderecoStringnumEntrada));////fprintf(arq, "%#010x","\nmov [numEntrada], DWORD 0; inicializacao do numero de saida");
	fprintf(arq, " be %#010x", converte_littleendian(enderecoStringLeitura));//fprintf(arq, "%#010x","\nmov esi, _leitura_");

	//fprintf(arq, "%#010x","\nloop_soma:");
	fprintf(arq, " 31 db");//fprintf(arq, "%#010x","\nxor ebx, ebx");
	fprintf(arq, " 8a 1e");//fprintf(arq, "%#010x","\nmov bl, byte [esi]");

	fprintf(arq, " 80 fb 0a");//fprintf(arq, "%#010x","\ncmp bl, 10 ; 10 = /n na tabela ASCII");
	// Pulo Opcode Indeterminado
	//fprintf(arq, "%#010x","\nje SAI");
	// *******************************8/


	fprintf(arq, " 80 eb 30");//fprintf(arq, "%#010x","\nsub  bl, 0x30; conserta char");
	fprintf(arq, " a1 %#010x", converte_littleendian(enderecoStringnumEntrada));//fprintf(arq, "%#010x","\nmov eax, dword [numEntrada]");
	fprintf(arq, " f7 25 %#010x", converte_littleendian(enderecoStringDez));//fprintf(arq, "%#010x","\nmul dword [_dez_]");
	fprintf(arq, " A3 %#010x", converte_littleendian(enderecoStringnumEntrada));//fprintf(arq, "%#010x","\nmov dword [numEntrada], eax ");
	fprintf(arq, " 01 1d %#010x", converte_littleendian(enderecoStringnumEntrada));//fprintf(arq, "%#010x","\nadd dword [numEntrada], ebx");

	fprintf(arq, " 46");//fprintf(arq, "%#010x","\ninc esi");

    // Utizando offset ate o loop_soma
	//fprintf(arq, "%#010x","\nloop loop_soma");
    //fprintf(arq, "E2 offset ");//
	//fprintf(arq, "%#010x","\nSAI:");

	fprintf(arq, " 5e");//fprintf(arq, "%#010x","\npop esi");
	fprintf(arq, " 5a");//fprintf(arq, "%#010x","\npop edx");
	fprintf(arq, " 59");//fprintf(arq, "%#010x","\npop ecx");
	fprintf(arq, " 5b");//fprintf(arq, "%#010x","\npop ebx");
	fprintf(arq, " 58");//fprintf(arq, "%#010x","\npop eax");
	fprintf(arq, " c9");//fprintf(arq, "%#010x","\nleave");
	fprintf(arq, " c3");//fprintf(arq, "%#010x","\nret\n");
}

void sintese_codigo (char *in_name, char *out_name, SYMBOL_TABLE *comeco_tabela){

	FILE *in, *dst;

	/*Buffer para a linha*/
	char buffer[linesize];

	/*Tokens separados*/
	char rotulo[tokensize], instr[8], dir[8], opr1[tokensize], opr2[tokensize];
	int mod1, mod2;

	/*Busca na tabela de simbolos*/
	SYMBOL_TABLE *aux, *aux2;

	in = fopen(in_name, "r");
	dst = fopen(out_name, "w+");

	mem_counter_code = mem_counter_data = 0;
	while (get_linha(in, buffer)){

		/*Necessário manter a informação dos dados para fazer os pulos relativos
		Alem disso, o .cod são os códigos de máquina tudo junto, não tem distinçao
		de dados e instruções*/
		mem_counter_code += tam_instr(instr, mod1);
		mem_counter_data += tam_dir(dir, mod1);

		/*Limpa os buffers*/
		*rotulo = *instr = *dir = *opr1 = *opr2 = '\0';
		mod1 = mod2 = 0;

		separa_linha(buffer, rotulo, instr, dir, opr1, &mod1, opr2, &mod2);

		aux = busca_tabela(comeco_tabela, opr1);

		if (!strcmp(instr, "add") || !strcmp(instr, "ADD"))
			traduzADDOp(dst, aux->endereco + mod1);

		else if (!strcmp(instr, "sub") || !strcmp(instr, "SUB"))
			traduzSUBOp(dst, aux->endereco + mod1);

		else if (!strcmp(instr, "mult") || !strcmp(instr, "MULT"))
			traduzMULTOp(dst, aux->endereco + mod1);

		else if (!strcmp(instr, "div") || !strcmp(instr, "DIV"))
			traduzDIVOp(dst, aux->endereco + mod1);

		else if (!strcmp(instr, "jmp") || !strcmp(instr, "JMP"))
			traduzJMPOp(dst, aux->endereco - mem_counter_code + mod1);

		else if (!strcmp(instr, "jmpp") || !strcmp(instr, "JMPP"))
			traduzJMPPOp(dst, aux->endereco - mem_counter_code + mod1);

		else if (!strcmp(instr, "jmpn") || !strcmp(instr, "JMPN"))
			traduzJMPNOp(dst, aux->endereco - mem_counter_code + mod1);

		else if (!strcmp(instr, "jmpz") || !strcmp(instr, "JMPZ"))
			traduzJMPZOp(dst, aux->endereco - mem_counter_code + mod1);

		else if (!strcmp(instr, "copy") || !strcmp(instr, "COPY")) {
			aux2 = busca_tabela(comeco_tabela, opr2);
			traduzCOPYOp(dst, aux->endereco + mod1, aux2->endereco + mod2);
		}

		else if (!strcmp(instr, "load") || !strcmp(instr, "LOAD"))
			traduzLOADOp(dst, aux->endereco + mod1);

		else if (!strcmp(instr, "store") || !strcmp(instr, "STORE"))
			traduzStoreOp(dst, aux->endereco + mod1);

		else if (!strcmp(instr, "input") || !strcmp(instr, "INPUT"))
			traduzINPUTOp(dst, code_size+8, code_size + 2, aux->endereco + mod1);

		else if (!strcmp(instr, "output") || !strcmp(instr, "OUTPUT"))
			traduzOUTPUTOp(dst, code_size+124, code_size+14, aux->endereco + mod1);

		else if (!strcmp(instr, "stop") || !strcmp(instr, "STOP"))
			traduzSTOPOp(dst);

		else if (!strcmp(dir, "const"))
			traduzCONSTOp(dst, mod1);

		else if (!strcmp(dir, "space"))
			traduzSPACEOp(dst, mod1);
	}

	/*Adicionando as bibliotecas ao arquivo executável*/

	if (usa_input || usa_output){
		fprintf (dst, " ah"); 								/*enter					code_size +1*/
		fprintf (dst, " 00");								/*numEntrada			code_size +2*/
		fprintf (dst, " 00 00 00 00 00 00 00 00 00 00 00");	/*_leitura_				code_size +3*/
		fprintf (dst, " 00");								/*numSaida				code_size +14*/
		fprintf (dst, " 10");								/*_dez_					code_size +15*/
		fprintf (dst, " 00 00 00 00 00 00 00 00 00 00 00"); /*numSaidaString		code_size +16*/
		fprintf (dst, " 00 00 00 00 00 00 00 00 00 00 00"); /*numSaidaStringAux		code_size +27*/

		/*essa funçao tem tamanho 86, localizada em code_size + 38*/
		escreveFuncaoLerInteiroOp(dst, code_size+3, code_size+2, code_size+15);

		/*localizada em code_size + 124*/
		escreveFuncaoEscreverInteiroOp(dst ,code_size+14, code_size+15, code_size+27,
										code_size+16, code_size+1);
	}

	fclose(in);
	fclose(dst);
}
