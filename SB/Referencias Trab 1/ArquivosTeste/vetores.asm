
; VOU TESTAR POSSÍVEIS PROBLEMAS COM VETORES.
; ASSUMO QUE PELO MENOS A INSTRUÇÃO ADD ESTEJA FUNCIONANDO

SECTION TEXT

;PRIMEIRO VOU BOTAR AS COISAS QUE TEM QUE SER VÁLIDAS DO PONTO DE VISTA DA CONSTRUÇÃO
		ADD		ROT
		ADD		ROT+1
		ADD		ROT-1
		ADD		ROT+10
		ADD		ROT-10

;AGORA EU VOU TESTAR PARA VERIFICAR SE ELE DÁ ERRO LEXICO SE TENTAR USAR ALGUM OPERADOR QUE NÃO DEVO
		ADD		ROT*1
		ADD		ROT/1
		ADD		ROT&1
		ADD		ROT$1

;AGORA VOU VERIFICAR POSSÍVEIS PROBLEMAS COM A ESCRITA DESSE MODIFICADOR
		ADD		ROT +1	; VÁLIDO? NO ROTEIRO NÃO FALA SE EU DEVO ACEITAR SOMENTE SE NÃO TIVER ESPAÇOS
		ADD		ROT + 1 ; VÁLIDO? NO ROTEIRO NÃO FALA SE EU DEVO ACEITAR SOMENTE SE NÃO TIVER ESPAÇOS

;AGORA VOU VERIFICAR A INTELIGENCIA DO PROGRAMADOR
		ADD		ROT+0	; VÁLIDO? EH BURRICE INOFENSIVA, MAS NO ROTEIRO NÃO FALA PRA PROIBIR.

SECTION DATA
		SPACE	20
ROT:	SPACE	20
