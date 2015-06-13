global _start

section .bss
numSaida: resd 1
numSaidaStringAux resb 11
numSaidaString resb 11
section .data
dez dd 10

section .text
_start:

mov dword [numSaida], 7777

call escreverInteiro

  
mov eax,1
mov ebx, 0
int 80h


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



