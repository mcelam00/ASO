#include <stdio.h>
#include <signal.h>

struct sigaction manejadora, comportamientoAnterior;

void manejadoraSIGINT(int senial)
{

		sigaction(SIGINT, &comportamientoAnterior, NULL); //debemos reenmascarar la señal utiliando el comportamiento anterior guardado en la otra estructura, es decir, cargamos el enmascaramiento que habia anterior al que hicimos en el main

		printf("\nEl comportamiento del enmascarado ha cambiado a: comportamientoAnterior\n");

}



int main()
{

	manejadora.sa_handler = manejadoraSIGINT; 

	sigaction(SIGINT, &manejadora, &comportamientoAnterior); //En el tercer parametro vamos a guardar el comportamiento ante la señal cntl + c que tenia el programa antes de este mismo enmascaramiento, es decir, en la estructura comportamientoAnterior, salvamos lo que ocurria antes de este sigaction si se daba cntrl c que al no haber enmascaramientos previos seria el comp. por defecto, es decir, terminar proceso
	
	//LEYENDO ASI: "Enmascarar SIGINT, desviando la ejecucion por la manejadora y guardando el comportamiento anterior en comportamientoAnterior"

	while(1)
	{

	}



return 0;

}