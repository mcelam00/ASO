//Escribir un programa de manera que el hijo conozca su PID , el de su padre y el de su abuelo (que es el padre del padre y le pedirá a su padre mediante una pipe).
//El padre conoce su pid, el de su hijo y el de su padre que deberá pasarle al hijo por un pipe.


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>




int main()
{

//Para utilizar pipes como medio de comunicacion entre procesos debemos en primer lugar definir el vector (de enteros)
	int tuberia[2];


	pipe(tuberia); //transformo la tuberia en un archivo con dos descriptores 0 para leer y 1 para escribir


pid_t vRetFork = 0;



vRetFork = fork();

	if(vRetFork == 0) //código del hijo
	{
		pid_t miPIDH = getpid();
		pid_t PIDMiPadre = getppid();

			char *buffer;

			buffer = (char *)malloc(sizeof(buffer)); //o reservamos el tamaño así de forma dinamica o ponemos uno fijo de 11 pero poniéndolo como puntero solo no funciona

			sleep(2);//para dar tiempo a que el padre escriba

			read(tuberia[0], buffer, sizeof(buffer));//El hijo lee de la tubería (en el buffer) el PID de su abuelo

			printf("HIJO:  Mi padre = %d   yo = %d   mi abuelo = %s \n",PIDMiPadre, miPIDH, buffer);

	}
	else //código del padre
	{

		pid_t PIDMiHijo = vRetFork;
		pid_t miPIDP = getpid();
		pid_t PIDAbuelo = getppid();
		char PIDAbueloCadena[11];

			sprintf(PIDAbueloCadena, "%d", PIDAbuelo);
			printf("	TEST PRUEBA CONCATENADO : %s\n", PIDAbueloCadena);

			write(tuberia[1], PIDAbueloCadena, sizeof(PIDAbueloCadena)); //El padre escribe en la tuberia el pid del abuelo


			printf("PADRE:  Mi padre = %d   yo = %d   mi hijo = %d \n", PIDAbuelo, miPIDP, PIDMiHijo);

			int status = 0;
			
			wait(&status);
	}



return 0;
}