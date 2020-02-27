#include <stdio.h>
#include <signal.h>

struct sigaction manejadora;

void manejadoraSIGINT(int senial)
{

	//debemos restaurar por defecto el comportamiento de la señal sigint

		//redefinimos el parametro sa_handler para que sea el comportamiento por defecto
		manejadora.sa_handler = SIG_DFL;

			//debemos actualizar los cambios en la estructura volviendo a enmascarar la senial 

		sigaction(SIGINT, &manejadora, NULL);

		printf("\nEl comportamiento del enmascarado ha cambiado a: por defecto\n");

}



int main()
{

	manejadora.sa_handler = manejadoraSIGINT; 

	sigaction(SIGINT, &manejadora, NULL); //Enmascaramos la señal pasandole la estructura que determinara cual sera la manejadora de la senial

	while(1)
	{

	}



return 0;

}
