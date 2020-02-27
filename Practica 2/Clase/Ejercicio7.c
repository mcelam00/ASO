/*

Crea un programa c en el que el programa principal cree 5 procesos hijos. Cada uno de los nuevos procesos
debe iniciar uno de estos programas: xeyes, xlogo, xload, xcalc, xclock -update 1. El programa principal debe
esperar a que uno de los procesos termine, cuando esto ocurra, se debe crear un nuevo proceso que inicie la aplicación
que se acab de cerrar.

*/


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){


pid_t retornoDelFork = 0;
pid_t hijos[5];


for(int i = 0; i < 5; i++){

retornoDelFork = fork();

	if(retornoDelFork  == 0){ //código de los hijos

		switch(i){

		case 0:
			execl("/usr/bin/xeyes", "xeyes", ((char *)0));
			break;
		
		case 1:
			execl("/usr/bin/xlogo", "xlogo", ((char *)0));		
			break;
		case 2:
			execl("/usr/bin/xload", "xload", ((char *)0));		
			break;
		case 3:
			execl("/usr/bin/xcalc", "xcalc", ((char *)0));		
			break;
		case 4:
			execl("/usr/bin/xclock", "xclock", ((char *)0));		
			break;
		}

	}else{ //código del padre, se conoce el pid del hijo creado en esta iteracion

		hijos[i] = retornoDelFork; //en la poscion del vector correspondiente a la iteracion almaceno el pid del hijo
	//lo único que hace es estorear los pid de su hijo en un vector

	}


}


int status = 0;
pid_t valRetornoWait;

valRetornoWait = wait(&status); //ademas de sakvar 

while(1){

	pid_t valRetornoWait = 0;
	retornoDelFork = fork();
	
	for (int i = 0; i < 5; i++){
	
		retornoDelFork = fork();
	
		if(retornoDelFork  == 0){ //código de los hijos

			if(hijos[i] == 	valRetornoWait){
			
				switch(i){

				case 0:
					execl("/usr/bin/xeyes", "xeyes", ((char *)0));
					break;
				
				case 1:
					execl("/usr/bin/xlogo", "xlogo", ((char *)0));		
					break;
				case 2:
					execl("/usr/bin/xload", "xload", ((char *)0));		
					break;
				case 3:
					execl("/usr/bin/xcalc", "xcalc", ((char *)0));		
					break;
				case 4:
					execl("/usr/bin/xclock", "xclock", ((char *)0));		
					break;
				}
			}

	}else{ //código del padre, se conoce el pid del hijo creado en esta iteracion

		hijos[i] = retornoDelFork; //en la poscion del vector correspondiente a la iteracion almaceno el pid del hijo
	//lo único que hace es estorear los pid de su hijo en un vector
		


	}
	
	
	
	}
	
	

} //constantemente repitiendo el bucle

return 0;




}
