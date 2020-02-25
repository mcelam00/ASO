#include <stdio.h>
#include <signal.h>
#include <unistd.h>

struct sigaction sa,ssa; //ssa va a almacenar el comportamiento de la llamada anterior 

void handler (int senial){

 //el manejador no sirve para nada puesto que alarm se utiliza unicamente para que pase un segundo
}

int main () {
sa.sa_handler = handler;

sigaction (SIGALRM, &sa , NULL) ; //la primera vez que lo ejecutamos lo manejara sa (y salva en ssa el comportamiento anterior al que estamos cambiando inmediatamente ahora), y en ssa almacena el comportamiento de la llamada anterior que seria el por defecto porque no hay una antes. De manera que si yo redefino el sigaction con ssa en lugar de sa le decimos que el nuevo comportamiento sera el anterior al que tenia cuando la manejadora manejaba la senial

int segundos = 0;
while (1) {

	alarm(1);
	printf("%d\n", segundos);
	segundos++;
	pause(); //que espere indefinidamente por señales
}

return 0;

}
