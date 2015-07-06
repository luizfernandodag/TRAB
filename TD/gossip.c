#include <stdio.h>			/*entrada e saída de dados para o terminal*/
#include <string.h>			/*copiar strings de um lado pro outro*/
#include <stdlib.h>			/*atoi*/
#include <stdbool.h>		/*tipo bool*/

/*Bibliotecas de rede*/
#include <unistd.h>
#include <arpa/inet.h>

#define 	BUFFER_SIZE			1024
#define		PORTA_DEFAULT		10101
#define		PORTA_MINIMA		00001						/*li que o S.O. quer pegar as portas menores, nao sei se tenho que limitar aqui*/
#define		PORTA_MAXIMA		20000						/*nao sei se eu tenho que limitar aqui*/
#define		LocalHost			"127.0.0.1"
#define		NumConexoes			2
#define		tickTime			1
#define		TIMEOUT				10

/*Comportamentos de cliente e servidor*/
int wait_contact (int request_socket, int timeout_time);
void serv_behaviour(int argc, char *argv[]);
void client_behaviour(int argc, char *argv[]);

/*Funções mistas*/
bool valida_cli (int argc, char **argv);
bool eh_servidor (char *string);
int porta_da_fofoca (int argc, char *string);
void relata_erros (int codigo);

int main(int argc, char *argv[]) {

    int request_socket, fofoca_socket;
    char emissor_Buffer[BUFFER_SIZE], receiver_Buffer[BUFFER_SIZE];
    struct sockaddr_in fofocador_serv, fofocador_cliente;
    int msg_size;

    /*Se a cli não passar o teste de validade, nem execute*/
	if (!valida_cli(argc, argv))
		return 0;

    /*Se for servidor, funcione como servidor*/
    if (eh_servidor(argv[1])){

		printf ("Eu sou o servidor.\n");

		/*Abre o socket para possibilitar a comunicação 1 para 1*/
		if ((request_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("Nao foi possivel criar o soquete.\n");
			return 0;
		}

		/*Definindo os parâmetros do servidor*/
		fofocador_serv.sin_family = AF_INET;
		fofocador_serv.sin_port = htons(porta_da_fofoca(argc, argv[2]));
		fofocador_serv.sin_addr.s_addr = inet_addr(LocalHost);
		bzero(&(fofocador_serv.sin_zero),8);

		/*Pegando a porta*/
		bind(request_socket, (struct sockaddr *)&fofocador_serv, sizeof(struct sockaddr));
		listen(request_socket, NumConexoes);

		/*Espera um sinal de vida*/
		do {
			sleep(tickTime);
			fofoca_socket = accept(request_socket, (struct sockaddr*)NULL, NULL);
		}
		while (fofoca_socket <= 0);

		/*Recebi um sinal de vida, manda o oi*/
		strcpy (emissor_Buffer, "HELLO CLT\n\0");
		write(fofoca_socket, emissor_Buffer, strlen(emissor_Buffer));

		/*Operação principal*/
		for (; true; sleep(tickTime)) {

			/*Tenta ler alguma coisa*/
			msg_size = read(fofoca_socket, receiver_Buffer, sizeof(receiver_Buffer)-1);

			/*Se recebi uma mensagem, escreva-a no terminal*/
			if (msg_size > 0) {
				receiver_Buffer[msg_size] = 0;
				puts(receiver_Buffer);

				/*Fim da fofoca*/
				strcpy(emissor_Buffer, "BYE CLT\n\0");
				write(fofoca_socket, emissor_Buffer, strlen(emissor_Buffer));
				close(fofoca_socket);
				return 0;
			}

		}
	}

	/*Se não eh servidor, eh cliente*/
	else {

		/*Abre o socket para possibilitar a comunicação 1 para 1*/
		if((fofoca_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("Nao foi possivel criar o soquete.\n");
			return 0;
		}

		/*Definindo os parâmetros do cliente*/
		fofocador_cliente.sin_family = AF_INET;
		fofocador_cliente.sin_port = htons(porta_da_fofoca(argc, argv[2]));
		fofocador_cliente.sin_addr.s_addr = inet_addr(LocalHost);
		bzero(&(fofocador_cliente.sin_zero),8);

		/*Tentando conexão*/
		if (connect(fofoca_socket, (struct sockaddr *) & fofocador_cliente, sizeof(fofocador_cliente)) < 0 ){
			printf ("Falha de conexao.\n");
			printf ("Tem algum servidor disponivel ai?\n");
			return 0;
		}

		/*Consegui conexao, manda um oi*/
		strcpy(emissor_Buffer, "HELLO SRV\n\0");
		write(fofoca_socket, emissor_Buffer, strlen(emissor_Buffer));

		/*Vê se o servidor fala algo*/
		for (; true ; sleep(tickTime)) {

			/*Tenta ler alguma coisa*/
			msg_size = read(fofoca_socket, receiver_Buffer, sizeof(receiver_Buffer)-1);

			/*Recebi mensagem*/
			if (msg_size >=0) {

				/*Se recebi uma mensagem, escreva no terminal*/
				puts(receiver_Buffer);

				/*Fim da fofoca*/
				strcpy(emissor_Buffer, "BYE SRV\n\0");
				write(fofoca_socket, emissor_Buffer, strlen(emissor_Buffer));
				close(fofoca_socket);
				return 0;
			}
		}
	}

    return 0;
}

/*Validação da cli: command line interface. True se tudo certo e False se deu algum problema*/
bool valida_cli (int argc, char **argv) {
	int temp;

	/*Problemas de quantidade de argumento*/
    if(argc<2 || argc>3) {
		relata_erros(1);
        return false;
    }

    /*O primeiro argumento tem que ser ou 'host' ou 'S', se não for reclame*/
    if ( strcmp(argv[1],"host") && strcmp(argv[1],"-S") && strcmp(argv[1], "-s") ){
		relata_erros(2);
		return false;
    }

	/*O segundo argumento eh opcional e não faço idéia do range disso...
	A priori vou usar o localhost 127.0.0.x com x=1,2,3,...,255*/
    if (argc==3) {
		temp = atoi(argv[2]);
		if (temp < PORTA_MINIMA || temp > PORTA_MAXIMA) {
			relata_erros(3);
			return false;
		}
    }

	return true;
}

/*Informa se pela CLI foi informado se essa aplicação será cliente ou servidor*/
bool eh_servidor (char *string) {
	return !strcmp(string, "-S");
}

/*Retorna a porta informada pela CLI*/
int porta_da_fofoca (int argc, char *string) {

	if (argc == 2)
		return PORTA_DEFAULT;

	return atoi(string);
}

void relata_erros (int codigo) {

	switch (codigo){

		case 1:
			printf ("O programa funciona com um/dois argumentos:\n");
			printf ("-> '-S' ou 'host' definindo servidor/cliente [obrigatorio]\n");
			printf ("-> port number [opcional]\n");
			break;

		case 2:
			printf ("O primeiro argumento tem que ser '-S' ou host.\n");
			break;

		case 3:
			printf ("A porta de acesso tem que ser um numero positivo nao nulo. Pelo menos eu acho.\n");
			break;
	}
}
