CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic 
LDFLAGS = -L.

.PHONY: all clean test
all : server supervisor client 


queue : queue.c
	@echo ""
	@echo " Compilo librerie"
	$(CC) $(CFLAGS) queue.c -c -o queue
	ar rvs libqueue.a queue

server : server.c
	@echo ""
	@echo "***** Compilo server"
	$(CC) $(CFLAGS) -pthread server.c $(LDFLAGS) -o server


supervisor : queue supervisor.c
	@echo ""
	@echo "*****Compilo supervisor"
	$(CC) $(CFLAGS) supervisor.c $(LDFLAGS) -lqueue -o supervisor


client : client.c
	@echo ""
	@echo "***** Compilo client"
	$(CC) $(CFLAGS) client.c $(LDFLAGS)  -o client

perm:
	@chmod +x test.sh

test:
	@./test.sh

clean: 
	rm -f server supervisor client queue clientout superr supout *.o
