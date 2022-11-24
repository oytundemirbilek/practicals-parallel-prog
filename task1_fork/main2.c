#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv) 
{ 
int level = 1;
printf("Level: %d\n", level);
printf("Parent Prosses ID: %d has a new child.\n", getpid());
int parent = fork();
if(parent != 0)printf("Child Prosses ID: %d\n", parent);

if(parent == 0)printf("\n");

if(parent == 0){
	level++;
	printf("Level: %d\n", level);
	printf("Parent Prosses ID: %d has a new child.\n", getpid());
	parent = fork();
	if(parent != 0)printf("Child Prosses ID: %d\n", parent);
		
	if(parent == 0){	
		level++;
		printf("Level: %d\n", level);
		printf("Parent Prosses ID: %d has a new child.\n", getpid());
		parent = fork();
		if(parent != 0)printf("Child Prosses ID: %d\n", parent);
		
		if(parent == 0){
			level++;
			printf("Level: %d\n", level);
			printf("These children do not have any child.\n");
		}
		else {
			printf("Parent Prosses ID: %d has a new child.\n", getpid());
			parent = fork();
			if(parent != 0)printf("Child Prosses ID: %d\n", parent);
		}

	}
	else {
		printf("Parent Prosses ID: %d has a new child.\n", getpid());
		parent = fork();
		if(parent != 0)printf("Child Prosses ID: %d\n", parent);
	}

}
else {	
	printf("Parent Prosses ID: %d has a new child.\n", getpid());	
	parent = fork();
	if(parent != 0)printf("Child Prosses ID: %d\n", parent);
	if (parent == 0){
		level++;
		printf("Level: %d\n", level);
		printf("Parent Prosses ID: %d has a new child.\n", getpid());	
		parent = fork();
		if(parent != 0)printf("Child Prosses ID: %d\n", parent);
	}
}
}
