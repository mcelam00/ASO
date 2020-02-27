#include <stdio.h>
#include <signal.h>


struct sigaction manejadora; //fuera del main

int main()
{
	manejadora.sa_handler = SIG_IGN; //A parte de poder indicar aqui la manejadora podemos indicar tambien 2 valores SIG_IGN para ignorar y el SIG_DFL para setear el comportamiento por defecto

	sigaction(SIGINT, &manejadora, NULL);


	while(1)
	{

	}

	return 0;

}