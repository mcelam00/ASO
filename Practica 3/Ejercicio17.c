//si el buffer vale 4096 y necesito copiar 700 caracteres pues de pronto copie todo el fichero 

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{

char buffer[4096];
 


int descriptorCopiar = open(argv[1], O_RDONLY, 0600);
int descriptorPegar = open(argv[2], O_WRONLY | O_CREAT, 0600);


int numeroDeBytesLeidos = read(descriptorCopiar, buffer, 4096);


write (descriptorPegar, buffer, numeroDeBytesLeidos); //si nos pasamos en el tamaño del los bits a leer lee caracteres raros, asi es que para hola mundo es 11 con el \n ya incluido

close (descriptorCopiar);
close (descriptorPegar);
return 0;



}