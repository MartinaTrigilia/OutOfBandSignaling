#!/bin/bash


corrette=0
errore=0
nstime=0

#array associativi
declare -A stime
declare -A secret

#prendo come input i file di log del client e del supervisor, e leggo dopodichè, riga per riga, i dati relativi ai secret e alle stime.
#uso id come chiave dell'array di stime e di secret, in questo modo posso fare un associazione.

for file in supout clientout; do
	
	while read line; do
		ARRAY=($line)
		case $line in
			#SUPERVISOR ESTIMATE Sid FOR id BASED ON n
			*"BASED "*) 
				stime[${ARRAY[4]}]=${ARRAY[2]} ;;
			#CLIENT id SECRET secret
			*"SECRET "*) 	
				secret[${ARRAY[1]}]=${ARRAY[3]} ;;
		esac
	done <$file  #file viene usato come input della read
done

	
#calcolo le stime corrette, errore medio e faccio un count delle stime. 
# "${secret[@]}" ottiene l'elenco di tutti gli indici dell'array. 

for id in "${!secret[@]}"; do 

	diff=$(( stime[$id]-secret[$id] ))
	
	#entro un range di 25 la stima è considerata corretta
	if (( $diff < 25 )); then
		corrette=$(( $corrette+1 ))
	fi
	#se la differenza è minore di zero faccio il valore assoluto della differenza.
	if (( $diff < 0 )); then
		diff=$((-$diff))
	fi

	errore=$(( $errore + $diff ))
	nstime=$(( $nstime+ 1 ))
done
	
errore=$(( $errore/$nstime ))
percentuale=$(($corrette*100/$nstime))
sbagliate=$(( $nstime-$corrette ))

echo "Numero Stime Corrette: " ${corrette}
echo "Numero Stime Sbagliate: " ${sbagliate}
echo "Percentuale Stime Corrette: ${percentuale}%"
echo "Errore Medio: ${errore}"


