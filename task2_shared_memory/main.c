// Oytun Demirbilek
// 150150032

// Geliştirme Ortamı: ITU SSH Servers
// Compile:
// Execute:

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>

#define KEY1 ftok("./OS2",150150032)
void shm_read(){

    int *array;
    int count = 5000;
    int i = 0;

    int shmid = shmget(KEY1, count*sizeof(int), IPC_EXCL);

    array = shmat(shmid, 0, SHM_RDONLY);

    for(i=0; i<5; i++)
    {
        printf("\n%d is read from shared memory.\n", array[i] );
    }
    shmdt((void *) array);
}
void shm_write(int* input){

    int count = 5000;
    int i = 0;

    int* array;
    int memsize = sizeof(*array)*count;
    // Shared Memory Allocation
    int shmid = shmget(KEY1, count*sizeof(int), IPC_CREAT);
    array = (int *)shmat(shmid, 0, 0);

    for(i=0; i<5; i++)
    {
        array[i] = i;
    }

    for(i=0; i<5; i++)
    {
        printf("\n%d is in shared memory.\n", array[i]);
    }
    shmdt((void *) array);

}
struct list {
    int data;
    struct list * next;
};
int get_size(struct list * root){
    int count = 0;
    while(root!=NULL){
        root = root->next;
        count++;
    }
    return count;
}
struct list * append(struct list * root, int num){
    if(root==NULL){
        root = (struct list *)malloc(1*sizeof(struct list));
        root->next = NULL;
        root->data = num;
    }
    else{
        struct list * temp = root;
        while(temp->next!=NULL){
            temp=temp->next;
        }
        temp->next = (struct list *)malloc(1*sizeof(struct list));
        temp->next->data = num;
    }
    
    return root;
}
void printlist(struct list * root){
    struct list * temp = root;
    while(temp!=NULL){
        printf("%d ", temp->data);
        temp=temp->next;
    }
}
int front(struct list * root){
    if(root!=NULL)return root->data;
    else return 0;
}
int back(struct list * root){
    if(root == NULL)return 0;
    else{
        struct list * temp = root;
        while(temp->next!=NULL){
            temp=temp->next;
        }
        return temp->data;
    }
}


// Utility function to create a piece of vector
struct list * slice(int begin, int end, int n_piece, int index){
    int len = (end-begin)/n_piece;
    struct list * slicedlist=NULL;
    int i = 0;
    int start=begin + index*len;

    for(i=0;i<len;i++){
        slicedlist = append(slicedlist, start + i);
    }
    //printf("\n");
    return slicedlist;

}

void* find_primes(void* args){
    struct list * numberarray = (struct list *)args;
    int primesarray[100];
    int i = 0, j = 2;
    int isprime=1;

    //printf("Thread Works! ");
    while (numberarray!=NULL){
        for (j=2;j<numberarray->data;j++){
            if(numberarray->data % j == 0)isprime=0;
        }
        if(isprime==1){
            primesarray[i] = numberarray->data;
            i++;
            printf("%d is prime.\n", numberarray->data);
            //shm_write(primesarray);
        }
        isprime=1;
        numberarray = numberarray->next;
    }
}
void create_threads(int nt, struct list * numbers)
{
    int i=0,j=0;

    struct list * child_results, * temp;
    struct list * th_primes;

    void* thread_results;
    //printf("Child Works!\n");
    pthread_t th1;
    for(i=0;i<nt;i++){
        int first = front(numbers), last = back(numbers)+1;

        temp = slice(first, last, nt, i);
        //printf("\n");
        printf("Thread %d starting... Searching: %d - %d\n", i, front(temp), back(temp));
        //printlist(temp);
        //printf("\n");
        pthread_create(&th1, NULL, find_primes, temp);
        pthread_join(th1, NULL);
        
    }
}    

int main(int argc, char **argv)
{
    int min_interval=atoi(argv[1]), max_interval=atoi(argv[2]), n_process=atoi(argv[3]), n_threads=atoi(argv[4]);
    struct list ** numbers_to_children = (struct list **)malloc(n_process*sizeof(struct list *));

    int i = 0, j = 0;
    int childnum;

    // Create Children
    pid_t pid;
    for(i = 0;i<n_process;i++){
        if(n_process==0)break;
        numbers_to_children[i]=slice(min_interval, max_interval, n_process, i);
        
        pid = fork();
        if(pid==0){
            childnum = i;
            break;
        }
    }
    
    // Run Children
    if (pid == 0)
    {
        // child process
        struct list * input;
        input = numbers_to_children[childnum];
        
        printf("Child %d starting... Searching: %d - %d\n", childnum, front(input), back(input));
        //printlist(input);
        create_threads(n_threads, input);
        pthread_exit(NULL);
    }
    // Run Parent
    else if (pid > 0)
    {
        // parent process waits for its children to finish.
        wait(NULL);
        sleep(1);
        printf("Latest Findings:\n");
        //shm_read();
        printf("\n");
    }

    return 0;
}