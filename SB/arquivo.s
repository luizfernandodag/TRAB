global _start

section text

_start:

MOV EAX, [b + 0]

MUL DWORD [h + 0]

DIV DWORD [dois + 0]

MOV DWORD [r], EAX

MOV EAX, 1
MOV EBX, 0
int 80h

section data

b dd 0
h dd 0
r dd 0
dois: dd 2
