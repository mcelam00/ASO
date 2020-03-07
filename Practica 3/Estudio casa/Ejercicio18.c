//programa que ejecute el comando ls | wc (proposito de conteo). 
//Se crea un hijo con fork
//El padre debe ejecutar ls con exec y el hijo wc con exec tambien
//la salida del comando ejecutado por el padre debe llegarle como entrada estandar al proceso hijo a través de un pipe, 
//es decir, hay que cambiar la salida para que en lugar de enviar la salida del comando a la terminal que seria la salida estandar lo haga al pipe


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main()
{

int tuberia[2];
int entradaEstandar = 0; 
int salidaEstandar = 1;


pipe(tuberia); //creo la tuberia preparada para la comunicacion

pid_t vRetornoFork = 0;

vRetornoFork = fork();

	if(vRetornoFork == 0) //código del hijo
	{
			
		dup2(0, tuberia[0]); //debo cambiar la entrada estandar del hijo para que sea el descriptor de lectura del pipe

		execl("/bin/ls", "ls", ((char *)0));


	}
	else //código del padre
	{

		dup2(1, tuberia[1]); //la salida de este programa no debe ser a la salida estandar sino a la pipe, de manera que debemos redirigir la salida estandar a la pipe de escritura

		execl("/usr/bin/wc", "wc", ((char *)0)); 


	}






}