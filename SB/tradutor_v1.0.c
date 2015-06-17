#include "bib.h"
#include "preproc.h"
#include "parser.h"
#include "scanner.h"

int main (int argc,char *argv[]) {

	FILE *entrada=NULL, *saida=NULL;
	SYMBOL_TABLE *comeco_tabela=NULL;

	/** Para o programa final a gente usa essas funções, para testar direto no codeblocks eh soh usar as linhas abaixo
	if (!valida_linhacomando(argc))
		return 0;

		pre_processamento(entrada);

		analise(entrada, comeco_tabela);

		sintese (codigo máquina inventado)

		sintese (codigo IA32)

		sintese ELF

	*/

	/*entrada = fopen("ArquivosTeste/dir.asm", "r");*/
	pre_processamento("ArquivosTeste/equ.asm");

	//remove(PREPROCESSED_FILE);
	return 0;
}
