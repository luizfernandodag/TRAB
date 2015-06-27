global _start

section text

CALL lerInteiro
PUSH  EDX
MOV EDX, DWORD [numEntrada]
MOV DWORD [b + 0], EDX
POP EDX

CALL lerInteiro
PUSH  EDX
MOV EDX, DWORD [numEntrada]
MOV DWORD [h + 0], EDX
POP EDX

MOV EAX, [b + 0]

MUL DWORD [h + 0]

DIV DWORD [dois + 0]

MOV DWORD [r + 0], EAX

CALL lerInteiro
PUSH  EDX
MOV EDX, DWORD [numEntrada]
MOV DWORD [r + 0], EDX
POP EDX

MOV EAX, 1
MOV EBX, 0
int 80h

section data

 dd 0

 dd 0

 dd 0

: dd 2


section code
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


section code
escreverInteiro:
enter 0,0
push eax
push ebx
push ecx
push edx
mov eax, dword [numSaida]
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
