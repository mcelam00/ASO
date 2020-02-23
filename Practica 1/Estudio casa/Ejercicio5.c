/*

	Utiliza la macro WEXITSTATUS() para interpretar el estado del proceso que termina (el primer octeto de los 16 bits o lo que es lo mismo el 33 tal cual viene del exit)



*/
//incluimos las librerias pertinentes
#include<stdio.h> //printf y scanf
#include<unistd.h> //libreria para el fork
#include<sys/wait.h>
#include<sys/types.h>
#include<stdlib.h>

int main(){

pid_t vRetornoFork;

vRetornoFork = fork(); //realizamos la llamada al fork === Se crea una copia de la imagen del proceso con su propio espacio de memoria (no se comaprten espacios de memoria) pero con otro PID

if(vRetornoFork == 0){ //parte del codigo del hijo

	
	printf("HIJO EN EJECUCIÓN \n VALOR RETORNO DEL FORK: %5d \n PID: %5d PID PADRE: %5d\n", vRetornoFork, getpid(), getppid());
	exit(33);


}else{ //vRetornoFork es el pid del hijo y por lo tanto es la parte de código del padre

	printf("PADRE EN EJECUCION \n VALOR RETORNO DEL FORK: %5d \n PID: %5d PID PADRE (padre del padre, es la terminal): %5d PID HIJO (proceso creado): %5d \n", vRetornoFork, getpid(), getppid(), vRetornoFork);
	
	//el padre espera a que su proceso hijo muera
	int valorDeTerminacion;
	pid_t pidHijoFallecido = waitpid(vRetornoFork, &valorDeTerminacion, 0);

	int traduccionEstado;
	
	traduccionEstado = WEXITSTATUS(valorDeTerminacion);

	printf("El hijo con PID: %5d ha terminado con valor de retorno: %5d \n", pidHijoFallecido, traduccionEstado);


}















return 0;

}
