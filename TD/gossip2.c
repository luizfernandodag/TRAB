#include <stdio.h>			/*entrada e saída de dados para o terminal*/
#include <string.h>			/*copiar strings de um lado pro outro*/
#include <stdlib.h>			/*atoi*/
#include <stdbool.h>		/*tipo bool*/

/*Bibliotecas de rede*/
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

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
int sockfd, newsockfd, portno, clilen;
   char buffer[256], bufferw[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0)
      {
      perror("ERROR opening socket");
      exit(1);
      }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = porta;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }
      
   /* Now start listening for the clients, here process will
   * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);

   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
   if (newsockfd < 0)
      {
      perror("ERROR on accept");
      exit(1);
      }



   while(1)
   {
   printf("Please enter the message: ");
   bzero(buffer,256);
   fgets(buffer,255,stdin);
   
   /* Send message to the server */
    n = write(newsockfd, buffer, strlen(buffer));
   
   if (n < 0)
   {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   bzero(buffer,256);
   n = read(newsockfd, buffer, 255);
   
   if (n < 0)
   {
      perror("ERROR reading from socket");
      exit(1);
   }
   printf("%s\n",buffer);
   }
	
}

void cliente (int porta){
    
  int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   char buffer[256];
   
   portno = porta;
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0)
   {
      perror("ERROR opening socket");
      exit(1);
   }
   server = gethostbyname("localHost");
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR connecting");
      exit(1);
   }
   
   /* Now ask for a message from the user, this message
   * will be read by server
   */
   while(1)
   {
   printf("Please enter the message: ");
   bzero(buffer,256);
   fgets(buffer,255,stdin);
   
   /* Send message to the server */
    n = write(sockfd, buffer, strlen(buffer));
   
   if (n < 0)
   {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);
   
   if (n < 0)
   {
      perror("ERROR reading from socket");
      exit(1);
   }
   printf("%s\n",buffer);
}


}
