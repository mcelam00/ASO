//La gracia es que el buffer no sea demasiado grande, entonces como conseguimos copiar todos los caracteres del fichero si sobrepasan los 4096¿? Pues le ponemos un do while sea diferente a cero


#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{

char buffer[4096];
 int numeroDeBytesLeidos = 0;


int descriptorCopiar = open(argv[1], O_RDONLY, 0600);
int descriptorPegar = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0600);

do
{

numeroDeBytesLeidos = read(descriptorCopiar, buffer, 4096);


write (descriptorPegar, buffer, numeroDeBytesLeidos); //si nos pasamos en el tamaño del los bits a leer lee caracteres raros, asi es que para hola mundo es 11 con el \n ya incluido

}
while(numeroDeBytesLeidos == 4096); //mientras que el cucharon de copiar siga vinivendo lleno es decir que el numero de bits que coge del archivo siga llenando el buffer pues es que el archivo sigue habiendo cosas para copiar

//tambien vale mientras que sea != 0 porque retorna 0 cuando se acaba el fichero
//while(numeroDeBytesLeidos != 0); mientras que el archivo no se termine

close (descriptorCopiar);
close (descriptorPegar);
return 0;



}
