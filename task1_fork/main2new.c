// Oytun Demirbilek
// 150150032

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void branches(void) {

	int level = 2;
	printf("Level: %d\n", level);
	printf("Parent Process ID: %d has a new child.\n", getpid());
	int child = fork();
	if(child != 0)printf("Child Process ID: %d, Child Level: %d\n", child, level + 1);
	if(child == 0)printf("\n");
	

	if(child == 0) {
		level++;
		printf("Level: %d\n", level);
		printf("Parent Process ID: %d has a new child.\n", getpid());
		child = fork();
		if(child != 0)printf("Child Process ID: %d, Child Level: %d\n", child, level + 1);
	}
	else { 
		
		printf("Level: %d\n", level);
		printf("Parent Process ID: %d has a new child.\n", getpid());
		child = fork();
		if(child != 0)printf("Child Process ID: %d, Child Level: %d\n", child, level + 1);
	}
}


int main(int argc, char ** argv) 
{
	int level = 1;
	printf("Level: %d\n", level);
	int mainpid = getpid();
	printf("Parent Process ID: %d has a new child.\n", mainpid);
	
	int child1 = fork();
	if(child1 != 0){ // Only parent process works
		printf("Child Process ID: %d\n", child1); // Print first child process id
		int child2 = fork(); // Fork Second child of parent process
		
		if(child2 != 0){ // Only parent process works
			printf("Child Process ID: %d\n", child2); // Print second child process id
			int child3 = fork(); // Fork Third child of parent process
			
			if(child3 != 0)printf("Child Process ID: %d\n", child3); // Print third child process id
			
			if(child3 == 0){ // Only child process works
				level++;
				printf("\n");
				printf("Child becomes new parent.\n");
				printf("\n");
			}
		}
		
		if(child2 == 0){ // Only child process works 
			level++;
			printf("\n");
			printf("Child becomes new parent.\n");
			printf("\n");
		}
	}
	
	if(child1 == 0){ // Only child process works
		level++;
		printf("\n");
		printf("Child becomes new parent.\n");
		printf("\n");
	}
	if(getpid() != mainpid){ // Only 3 children of main process work
		branches();
	}
}
