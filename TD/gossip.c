#include <stdio.h>			/*entrada e saída de dados para o terminal*/
#include <string.h>			/*copiar strings de um lado pro outro*/
#include <stdlib.h>			/*atoi*/
#include <stdbool.h>			/*tipo bool*/
#include "Getch.c"			/*timed getchar(), se não responder antes do timeout retorna EOF */

/*Bibliotecas de rede*/
#include <unistd.h>
#include <arpa/inet.h>

/*Um bocado que nem sei pra que*/
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>

#define 	BUFFERSIZE	1024
#define		PORTA_DEFAULT	10101
#define		PORTA_MINIMA	1000		/*li que o S.O. quer pegar as portas menores, nao sei se tenho que limitar aqui*/
#define		PORTA_MAXIMA	65000
#define		LocalHost	"127.0.0.1"
#define		NumConexoes	2
#define		tickTime	1

/*Comportamentos de cliente e servidor*/
void servidor (int porta);
void cliente (int porta);

/*Funções mistas*/
bool valida_cli (int argc, char **argv);
bool eh_servidor (char *string);
int porta_da_fofoca (int argc, char *string);
void relata_erros (int codigo);
struct sockaddr_in caracteristicas_endereco (int porta);

int main(int argc, char *argv[]) {

    /*Se a cli não passar o teste de validade, nem execute*/
	if (!valida_cli(argc, argv))
		return 0;

	if (eh_servidor(argv[1])){
		printf ("sou servidor na porta %d\n", porta_da_fofoca(argc, argv[2]));

		servidor(porta_da_fofoca(argc, argv[2]));
	}
	else {
		printf ("sou cliente na porta %d\n", porta_da_fofoca(argc, argv[2]));

		cliente(porta_da_fofoca(argc, argv[2]));
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
	A priori vou usar ports de 1000 ateh 65k*/
    if (argc==3) {
		temp = atoi(argv[2]);
		if (temp <= PORTA_MINIMA || temp >= PORTA_MAXIMA) {
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

		default:
			;
	}
}

struct sockaddr_in caracteristicas_endereco (int porta) {

	struct sockaddr_in a;

	a.sin_family = AF_INET;
    a.sin_port = htons(porta);
    a.sin_addr.s_addr = inet_addr(LocalHost);

	return a;
}

void servidor (int porta) {

	int listenfd = 0, soquete = 0, i=0, n=0;
	char sendBuffer[1024], receiveBuffer[1024], c;
	struct sockaddr_in serv_addr;

	/*inicialização de memória*/
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuffer, '0', sizeof(sendBuffer));

	/*abre o socket*/
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(porta);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	listen(listenfd, 10);

	/*espera um primeiro contato*/
	soquete = accept(listenfd, (struct sockaddr*)NULL, NULL);

	/*evita o bloqueio do socket*/
	if ((n = fcntl (soquete, F_GETFL)) < 0 || fcntl (soquete, F_SETFL, n | O_NONBLOCK) < 0) {
		printf ("Problemas no desbloqueio do meio\n.");
		return;
	}

	/*Operação principal, escreve sem parar */

	while (1) {

		/*tenta pegar um caractere do usuário*/
		if ( (c=tgetche(1)) != EOF ){
			sendBuffer[i++] = c;

			/*suporte à backspace*/
			if (c == '\b')
				sendBuffer[--i] = '\0';

			/*enter envia a mensagem*/
			if (c == '\n'){
				/*envia a mensagem*/
				sendBuffer[i] = '\0';
				write(soquete, sendBuffer, strlen(sendBuffer)+1);
				sleep(1);

				/*prepara para um novo envio*/
				memset(sendBuffer, 0,sizeof(sendBuffer));
				i=0;
			}
		}

		/*olha se tem algo pra ler no buffer*/
		if (n=read(soquete, receiveBuffer, sizeof(receiveBuffer)) > 0) {
			printf ("> ");
			printf ("%s", receiveBuffer);

			/*se receber o oi, responda*/
			if (!strcmp("HELLO SRV\n", receiveBuffer)){
				strcpy(sendBuffer, "HELLO CLT\n\0");
				write(soquete, sendBuffer, strlen(sendBuffer)+1);
			}

			/*se receber o tchau, feche*/
			if (!strcmp("BYE SRV\n", receiveBuffer)){
				close(soquete);
				return;
			}
		}
	}
}

void cliente (int porta){
    int soquete = 0, n = 0;
    char receiveBuffer[1024], sendBuffer[1024], c, i=0;
    struct sockaddr_in serv_addr;

    memset(receiveBuffer, '\0',sizeof(receiveBuffer));

    if((soquete = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(porta);

    if(inet_pton(AF_INET, LocalHost, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return;
    }

	if( connect(soquete, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Error : Connect Failed \n");
		return;
	}

	/*evita o bloqueio do socket*/
	if ((n = fcntl (soquete, F_GETFL)) < 0 || fcntl (soquete, F_SETFL, n | O_NONBLOCK) < 0) {
		printf ("Problemas no desbloqueio do meio\n.");
		return;
	}

	/*manda um oi*/
	strcpy(sendBuffer, "HELLO SRV\n\0");
	write(soquete, sendBuffer, 1+strlen(sendBuffer));

	while (true) {

		/*tenta pegar um caractere do usuário*/
		if ( (c=tgetche(1)) != EOF ){
			sendBuffer[i++] = c;

			/*suporte à backspace*/
			if (c == '\b')
				sendBuffer[i--] = '\0';

			/*enter envia a mensagem*/
			if (c == '\n'){
				/*envia a mensagem*/
				sendBuffer[i] = '\0';
				write(soquete, sendBuffer, 1+strlen(sendBuffer));
				sleep(1);

				/*prepara para um novo envio*/
				memset(sendBuffer, 0,sizeof(sendBuffer));
				i=0;
			}
		}

		/*olha se tem algo pra ler no buffer*/
		if (n=read(soquete, receiveBuffer, sizeof(receiveBuffer)) > 0) {
			printf ("> ");
			printf ("%s", receiveBuffer);

			/*se receber o tchau, é hora de dar tchau*/
			if (!strcmp("BYE CLT\n", receiveBuffer)){
				strcpy(sendBuffer, "BYE SRV\n\0");
				write(soquete, sendBuffer, 1+strlen(sendBuffer));
				return;
			}
		}
	}
}
