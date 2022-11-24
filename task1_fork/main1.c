#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char ** argv) 
{

int c = 0;
printf("Prosses ID: %d has a new child, has value c: %d\n", getpid(), c);
int cocuk = fork();

c++;
if(cocuk == 0) 
{   
	printf("Prosses ID: %d has a new child, has value c: %d\n", getpid(), c);
	cocuk = fork(); 
    c += 2; 
    if(cocuk) c = c*3; 
}
else 
{ 
	c += 4;
	printf("Prosses ID: %d has a new child, has value c: %d\n", getpid(), c);
	fork();
}
printf("Prosses ID: %d has value c: %d\n", getpid(), c);
}

