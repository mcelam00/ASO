#include <stdio.h>
#include <signal.h>

struct sigaction sa;

void handler (int senial){
	
	printf ( "SIGINT RECEIVED\n" ) ;
	sa.sa_handler = SIG_DFL; //es necesario redefinir quien controla el cntrl c la siguiente vez, en este caso, el comportamiento es ignorar
	sigaction (SIGINT, &sa , NULL) ;  //es necesario volver a llamar a sigaction

}

int main () {
sa.sa_handler = handler;

sigaction (SIGINT, &sa , NULL) ;
while (1) {}

return 0;

}
