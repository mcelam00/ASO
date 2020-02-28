#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



int main()
{


pid_t retornoFork = 0;
pid_t hijos[5];

	for(int i = 0; i < 5; i++)
	{
		retornoFork = fork(); //vamos a crear 5 hijos, y cada hijo en su iteracion en su codigo ejecutara una app


			if(retornoFork == 0) //parte del código que ejeucta el hijo. Será diferente en funcion de la iteracion
			{
				//habrá que ver qué hijo es el que toca

				switch(i)
				{
					case 0:
							execl("/usr/bin/xeyes","xeyes",((char *)0));
							break; //IMPORTANTE NO OLVIDARLAS
					case 1:
							execl("/usr/bin/xlogo","xlogo",((char *)0));
							break;
					case 2:
							execl("/usr/bin/xload","xload",((char *)0));
							break;
					case 3:
							execl("/usr/bin/xcalc","xcalc",((char *)0));
							break;
					case 4:
							execl("/usr/bin/xclock","xclock","-update 1",((char *)0));
							break;

				}

			}
			else //parte del código que ejecuta el padre
			{
				//para volver a arrancar debe saber que hijo ha muerto, es decir, aquel cuyo pid falte

				hijos[i] = retornoFork;


			}



	}

//PRECONDICION : LAS 5 APLICACIONES ESTÁN ACTIVAS Y FUNCIONANDO

	while(1)   //Tenemos que estar constantemente esperando que mueran hijos
	{

		pid_t pidHijoMuere = 0;
		int status = 0; 

		pidHijoMuere = wait(&status);

		//busco en el vector de hijos el PID y en la posicion que esté es segun (mirando el switch porque tiene el orden del vector), lanzar de nuevo la app y salvar de nuevo el PID en ese mismo hueco

			for(int i = 0; i < 5; i++)
			{
				if(hijos[i] == pidHijoMuere)
				{
					//resulta que en esa i es donde esta el hijo muerto
					//creo un nuevo hijo
					retornoFork = fork();

						if(retornoFork == 0)
						{
							//relanzo segun el switch
							switch(i)
							{
							case 0:
									execl("/usr/bin/xeyes","xeyes",((char *)0));
									break; //IMPORTANTE NO OLVIDARLAS
							case 1:
									execl("/usr/bin/xlogo","xlogo",((char *)0));
									break;
							case 2:
									execl("/usr/bin/xload","xload",((char *)0));
									break;
							case 3:
									execl("/usr/bin/xcalc","xcalc",((char *)0));
									break;
							case 4:
									execl("/usr/bin/xclock","xclock","-update 1",((char *)0));
									break;

							}


						}
						else
						{
							//el padre sobreescribe el PID en el lugar exacto donde estaba el del que murio

							hijos[i] = retornoFork;


						}


				}


			}






	}





 




}

