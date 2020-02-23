#include<stdio.h> //libreria de los printf y scanf
#include<unistd.h> 

int main(){

pid_t pid;

pid = fork();


	if(pid == 0){ //código del hijo
		//le hago que espere para que de tiempo a que el padre ejecute antes que el hijo
		
		printf("Soy el hijo, mi PID es = %d y el de mi padre es %d\n", getpid(), getppid());
		sleep(2);
	}else{

		printf("Soy el padre, mi PID es = %d, el de mi padre (terminal) es %d y el de mi hijo es %d\n", getpid(), getppid(), pid);
	

	}


return 0;

}
