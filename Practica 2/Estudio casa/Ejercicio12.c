#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>



int main()
{


pid_t valorDeRetornoDelFork;

valorDeRetornoDelFork = fork();

if(valorDeRetornoDelFork == 0)
{

	sleep(10); //el hijo espera 10 segundos de rigor para que el usuario pueda directamente terminar el proceso dando intro lo que resulta en usar la funcion kill
	exit(56); //NO REPRESENTA MAS ALLÁ DE 253, EL 253 ES EL MAXIMO


}else{

	int numero = 0;

	printf("Por favor, digite un número para que se haga la llamada a kill\n");
	scanf("%d", &numero);
	 //DESCOMENTAR LINEA PARA VER EL FUNCINAMIENTO SI SE MATA AL PROCESO CON KILL ANTES DE QUE TERMINE NATURALMENTE
	//kill(getpid(), SIGKILL); //es del reves que en el terminal

	//si mato el proceso el WIFEXITED devuelve 0
	int status; 
	pid_t hijoTerminado = wait(&status);

	if(WIFEXITED(status) != 0)
	{

		printf("El proceso se ha muerto naturalmente con exit retornando el valor %d\n", WEXITSTATUS(status));

	}

}








}