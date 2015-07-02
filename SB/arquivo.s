global _start
section .text
_start:
ADD EAX, DWORD [quatro + 0]

SUB EAX, DWORD [quatro + 0]

MUL DWORD [quatro + 0]

DIV DWORD [quatro + 0]

JMP meio

CMP EAX, 0
JA meio

CMP EAX, 0
JB meio

CMP EAX, 0
JE meio

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

ADD EAX, DWORD [quatro + 0]

meio: 
MOV EAX, 1
MOV EBX, 0
int 80h

section .data

ghost dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
dois: dd 2
quatro: dd 4