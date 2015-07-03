
section text
	;add			quatro
	;sub			quatro
	;mult			quatro
	;div			quatro
	;jmp			pindamonhangaba
	;jmpp			pindamonhangaba
	;jmpn			pindamonhangaba
	;jmpz			pindamonhangaba
	copy			ghost, dois
	;load			quatro
	;store			ghost
	;input			quatro
	;output			quatro
	;stop

fim_instr:

	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois
	add			dois

pindamonhangaba:
	stop

section data
quatro: const 0x4
ghost:	space 70
dois:	const 0x2

