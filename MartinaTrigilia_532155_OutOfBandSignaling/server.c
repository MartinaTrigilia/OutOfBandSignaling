/* 

	PROGETTO SISTEMI OPERATIVI 2019
	Martina Trigilia, 532155
	Server
	
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <signal.h> 
#include <sys/socket.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/un.h> 
#include <errno.h> 
#include <inttypes.h> 
#include <sys/time.h> 
#include <limits.h>
#include <assert.h>
#include "utility.h"

#define UNIX_PATH_MAX 108
#define CLOCK CLOCK_REALTIME

int fdsocket;
int fdpipe;
int indice_server;
char nomeSocket[30];

typedef struct message {
	unsigned int stima;
	uint32_t id_client;
	int server;
}msg;


//Questa funzione serve per convertire i campi di succ prec in millisecondi.
 // Allo stesso tempo fornisce la differenza tra succ e prec 
unsigned int convertiAndSottrai(struct timespec succ, struct timespec prec) {
	return (succ.tv_sec - prec.tv_sec)*1000 + (succ.tv_nsec - prec.tv_nsec)/1000000;
}

/* PROTOTIPI */
void AvvioServer();
void *Comunicazione(void *arg);


int main(int argc, char* argv[]){

	if(argc!=3){
		printf("Numero di argomenti diverso da 3 \n");
		exit(EXIT_FAILURE);
	}
	
	fdpipe = atoi(argv[1]); //file descriptor per comunicare con il supervisor
	indice_server = atoi(argv[2]); 	
	
	printf("SERVER %d ACTIVE\n", indice_server);

	AvvioServer();	
	return 0;
}

void *Comunicazione(void *arg){
	int fd = (int) arg;
	int c;
	uint32_t id=-1;
	msg messaggio;
	unsigned int new_stima, tempo_arrivo;	
	int primo=0;
	struct timespec precedente, ultimo, iniziale;
		
	clock_gettime(CLOCK, &iniziale);
	//inizializzo la stima
	messaggio.stima=UINT_MAX;

	while( (c = read(fd, &id, sizeof(id)) ) > 0){
		// conversione da network a host byte order 
		id = ntohl(id); 
		
		handle_error1(clock_gettime(CLOCK, &ultimo), "Error : Clock\n" );
		
		tempo_arrivo = convertiAndSottrai(ultimo, iniziale);

		//se ho ricevuto il primo messggio posso calcolare la stima
		if(primo){
			new_stima=convertiAndSottrai(ultimo,precedente);
			//conservo la stima più piccole tra quelle ricevute 
			if(new_stima < messaggio.stima)
				messaggio.stima = new_stima;
		}

		precedente = ultimo;
		printf("SERVER %d INCOMING FROM %" PRIX32 " @ %u\n",  indice_server, id, tempo_arrivo);
		primo=1;
	}

	if(c==-1){
		perror("read socket");
		pthread_exit(NULL);
	}
	else if(c==0) {

		messaggio.server=indice_server;
		messaggio.id_client=id;

		printf("SERVER %d CLOSING %"PRIX32" ESTIMATE %u\n", messaggio.server, messaggio.id_client, messaggio.stima);
		handle_error1( write(fdpipe, &messaggio, sizeof(msg)) , "write pipe\n");
	}

	close(fd);
	pthread_exit(NULL);
	
	return 0;
}

void AvvioServer() {

	sprintf(nomeSocket, "OOB-server-%d",indice_server);
	
	if( unlink(nomeSocket)==-1 && errno!=ENOENT ) { 
		printf("unlink\n");
		exit(EXIT_FAILURE);
	}
	
	//ASSEGNAZIONE DI UN INDIRIZZO ALLA SOCKET	
	struct sockaddr_un sa;
	strncpy(sa.sun_path, nomeSocket, UNIX_PATH_MAX);
	sa.sun_family=AF_UNIX;
	//CREAZIONE DELLA SOCKET 
	handle_error1( (fdsocket = socket(AF_UNIX,SOCK_STREAM,0)), "socket\n");
	handle_error1( (bind(fdsocket,(struct sockaddr *)&sa,sizeof(sa))), "bind\n");
	//SETTAGGIO DELLA SOCKET ALL'ASCOLTO 
	handle_error1( listen(fdsocket,SOMAXCONN), "listen\n");
	int newfd;
	//ATTESA DI CONNESSIONI DA PARTE DEI CLIENT	
	while( ( newfd=accept(fdsocket, NULL, 0) ) !=-1) {

		pthread_t tid;
		printf("SERVER %d CONNECT FROM CLIENT\n", indice_server);
		//ritorna 1 in caso di errore
		if( pthread_create( &tid, NULL, &Comunicazione, (void*)newfd ) )
			fprintf( stderr, "impossibile creare thread\n" );
		
		else if( pthread_detach( tid ) ) 
			fprintf( stderr, "errore pthread_detach\n" );			
	}
	
}

