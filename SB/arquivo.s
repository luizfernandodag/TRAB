global _start
section .text
_start:
PUSH EBX
MOV EBX, DWORD [dois + 0]
MOV DWORD [ghost + 0],  EBX 
POP EBX

fim_instr: 
ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

ADD EAX, DWORD [dois + 0]

pindamonhangaba: 
MOV EAX, 1
MOV EBX, 0
int 80h

section .data

quatro: dd 4
ghost dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
dois: dd 2