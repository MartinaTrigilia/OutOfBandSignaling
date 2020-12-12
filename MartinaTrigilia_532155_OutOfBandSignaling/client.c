/* 

	PROGETTO SISTEMI OPERATIVI 2019
	Martina Trigilia, 532155
	Client
	
*/


#define _POSIX_C_SOURCE 200809L 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "utility.h"

void contattaServer();
uint32_t id_client;
int secret;
/* Parametri in input */
int k;
int p;
int w; 


//Ritorna 1 nel caso l'elemento che si vuole inserire in srvScelti è già presente, 0 altrimenti 
int check (int i, int *vect , int x ) {
	int j=0;
	int trovato=0;
	
	while (!trovato && j<i) {
		if (vect[j] == x)
			trovato=1;
		j++;
	}
	
	return trovato;
}


int main(int argc, char **argv) {
	
	if( argc!=4 ) {
		printf("Usage : p, k, w\n");
		exit(EXIT_FAILURE);
	}
	
	p=atoi(argv[1]);
	k=atoi(argv[2]);
	w=atoi(argv[3]);
	
	if(p > k || (3 * p) >= w || p < 1) {
		printf("Usage: 1 ≤ p <k e w > 3p\n");
		exit(EXIT_FAILURE);
	}
	
	//Inizializzo il generatore di numeri casuali in modo da generare id univoci
	struct timeval t;
	gettimeofday( &t, NULL );
	srand((getpid())*(t.tv_usec + t.tv_sec)); 
 
	//genero un numero esadecimale di 32 bit
	id_client= rand() ^ rand()<<1;

	//genero numeri compresi tra 1 e 3000
	secret = (rand()%(3000))+1;

	fprintf(stdout , "CLIENT %"PRIX32" SECRET %d\n" , id_client , secret );

	int srvScelti[p]; // vettore contenente gli interi scelti corrispondenti ai server
	for (int i=0; i<p ; i++ ) {
		srvScelti[i] = (rand() % k) + 1;
		
		while (check(i, srvScelti,srvScelti[i]))
			srvScelti[i] = (rand() % k) + 1;
	}

	contattaServer(p,w,srvScelti);
	return 0;
}


void contattaServer(int p, int w, int *srvScelti) {
		
		int *sock;
		handle_errorNULL( (sock=malloc(sizeof(int)*p)), "Error: Malloc\n"); ;
		
		//il client si connette a p server
		for( int i = 0 ; i < p; i++ ) {
			struct sockaddr_un sa;
			sa.sun_family = AF_UNIX;
			sprintf(sa.sun_path, "OOB-server-%d", srvScelti[i]);
			handle_error1( (sock[i]=socket(AF_UNIX, SOCK_STREAM, 0)), "socket" );
		
			//riprovo finchè il socket non viene creato
			while( (connect(sock[i], (struct sockaddr *)&sa, sizeof(sa)))==-1 ) {
				sleep(1 );
			}
			
		}

		//converto l'id del client da host byte order a network byte order
		uint32_t id_nbo = htonl(id_client);
 		int j;
		struct timespec req, rem;
		req.tv_sec = secret/1000; 
		req.tv_nsec = (secret % 1000) * 1000000;
		for(int i = 0 ; i < w; i++) {
			handle_error1( nanosleep( &req, &rem ), "nanosleep" );
			j=rand()%p;
			handle_error1( write(sock[j], &id_nbo, sizeof(uint32_t)), "write socket");
		}

	fprintf(stdout,"CLIENT %" PRIX32 " DONE\n", id_client);
}
