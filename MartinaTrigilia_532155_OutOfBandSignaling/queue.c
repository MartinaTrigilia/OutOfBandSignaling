/* 

	PROGETTO SISTEMI OPERATIVI 2019
	Martina Trigilia, 532155
	queue.c
	
*/

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "queue.h"
#include "utility.h"

/*ritorna 1 se trova un nodo con lo stesso client e in quel caso controlla se può aggiornare la stima
altrimenti ritorna 0 */
int node_find(node* queue, uint32_t idclient, unsigned stima) { 

	if (queue==NULL) 
		return 0;
	node *corr=queue;
	while(corr!=NULL) {

		if( (corr->id) == idclient ) {

			if( (corr->num)!=5 )
				(corr->num)+=1;
			if( stima < (corr->s) )
				corr->s=stima;
			
			return 1;
		}

		else corr=corr->next;
	}
return 0;
}

node *node_add(node* queue, uint32_t idclient, unsigned stima) {
	node *new;
	handle_errorNULL( (new=(node*)malloc(sizeof(node))), "Error: Malloc\n" );

	if (queue==NULL) {

		new->id=idclient;
		new->s=stima;
		new->num=1;	
		new->next=NULL;
		queue=new;
	
	}  
	if( (node_find(queue,idclient,stima))==0) {
		
		new->id=idclient;
		new->s=stima;
		new->num=1;
		new->next=queue;
		queue=new;
				
	} 
return queue;
}

void print(FILE *stream, node *queue) {
	
	while(queue!=NULL) {
		fprintf(stream,"SUPERVISOR ESTIMATE %u FOR %" PRIX32 " BASED ON %d\n",queue->s, queue->id, queue->num );
		queue=queue->next;
	}
}

