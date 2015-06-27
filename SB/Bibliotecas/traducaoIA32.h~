
void traduzADD(FILE *dst, char *arg1, int numArgs) {

	if(numArgs == 1)
		fprintf(dst,"\nADD EAX, [%s]", args[0] );

	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst, "\nADD EAX, DWORD [%s + %d]", args[0], 4*offset );

	}
}

void traduzSUB(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
		fprintf(dst,"\nSUB EAX,  DWORD [%s]", args[0] );
	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst, "\nSUB EAX, DWORD [%s + %d]", args[0], 4*offset );

	}
}

void traduzMULT(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
	{
		fprintf(dst,"\nMUL DWORD [%s]", args[0] );

	}
	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst, "\nMUL DWORD [%s + %d]", args[0], 4*offset );

	}
}

void traduzDIV(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
	{
<<<<<<< HEAD:SB/traducao.c
		fprintf(arquivo,"\nCDQ [%s]", args[0] );//CDQ extende o sinal de EAX para EDX
		fprintf(arquivo,"\nDIV DWORD [%s]", args[0] );
=======
		fprintf(dst,"\nDIV DWORD [%s]", args[0] );
>>>>>>> c9a3d73edd1e315786ebb5e7bd7014e253d4c4d2:SB/Bibliotecas/traducaoIA32.h

	}
	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst, "\nDIV DWORD [%s + %d]", args[0], 4*offset );

	}
}

void traduzJMP(FILE *dst, char *arg1, int numArgs) {

	if(numArgs == 1)
	{
		fprintf(dst,"\nJMP %s ", args[0] );

	}
}

void traduzJMPN(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
	{
		fprintf(dst,"\nCMP EAX, 0" );
		fprintf(dst,"\nJB %s ", args[0] );/*JMP LABEL*/

	}
}

void traduzJMPP(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
	{
		fprintf(dst,"\nCMP EAX, 0" );
		fprintf(dst,"\nJA %s ", args[0] );/*JMP LABEL*/

	}
}

void traduzJMPZ(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
	{
		fprintf(dst,"\nCMP EAX, 0" );
		fprintf(dst,"\nJE %s ", args[0] );/*JMP LABEL*/

	}
}

void traduzCOPY(FILE *dst, char *arg1, int numArgs) {
	/*
	a funcao copy deve ter pelo menos 2 argumentos
	ARG1 = args[0]
	OFFSET1 = args[1]
	ARG2 = args[2]
	OFFSET2 = args[3]
	caso qualquer um dos offset nao sejam explicitados, OFFSET = "0"
	*/


	if(numArgs == 4) {
		int offset2 = (int)strtol(args[3], NULL, 0);
		int offset1 = (int)strtol(args[1], NULL, 0);
		fprintf(dst,"\nPUSH EBX");
		fprintf(dst,"\nMOV EBX, DWORD [%s + %d]", args[2], 4*offset2);
		fprintf(dst,"\nMOV DWORD [%s + %d],  EBX ", args[0], 4*offset1 );
		fprintf(dst,"\nPOP EBX");
	}

}

void traduzLOAD(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
	{
		fprintf(dst,"\nMOV EAX, [%s]", args[0] );

	}
	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst, "\nMOV EAX, DWORD [%s + %d]", args[0], 4*offset );

	}
}

void traduzSTORE(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
	{
		fprintf(dst,"\nMOV DWORD [%s], EAX", args[0] );

	}
	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst, "\nMOV DWORD [%s + %d], EAX", args[0], 4*offset );

	}
}

void traduzINPUT(FILE *dst, char *arg1, int numArgs) {/*
	Para essa funcao funcionar, eh necessario que o procedimento lerInteiro
	e as variaveis
					section .bss
					leitura: resb 11
					numEntrada: resd 1

					section .data
					dez dd 10
	tenham sido definidas
	*/
	if(numArgs == 1)
	{
		fprintf(dst,"\nCALL lerInteiro" );
		fprintf(dst,"\nPUSH  EDX" );
		fprintf(dst,"\nMOV EDX, DWORD [numEntrada]" );
		fprintf(dst,"\nMOV DWORD [%s], EDX", args[0] );
		fprintf(dst,"\nPOP EDX" );


	}
	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst,"\nCALL lerInteiro" );
		fprintf(dst,"\nPUSH  EDX" );
		fprintf(dst,"\nMOV EDX, DWORD [numEntrada]" );
		fprintf(dst,"\nMOV DWORD [%s + %d], EDX", args[0], 4*offset );
		fprintf(dst,"\nPOP EDX" );

	}
}

void traduzOUTPUT(FILE *dst, char *arg1, int numArgs) {
	/*Para essa funcao funcionar, eh necessario que o procedimento escreverInteiro
	e as variaveis
					section .bss
					numSaida: resd 1
					numSaidaStringAux: resb 11
					numSaidaString resb 11

					section .data
					dez dd 10
	tenham sido definidas
	*/
	if(numArgs == 1)
	{
		fprintf(dst,"\nPUSH  EDX" );
		fprintf(dst,"\nMOV EDX, DWORD [%s]",args[0] );
		fprintf(dst,"\nMOV DWORD [numSaida], EDX" );
		fprintf(dst,"\nCALL escreverInteiro" );
		fprintf(dst,"\nPOP EDX" );


	}
	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst,"\nPUSH  EDX" );
		fprintf(dst,"\nMOV EDX, DWORD [%s + %d]",args[0], 4*offset);
		fprintf(dst,"\nMOV DWORD [numSaida], EDX" );
		fprintf(dst,"\nCALL escreverInteiro" );
		fprintf(dst,"\nPOP EDX" );
	}
}

void traduzCONST(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst, "\n%s dd %d", args[0], offset );


	}
}

void traduzSPACE(FILE *dst, char *arg1, int numArgs) {
	if(numArgs == 1)
	{
		fprintf(dst, "\n%s RESD 1", args[0] );

	}
	else if(numArgs == 2)
	{
		int offset =  (int)strtol(args[1], NULL, 0);
		fprintf(dst, "\n%s RESD %d", args[0], offset );


	}
}

void escreveFuncaoEscreverInteiro(FILE * arq) {
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
fprintf(arq, "%s","div dword [dez]\n");
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



fprintf(arq, "%s","pop edx\n");
fprintf(arq, "%s","pop ecx\n");
fprintf(arq, "%s","pop ebx\n");
fprintf(arq, "%s","pop eax\n");
fprintf(arq, "%s","leave\n");
fprintf(arq, "%s","ret\n");


}

/*Funcao para escrever no dst do assembly ia32 a funcao lerInteiro*/
void escreveFuncaoLerInteiro(FILE * arq) {

fprintf(arq, "%s","\nLerInteiro:\n");
fprintf(arq, "%s","\nenter 0, 0");
fprintf(arq, "%s","\npush eax");
fprintf(arq, "%s","\npush ebx");
fprintf(arq, "%s","\npush ecx");
fprintf(arq, "%s","\npush edx");
fprintf(arq, "%s","\npush esi");

fprintf(arq, "%s","\n; ler numero");
fprintf(arq, "%s","\nmov eax, 3");
fprintf(arq, "%s","\nmov ebx, 0");
fprintf(arq, "%s","\nmov ecx, leitura");
fprintf(arq, "%s","\nmov edx, 10");
fprintf(arq, "%s","\nint 80h");


fprintf(arq, "%s","\nmov eax, 0; comecar as posicoes da string");
fprintf(arq, "%s","\nmov [numEntrada], DWORD 0; inicializacao do numero de saida");
fprintf(arq, "%s","\nmov esi, leitura");


fprintf(arq, "%s","\nloop_soma:");
fprintf(arq, "%s","\nxor ebx, ebx");
fprintf(arq, "%s","\nmov bl, byte [esi]");

fprintf(arq, "%s","\ncmp bl, 10 ; 10 = /n na tabela ASCII");
fprintf(arq, "%s","\nje SAI");
fprintf(arq, "%s","\nsub  bl, 0x30; conserta char");
fprintf(arq, "%s","\nmov eax, dword [numEntrada]");
fprintf(arq, "%s","\nmul dword [dez]");
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
