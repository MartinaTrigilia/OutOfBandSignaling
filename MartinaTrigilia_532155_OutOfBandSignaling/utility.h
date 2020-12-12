/* 

	PROGETTO SISTEMI OPERATIVI 2019
	Martina Trigilia
	MACRO PER LA GESTIONE DEGLI ERRORI
	
*/


	#define handle_error1(fun, msg)\
 		if( (fun)==-1 ){ perror(msg); exit(errno); }

	#define handle_errorNULL(fun, msg)\
		 if( (fun)==NULL ) { perror(msg); exit(errno); }


