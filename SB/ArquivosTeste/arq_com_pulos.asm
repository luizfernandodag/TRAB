; Recebe um numero números
; se for maior que 5, mostra 2
; se for menor que 5, mostra 4 (mas pera, -1 = 11...111 em complemento de dois e interpretando sem sinal eh um numero positivo grande pakas)
; se for igual a 5, mostra 8
section text

	input a

	load a
	sub cinco
	jmpp POSITIVO
	jmpz NEUTRO
	jmpn NEGATIVO

POSITIVO:
	output dois
	stop

NEGATIVO:
	output quatro
	stop

NEUTRO:
	output oito
	stop

section data
	a:	space
	cinco: const 5
	dois: const 2
	quatro: const 4
	oito: const 8
