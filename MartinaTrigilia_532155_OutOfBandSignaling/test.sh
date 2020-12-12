#!/bin/bash
./supervisor 8 >>supout 2>>superr &

sleep 2


echo "Eseguo i client"
for (( i=0; i<10; i++ ));do
	./client 5 8 20 >>clientout & 
	./client 5 8 20 >>clientout & 
	sleep 1 
done

echo "Invio i segnali"
for ((i = 0 ; i < 6 ; i++ )); do
	kill -INT $(pidof supervisor)
	sleep 10
done

kill -INT $(pidof supervisor)
kill -INT $(pidof supervisor)

echo " *********Report Stime********** "
echo " "
./misura.sh supout clientout