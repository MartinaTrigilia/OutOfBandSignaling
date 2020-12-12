#ifndef QUEUE_H_
#define QUEUE_H_


typedef struct nodes {
	uint32_t id; 
	unsigned int s;
	int num; //num di server che ha fornito stime per quel client
	struct nodes *next;
}node;



void print(FILE *stream, node *queue);

node *node_add(node* queue, uint32_t idclient, unsigned int stima);

int node_find(node* queue, uint32_t idclient, unsigned int stima );

#endif
