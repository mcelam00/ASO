//Emular el comportamiento del comando cp. 
//2 argumentos: La ruta del archivo del que copiar y la ruta del archivo al que copiar
//el buffer tendrá 4096 bytes


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	
//declaramos el buffer
char buffer[4096];


//vamos a necesitar 2 ficheros, uno para leer y otro para copiar. Se abren si creados y se crean si no

int descriptorCopiar = 0;
int descriptorPegar = 0;

	descriptorCopiar = open(argv[1], O_RDONLY, 777); //el archivo del que voy a copiar lo abro con permisos de lectura o lo creo si no esta creado
	descriptorPegar = open(argv[2], O_WRONLY | O_CREAT, 777); //el fichero que voy a pegar lo abro con permisosde escritura o lo creo si no estaba


	int numeroDeBytesCopiados = read(descriptorCopiar, buffer, 4096); //vamos a copiar en el buffer y 4096 bits

	write(descriptorPegar, buffer, numeroDeBytesCopiados); //vamos a pegar en el archivo de pegado, lo que este en buffer (que será lo que hayamos copiado), y si acaso el buffer no se lleno pegamos solo lo que nos ha dicho el read que ha leid que bien pueden ser los 4096 si los hay o menos


	close(descriptorCopiar);
	close(descriptorPegar);

	return 0;




}
