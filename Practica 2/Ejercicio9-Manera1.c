#include <stdio.h>
#include <signal.h>

struct sigaction sa;

void handler (int senial){
	
}

int main () {
sa.sa_handler = handler ;

sigaction (SIGINT, &sa , NULL) ;
while (1) {}

return 0;

}
