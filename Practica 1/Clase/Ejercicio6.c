#include<stdio.h> //libreria de los printf y scanf
#include<unistd.h> 

int main(){

pid_t pid;

pid = fork();


	if(pid == 0){

		printf("Soy el hijo, mi PID es = %d y el de mi padre es %d\n", getpid(), getppid());
		//El hijo pasará a ejecutar el calendario

		int retornoFallidoExec = execl("/usr/bin/cal", "cal",(char *)0);

		//se carga asi pues la imagen de proceso del calendario perdiendose todo el código que pueda haber de aqui para abajo. En el if del hijo y cuando termine el if debajo del if (que es codigo comun de ambos pero solo lo ejecuta el padre por este motivo)
		


	
	}else{

		printf("Soy el padre, mi PID es = %d, el de mi padre (terminal) es %d y el de mi hijo es %d\n", getpid(), getppid(), pid);
	

	}


return 0;

}
