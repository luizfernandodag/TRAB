#include <stdio.h>			/*entrada e saída de dados para o terminal*/
#include <string.h>			/*copiar strings de um lado pro outro*/
#include <stdlib.h>			/*atoi*/
#include <stdbool.h>		/*tipo bool*/

/*Bibliotecas de rede*/
#include <unistd.h>
#include <arpa/inet.h>

#define 	BUFFERSIZE		1024
#define		PORTA_DEFAULT	10101
#define		PORTA_MINIMA	2000	/*li que o S.O. quer pegar as portas menores, nao sei se tenho que limitar aqui*/
#define		LocalHost		"127.0.0.1"
#define		NumConexoes		2
#define		tickTime		1

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
	A priori vou usar o localhost 127.0.0.x com x=1,2,3,...,255*/
    if (argc==3) {
		temp = atoi(argv[2]);
		if (temp <= PORTA_MINIMA) {
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

	char mensagem[1024];

	int listenfd = 0, connfd = 0;

	struct sockaddr_in serv_addr;

	char sendBuff[1025];

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(porta);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	listen(listenfd, 10);

	/*espera um primeiro contato*/
	connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

	/*manda um oi*/
	snprintf(sendBuff, sizeof(sendBuff), "Hello Client\n");
	write(connfd, sendBuff, strlen(sendBuff));

	/*Operação principal, escreve sem parar */
	while(scanf(" %s", mensagem) > 0)
	{
		printf ("mensagem a ser enviada: '%s'\n", mensagem);
		write(connfd, mensagem, strlen(mensagem));
		printf ("mensagem enviada.\n");
		sleep(1);
	}

	close(connfd);
}

void cliente (int porta){
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0',sizeof(recvBuff));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(porta);

    if(inet_pton(AF_INET, LocalHost, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return;
    }

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return;
    }

	while (true) {

		/*leitura*/
		while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0) {
		recvBuff[n] = 0;

		if(fputs(recvBuff, stdout) == EOF)
			printf("\n Error : Fputs error\n");

		/**/
		/*espera um contato*/
		sockfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
		}
	}

    if(n < 0)
    {
        printf("\n Read error \n");
    }
}
