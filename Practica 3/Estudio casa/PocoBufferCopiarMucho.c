/*
Sin modificar el tamaño del buffer de 4096 bytes, copiar y pegar el contenido de un archivo 
mucho mas grande sin modificar el tamanio del buffer
*/

#include <unistd.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{


char buffer[4096]; //nuestro buffer inamovible
int numeroDeBytesLeidos = 0;

//se abre el archivo de copiar y de pegar

	int descriptorCopiar = open(argv[1], O_RDONLY, 777); //path, flags y permisos
	int descriptorPegar = open(argv[2], O_WRONLY | O_CREAT, 777);


	//como con 4096 no va a ser suficiente para coger la informacion de una vez, lo que hacemos es un do while, de manera que copie hasta que el numero de bytes leidos por el read sean 0 en cuyo caso ya no habra mas que leer en el archivo

	do
	{

		 numeroDeBytesLeidos = read(descriptorCopiar, buffer, sizeof(buffer)); //vamos a leer del archivo a copiar, en el buffer lo guardamos y el nº de bytes es el que indique el buffer, es decir lo que se ha copiado

		write(descriptorPegar, buffer, numeroDeBytesLeidos); //pego, en el archivo pegar, lo que haya en el buffer que seran los bytes leidos en tamanio



	}while(numeroDeBytesLeidos != 0); //mientras que el read siga encontrando cosas para leer en el archivo y no haya llegado al final


return 0;

}