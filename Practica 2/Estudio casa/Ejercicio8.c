#include <stdio.h>
#include <signal.h>

struct sigaction manejadora;  //defino la estructura de tipo sigaction fuera para evitar posibles errores

void manejadoraSIGINT(int senial)  //funcion manejadora que se ejecutará cada vez que se envíe la señal cntl + c al programa
{

	printf("SEÑAL %d recibida\n", senial);

}




int main()
{

	manejadora.sa_handler = manejadoraSIGINT;  //establecemos en el parametro correspondiente de la estructura cual sera la manejadora

	sigaction(SIGINT, &manejadora, NULL); //primero se dice qué señal vamos a enmascarar, despues, pasamos por referencia la estructura, y en el ultimo parametro se guardaria (en otra otra estructura sigaction el comportamiento anterior a este enmascaramiento para la senial)

	//ahora mismo a partir de la linea anterior, en el momento en que llega un control + c inmediatamente se va a ejecutar la funcion manejadora

	while(1)
	{
		//infinitamente en bucle para que podamos mandar todas las señales sin miedo a que se termine el programa y no nos dé tiempo
	}


	return 0;

}
