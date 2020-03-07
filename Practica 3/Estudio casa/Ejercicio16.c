//Recibe un argumento, la ruta del fichero, Si no existe, el programa debe crearlo y escribir holaMundo
//Si ya existe, el programa debe sobreescribir su contenido por "Hola Mundo"

/*NOTA: MEJOR USAR RUTAS RELATIVAS QUE ABSOLUTAS PARA EJECUTARLO*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>



int main(int argc, char *argv[])
{

	if(argc == 1)
	{
		printf("ERROR: No se han introducido Parámetros\n");
		exit -1;
	}

//lo primero, abrimos el fichero. Como debe escribirse en el permiso de escritura y como si no esta debe crearlo, pues o de escritura o de crearlo segun corresponda
int descriptorDelFichero = 0;


descriptorDelFichero = open(argv[1], O_WRONLY | O_CREAT, 777); //le pasamos la ruta del archivo, los flag y los permisos


//una vez abierto o creado segun proceda vamos a escribir en él

char *buffer = "Hola Mundo\n";

write(descriptorDelFichero, buffer, 11); //contando el \n como 1 byte son 11 bytes la cadena

close (descriptorDelFichero); //cerramos el archivo una vez terminamos


return 0;
}