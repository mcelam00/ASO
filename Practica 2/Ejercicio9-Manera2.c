#include <stdio.h>
#include <signal.h>

struct sigaction sa;

void handler (int senial){
	printf("Senial recibida SIGINT\n");	
}

int main () {
sa.sa_handler = SIG_IGN; //cambiando el parámetro del struct de manera que ya no tiene definida una manejadora sino un comportamiento que implica ignorar la senial.

sigaction (SIGINT, &sa , NULL) ;
while (1) {}

return 0;

}
