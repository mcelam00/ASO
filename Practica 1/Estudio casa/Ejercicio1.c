/*

	Modificar el programa para que se imprima siempre la linea del padre antes que la del hijo, esto es, el hijo debe contener un sleep, de manera que el padre se ejecute antes siempre en ese tiempo que el hijo esta dormido



*/
//incluimos las librerias pertinentes
#include<stdio.h> //printf y scanf
#include<unistd.h> //libreria para el fork

int main(){

pid_t vRetornoFork;

vRetornoFork = fork(); //realizamos la llamada al fork === Se crea una copia de la imagen del proceso con su propio espacio de memoria (no se comaprten espacios de memoria) pero con otro PID

if(vRetornoFork == 0){ //parte del codigo del hijo

	sleep(1);//el hijo duerme un segundo antes de pintarse, de manera que el padre siempre va antes
	printf("HIJO EN EJECUCIÓN \n VALOR RETORNO DEL FORK: %5d \n PID: %5d PID PADRE: %5d\n", vRetornoFork, getpid(), getppid());


}else{ //vRetornoFork es el pid del hijo y por lo tanto es la parte de código del padre

	printf("PADRE EN EJECUCION \n VALOR RETORNO DEL FORK: %5d \n PID: %5d PID PADRE (padre del padre, es la terminal): %5d PID HIJO (proceso creado): %5d \n", vRetornoFork, getpid(), getppid(), vRetornoFork);




}















return 0;

}


