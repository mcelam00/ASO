/*

	Realizar un pequeño programa en C de manera que se muestre por pantalla el PID y acto seguido usando la llamada exec cargue la imagen de memoria del calendario (comando cal) y Una vez hecho ésto el programa debe tratar de volver a mostrar su pid. (no será posible porque una vez que el programa ejecuta exec nunca vuelve al codigo original puesto que se reemplaza la imagen de memoria por la del calendario en este caso)

*/


#include<stdio.h>//printf y scanf
#include<unistd.h> //fork y exec



int main(){

printf("Soy yo, el proceso, mi PID es: %d y voy a ejecutar el calendario\n", getpid());

execl("/usr/bin/cal", "cal", (char *)0);//le buscamos el path con whereis cal, el comando es cal que no tiene argumentos y al final siempre va (char *)0

//EN ESTE MOMENTO SE HA CARGADO LA IMAGEN DE MEMORIA DEL CALENDARIO Y SE PASARA A EJECUTAR ESA IMAGEN, NO VOLVIENDOSE A ESTE CÓDIGO,CON LO CUAL NADA DE LO QUE VAYA AQUI DEBAJO VA A FUNCIONAR.

printf("ESTO NO VA A FUNCIONAR, PERO MI PID ES: %d\n", getpid());




return 0;
}



