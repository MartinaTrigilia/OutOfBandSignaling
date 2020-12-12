/* 

	PROGETTO SISTEMI OPERATIVI 2019
	Martina Trigilia, 532155
	Supervisor
	
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <errno.h> 
#include <sys/wait.h> 
#include <inttypes.h> 
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include "utility.h"
#include "queue.h"


typedef struct message {
	unsigned int stima;
	uint32_t id_client;
	int server;
}msg;

//contatore per riconoscere il doppio SIGINT
volatile sig_atomic_t sig_counter=0;

 
node *queue=NULL;//lista che contiene per ogni id client la stima fornita dal supervisor
int fdpipe[2];
int *pidFigli; //vettore contenente i pid dei figli del supervisor
int k; // server

 
/* prototipi */
void comunicazioneServer();
void creaServer();
void invia_kill();
void unlink_socket();

/*GESTORE SIGINT :
	all'arrivo del segnale SIGINT vado a controllare il contatore del segnale. Se questo è a 0 significa che l'ultimo SIGINT è arrivato
	un secondo fa o di più, quindi stampo su stderr la lista delle stime , setto il contatore a 1 e imposto una sveglia di SIGALARM 
	tra un secondo. Se invece il contatore è a 1 significa che l'ultimo SIGINT è arrivato meno di un secondo fa, quindi viene stampato
	su stdout la lista delle stime, uccido i figli e termino il programma */

void handler_int() {
	if( sig_counter == 1 ) {
		print(stdout,queue);
		printf("SUPERVISOR EXITING\n");
		exit(EXIT_SUCCESS);
	}
	print(stderr,queue);		
	sig_counter = 1;
	alarm(1);
}

/*GESTORE SIGALARM :
	se arriva SIGALAR significa che è passato esattamente un secondo dall'ultimo SIGINT --> resetto il contatore di SIGINT */

void handler_alarm() {
	sig_counter = 0;
}

int main(int argc, char* argv[]){

	if(argc!=2){
		printf("Usage: supervisor_filename k \n");
		exit(EXIT_FAILURE);
	}
	
	/* GESTIONE DEI SEGNALI SIGINT E SIGALARM */
	struct sigaction act_int;
     
	memset(&act_int,0,sizeof(act_int));
    act_int.sa_handler=handler_int;
   	sigaction(SIGINT, &act_int, NULL);
    
 	struct sigaction act_alarm;
     
	memset(&act_alarm,0,sizeof(act_alarm));
  	act_alarm.sa_handler=handler_alarm;
    	sigaction(SIGALRM, &act_alarm, NULL);

	k = atoi( argv[1] );
	printf("SUPERVISOR STARTING\n");
	
	creaServer();	

	if(atexit( invia_kill )!=0) { 
		fprintf(stderr, "Error : atexit\n");
		exit(EXIT_FAILURE); 
	}
	
	if(atexit( unlink_socket )!=0) { 
		fprintf(stderr, "Error : atexit\n");
		exit(EXIT_FAILURE); 
	}

	comunicazioneServer();

	return 0;

}

void creaServer() {
	char pip[10], indice[10] ;

	handle_errorNULL((pidFigli=malloc(sizeof(int)*k)),"Error: Malloc\n");

	handle_error1( pipe(fdpipe), "Error: Open Pipe\n");

	//crea k figli
	for(int i=0; i < k; i++){
		int pid;
		handle_error1( (pid=fork()), "Error: Fork\n");
		//i server chiudono l'estremità in lettura delle loro pipe mentre il supervisor l'estremità in scrittura 
		if(pid==0) {  //server 
				handle_error1( close(fdpipe[0]), "Error: Close\n");
				sprintf(indice, "%d", i+1);
				sprintf(pip, "%d", fdpipe[1]);
				handle_error1( execl("./server", "server", pip, indice, (char*)NULL) , "Error: Execl\n");
		} else  //supervisor
				pidFigli[i]=pid;			
	}

	handle_error1(close(fdpipe[1]), "Error: Close\n");
}


void comunicazioneServer(){
	int c;
	int size=sizeof(msg);
	msg *m;
	char *buf;
	handle_errorNULL( (m=malloc(sizeof(msg))) , "Error: Malloc\n" );
	buf=(char *)m;
	while( (c=read(fdpipe[0], buf, size))) { 
		if(c==-1 && errno!=EINTR ){
			perror("Error: Read Pipe\n");
			exit(EXIT_FAILURE);
		}
		else if(c==0) { //lettura completa
			handle_error1( close(fdpipe[0]), "Error: Close\n");
		}
		else {
			printf( "SUPERVISOR ESTIMATE %u FOR %" PRIX32 " FROM %d\n", m->stima, m->id_client, m->server);
			size = sizeof(msg);			
			buf = (char *)m;
			queue=node_add(queue,m->id_client, m->stima);
		}	
	}	
}



void invia_kill() {
	
	for(int i=0 ; i< k ; i++) 
		 kill(pidFigli[i], SIGTERM);

}

void unlink_socket() {

	char nomeSocket[30];

	for(int i=0 ; i< k ; i++) {
		sprintf(nomeSocket, "OOB-server-%d",i+1);
	
		if( unlink(nomeSocket)==-1 && errno!=ENOENT ) { 
			printf("unlink\n");
			exit(EXIT_FAILURE);
		}
	}
}


