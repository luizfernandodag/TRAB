global _start

section .bss
num: resd 1
numSaidaString2 resb 11
numSaidaString resb 11
numDigitos resd 1
section .data
dez dd 10
zero dd 0

section .text
_start:

mov dword [num], 123456789

call escreverInteiro

mov eax, 4
mov ebx, 1
mov ecx, numSaidaString
mov edx, 11
int 80h




mov eax,1
mov ebx, 0
int 80h


escreverInteiro:
enter 0,0
push eax
push ebx
push ecx
push edx

mov eax, dword [num]
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

mov byte [numSaidaString2 + ebx], dl
add byte [numSaidaString2 + ebx], 0x30
inc ebx


jmp loop1

xor ebx, ebx

fim:
mov byte [numSaidaString2 + ebx], dl
add byte [numSaidaString2 + ebx], 0x30

xor ebx, ebx
loop2:

mov al, byte [numSaidaString2 + ecx];
mov byte [numSaidaString + ebx], al

cmp ecx, 0
je fim2

inc ebx
dec ecx
jmp loop2

fim2:

;mov byte [numSaidaString + ebx], al


inc ebx
mov dword [numDigitos], ebx

pop edx
pop ecx
pop ebx
pop eax
leave 
ret



