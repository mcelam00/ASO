#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>



//Pipe es un fichero, luego el path es: la parte de escribir es el tubo de 1 y la de leer es el 0
//Usamos pipe que es un fichero, que tiene dos descriptores uno para leer (leer[0]) y otro para escribir (escribir[1]) 



int main ()
{

int tuberia[2];
pid_t valorRetornoFork = 0;

pipe(tuberia);

char mensaje[30] = "He comprado peras y manzanas\n";

valorRetornoFork = fork();


if(valorRetornoFork == 0){
//codigo del hijo

printf("HIJO: VOY A ESCRIBIR EN EL PIPE\n");

write(tuberia[1], mensaje, 30);


}else{
//codigo del padre
printf("PADRE: VOY A LEER DEL PIPE\n");

char recibido[30] = "";

//recibido = (char *)malloc(sizeof(mensaje));

read(tuberia[0], recibido, 30);

printf("%s\n", recibido);



}
























return 0;
}
