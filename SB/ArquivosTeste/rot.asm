
SECTION TEXT

; PROBLEMAS DE ':' E MODIFICADORES
	ROTULO		ADD 	ALPHA		; INVÁLIDO, FALTA O ':'
	ROTULO+1:	ADD		ALPHA		; INVÁLIDO, +1 EH INVÁLIDO PARA OPERAR O RÓTULO
	ROTULO+1	ADD		ALPHA		; INVÁLIDO, +1 EH INVÁLIDO PARA OPERAR O RÓTULO, E FALTA O :

; PROBLEMAS DE ROTULOS REDEFINIDOS E NÃO DEFINIDOS
ALPHA:			ADD		ALPHA
ALPHA:			SUB		ALPHA		; INVÁLIDO, RÓTULO REDEFINIDO!
				ADD		BETA		; INVÁLIDO, RÓTULO NÃO DEFINIDO EM LUGAR NENHUM

; DIVISÃO POR ZERO
				DIV		ZERO				; VÁLIDO, DEIXO DIVIDIR POR ZERO
				JMP		zero				; INVÁLIDO, NÃO PODEMOS PULAR PARA A SEÇÃO DATA
				store	zero				; INVÁLIDO, NÃO PODEMOS ALTERAR UMA CONSTANTE
				ADD 	alpha+1				; INVÁLIDO, SÓ DEIXO USAR VETORES NA SEÇÃO DE DADOS!
				LOAD	VECTOR+39
				LOAD 	VECTOR+40			; INVÁLIDO, 0-39 SAO OS POSSÍVEIS
				LOAD	VECTOR+41			; INVÁLIDO, 0-39 SÃO OS POSSÍVEIS

SECTION DATA

GOL:			CONST	1
ROT_ALEATORIO:	CONST	30
ZERO:			const	0
VECTOR:			SPACE	40