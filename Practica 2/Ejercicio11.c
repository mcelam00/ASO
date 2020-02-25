#include <stdio.h>
#include <signal.h>

struct sigaction sa,ssa; //ssa va a almacenar el comportamiento de la llamada anterior 

void handler (int senial){
	
	printf ( "SIGINT RECEIVED\n" ) ;
	sigaction (SIGINT, &ssa , NULL) ; 

}

int main () {
sa.sa_handler = handler;

sigaction (SIGINT, &sa , &ssa) ; //la primera vez que lo ejecutamos lo manejara sa (y salva en ssa el comportamiento anterior al que estamos cambiando inmediatamente ahora), y en ssa almacena el comportamiento de la llamada anterior que seria el por defecto porque no hay una antes. De manera que si yo redefino el sigaction con ssa en lugar de sa le decimos que el nuevo comportamiento sera el anterior al que tenia cuando la manejadora manejaba la senial
while (1) {}

return 0;

}
