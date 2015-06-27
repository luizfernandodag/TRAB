section data
numEntrada:			dd "0"
numSaida:			dd "0"
leitura: 			dd "00000000000"
numSaidaStringAux: 	dd "00000000000"
numSaidaString:		dd "00000000000"
digito: 		dd "0"
dez:			dd 10
zero:			dd 0

var:			dd "1234"
var2:			dd "5678"

section text

global _start
_start:

;Quatro operações fundamentais
	add		EAX, [var+1]
	sub		EAX, [var+1]
	mul		dword [var+1]
	div		dword [var+1]

;Pulos
	jmp		ETIQUETA4

	cmp		EAX, 0
	jb		ETIQUETA4

	cmp		EAX, 0
	ja		ETIQUETA4

	cmp		EAX, 0
	je		ETIQUETA4

ETIQUETA4:

;Copy
	PUSH EBX
	MOV EBX, DWORD [var2]
	MOV DWORD [var], EBX
	POP EBX

;Load
	MOV EAX, [var + 1]

; STORE
	MOV [var + 1], EAX

;Input
	CALL LerInteiro
	PUSH  EDX
	MOV EDX, DWORD [numEntrada]
	MOV DWORD [var + 1], EDX
	POP EDX

;Output
	PUSH EDX
	MOV EDX, DWORD [var + 1]
	MOV DWORD [numSaida], EDX
	CALL escreverInteiro
	POP EDX

;Stop
	mov		eax, 1
	mov 	ebx, 0
	int 80h

; ------------------------------------ FUNÇÃO DE LEITURA DE NÚMERO -------------------------------------------
LerInteiro:
enter 0, 0
push eax
push ebx
push ecx
push edx
push esi

; ler numero
mov eax, 3
mov ebx, 0
mov ecx, leitura
mov edx, 10
int 80h


mov eax, 0; comecar as posicoes da string
mov [numEntrada], DWORD 0; inicializacao do numero de saida
mov esi, leitura


loop_soma:
xor ebx, ebx
mov bl, byte [esi]

cmp bl, 10 ; 10 = /n na tabela ASCII
je SAI
sub  bl, 0x30; conserta char
mov eax, dword [numEntrada]
mul dword [dez]
mov dword [numEntrada], eax
add dword [numEntrada], ebx


inc esi

loop loop_soma


SAI:

pop esi
pop edx
pop ecx
pop ebx
pop eax
leave
ret

; ------------------------------------ FUNÇÃO DE ESCRITA DE NÚMERO ------------------------------------------------------------------
escreverInteiro:
enter 0,0
push eax
push ebx
push ecx
push edx

mov eax, dword [numSaida]
;mov dword [ebp-4], eax


xor ecx, ecx
xor edx, edx
xor ebx, ebx

loop1:
inc ecx
xor edx, edx
div dword [dez]
cmp eax, 0
jle fim

mov byte [numSaidaStringAux + ebx], dl
add byte [numSaidaStringAux + ebx], 0x30
inc ebx


jmp loop1

xor ebx, ebx

fim:
mov byte [numSaidaStringAux + ebx], dl
add byte [numSaidaStringAux + ebx], 0x30

xor ebx, ebx
loop2:

mov al, byte [numSaidaStringAux + ecx];
mov byte [numSaidaString + ebx], al

cmp ecx, 0
je fim2

inc ebx
dec ecx
jmp loop2

fim2:

;mov byte [numSaidaString + ebx], al

mov eax, 4
mov ebx, 1
mov ecx, numSaidaString
mov edx, 11
int 80h



pop edx
pop ecx
pop ebx
pop eax
leave
ret
