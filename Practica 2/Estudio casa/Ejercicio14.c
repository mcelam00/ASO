# include <stdio.h>
# include <unistd.h>
# include <signal.h>


struct sigaction sa ;

int seconds ;

void tic ( int i ) {
	seconds ++;
	printf("%d\n", seconds);
}

int main ( void ) {

	sa.sa_handler = tic ;
	sigaction ( SIGALRM , &sa , NULL ) ;
	while(1)
	{
		alarm(1);
		pause();
		
	}
}