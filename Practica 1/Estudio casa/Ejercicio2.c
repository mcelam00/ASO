/*

	Modificar el programa para que el proceso padre, despues de escribir su mensaje espere a que termine su hijo y pinte un mensaje conteniend oel pid de su hijo muerto, y el estado de terminacion



*/
//incluimos las librerias pertinentes
#include<stdio.h> //printf y scanf
#include<unistd.h> //libreria para el fork
#include<sys/wait.h>
#include<sys/types.h>

int main(){

pid_t vRetornoFork;

vRetornoFork = fork(); //realizamos la llamada al fork === Se crea una copia de la imagen del proceso con su propio espacio de memoria (no se comaprten espacios de memoria) pero con otro PID

if(vRetornoFork == 0){ //parte del codigo del hijo

	
	printf("HIJO EN EJECUCIÓN \n VALOR RETORNO DEL FORK: %5d \n PID: %5d PID PADRE: %5d\n", vRetornoFork, getpid(), getppid());


}else{ //vRetornoFork es el pid del hijo y por lo tanto es la parte de código del padre

	printf("PADRE EN EJECUCION \n VALOR RETORNO DEL FORK: %5d \n PID: %5d PID PADRE (padre del padre, es la terminal): %5d PID HIJO (proceso creado): %5d \n", vRetornoFork, getpid(), getppid(), vRetornoFork);
	
	//el padre espera a que su proceso hijo muera
	int valorDeTerminacion;
	pid_t pidHijoFallecido = wait(&valorDeTerminacion);

	printf("El hijo con PID: %5d ha terminado con valor de retorno: %5d \n", pidHijoFallecido, valorDeTerminacion);


}















return 0;

}
