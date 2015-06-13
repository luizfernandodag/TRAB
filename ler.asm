global _start

section .bss
leitura: resb 11
numEntrada: resd 1
digito: resb 1
section .data
string1: db "Digite um numero", 0
tam1 EQU $-string1
CERTO: db "Certo", 0
tam2 EQU $-CERTO

dez dd 10
zero dd 0

section .text
_start:

; escreve pedindo um numero
mov eax, 4
mov ebx, 1
mov ecx, string1
mov edx, tam1
int 80h


call LerInteiro
cmp dword [numEntrada], dword 111
JNE FIM



mov eax, 4
mov ebx, 1
mov ecx, CERTO
mov edx, tam2
int 80h



FIM:
mov eax,1
mov ebx, 0
int 80h


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











