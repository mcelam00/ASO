//el open nos retorna un descriptor de fichero que usaremos en el close

//si yo le paso un buffer a lectura de 2 y en el fichero esta hola solamente me va a seleccionar "ho" char[2]; read(a,buffer,2); write(a,buffer,);

//en este ejercicio solo hay que leer

//en el segundo hay que coger el fichero en que va a escribir, entonces leo de fichero lo almaceno en el buffer y ese mismo bufffer que contiene lo que hemos leido se lo pasamos al segundo de los ficheros.

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{

char *buffer = "Hola Mundo\n";
 


int descriptor = open(argv[1], O_WRONLY | O_CREAT, 777);
write (descriptor, buffer, 11); //si nos pasamos en el tamaño del los bits a leer lee caracteres raros, asi es que para hola mundo es 11 con el \n ya incluido

close (descriptor);
return 0;

}
