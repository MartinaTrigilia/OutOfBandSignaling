# OutOfBandSignaling
Introduzione


Lo studente dovrà realizzare un sistema per l'out-of-band signaling. Si tratta di una tecnica di comunicazione
in cui due entità si scambiano informazioni senza trasmettersele direttamente, ma utilizzando segnalazione
collaterale: per esempio, il numero di errori “artificiali”, o la lunghezza di un pacchetto pieno di dati inutili, o
anche il momento esatto delle comunicazioni.
Nel nostro caso, vogliamo realizzare un sistema client-server, in cui i client possiedono un codice segreto
(che chiameremo secret) e vogliono comunicarlo a un server centrale, senza però trasmetterlo. Lo scopo è
rendere difficile intercettare il secret a chi stia catturando i dati in transito.
Nel nostro progetto avremo n client, k server, e 1 supervisor. Viene lanciato per primo il supervisor, con un
parametro k che indica quanti server vogliamo attivare; il supervisor provvederà dunque a creare i k server
(che dovranno essere processi distinti). Gli n client vengono invece lanciati indipendentemente, ciascuno in
tempi diversi. 
