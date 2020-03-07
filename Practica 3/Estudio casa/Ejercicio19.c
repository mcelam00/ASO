/*
El programa principal crea un proceso hijo
El hijo debe escribir Hola Mundo en el pipe
El padre debe leer el contenido del hijo por el extremo de lectura del pipe y enseñarlo por pantalla
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>





int main()
{

int tuberia[2]; //declaro la tuberia

pipe(tuberia); //es como si abriera la tuberia para las comunicaciones 0 para leer y 1 para escribir


pid_t vRetornoFork = 0;

vRetornoFork = fork();

	if(vRetornoFork == 0) //código del hijo
	{
		//escribe holamundo en el pipe
		char mensaje[11] = "Hola Mundo";

		write(tuberia[1], mensaje, sizeof(mensaje));

	}
	else //código del padre
	{
		//lee holamundo del pipe y lo pinta por la pantalla
		char bufferLectura[11];	//ES MEJOR TRABAJAR CON VECTORES ESTÁTICOS

		read(tuberia[0], bufferLectura, sizeof(bufferLectura));


		printf("%s\n", bufferLectura);

	}

return 0;

}









