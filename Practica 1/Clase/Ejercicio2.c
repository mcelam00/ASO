#include<stdio.h> //libreria de los printf y scanf
#include<unistd.h> 

//librerias para el wait
#include<sys/types.h>
#include<sys/wait.h>

int main(){

pid_t estadoDelHijoAlMorir;
pid_t pid;

pid = fork();


	if(pid == 0){ //código del hijo

		printf("Soy el hijo, mi PID es = %d y el de mi padre es %d\n", getpid(), getppid());
				

	}else{

		printf("Soy el padre, mi PID es = %d, el de mi padre (terminal) es %d y el de mi hijo es %d\n", getpid(), getppid(), pid);
		
		wait(&estadoDelHijoAlMorir);

		printf("Mi hijo ha terminado con valor de retorno %d\n", estadoDelHijoAlMorir);	

	}


return 0;

}
