#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

#define SEMKEY1 1501500321
#define SEMKEY2 1501500322
#define SEMKEY3 1501500323
#define SHMKEY1 1501500324
#define SHMKEY2 1501500325

void sem_release(int id, int val){
    struct sembuf sem;
    sem.sem_num = 0;
    sem.sem_op = val;
    sem.sem_flg = 1;
    semop(id, &sem, 1);
}
void sem_wait(int id, int val){
    struct sembuf sem;
    sem.sem_num = 0;
    sem.sem_op = (-1*val);
    sem.sem_flg = 1;
    semop(id, &sem, 1);
}
void print_signal(int num){
    //printf("SIGNAL: %d\n", num);
}
void sig_set(int num){
    struct sigaction act;
    act.sa_handler=(void *)print_signal;
    act.sa_flags=0;
    sigaction(num, &act, NULL);
}
int main(int argc, char ** argv){
    sig_set(10);
    int N=atoi(argv[1]), ni=atoi(argv[2]), nd=atoi(argv[3]), ti=atoi(argv[4]), td=atoi(argv[5]);
    int forknum=1, i, tempval=0;
    int parent_sem, tour_sem, child_sem, shm_id, shm_id2;
    int current_value=0;
    int is_incrementer;
    int * money_shm = NULL, * condition = NULL;
    int * children = (int*)malloc((ni+nd)*sizeof(int));
    // FORK INCREMENTER PROCESSES
    for(i=0;i<ni+nd;i++){
        if(forknum>0) forknum = fork();
        if(forknum==-1){
            printf("FORK ERROR\n");
            exit(1);
        }
        if(i<ni)is_incrementer = 1;
        else is_incrementer = 0;
        if(forknum==0)break;
        else children[i] = forknum;
    }
    
    if(forknum>0){
        // PARENT PROCESS CREATES SEMAPHORES FOR PARENT-CHILD SYNCHRONIZATION
        parent_sem = semget(SEMKEY1, 1, 0700|IPC_CREAT);
        semctl(parent_sem, 0, SETVAL, 0);

        // PARENT PROCESS CREATES SEMAPHORES FOR CHILD-CHILD SYNCHRONIZATION
        tour_sem = semget(SEMKEY2, 1, 0700|IPC_CREAT);
        semctl(tour_sem, 0, SETVAL, 1);

        // PARENT PROCESS CREATES SEMAPHORES FOR INCREMENTER-DECREMENTER SYNCHRONIZATION
        child_sem = semget(SEMKEY3, 1, 0700|IPC_CREAT);
        semctl(child_sem, 0, SETVAL, 1);

        // PARENT PROCESS CREATES SHARED MEMORY AREA
        shm_id = shmget(SHMKEY1, sizeof(int), 0700|IPC_CREAT);
        shm_id2 = shmget(SHMKEY2, sizeof(int), 0700|IPC_CREAT);
        money_shm = (int *)shmat(shm_id, 0, 0);
        condition = (int *)shmat(shm_id2, 0, 0);
        *money_shm = 0;
        *condition = 1;

        shmdt(money_shm);
        shmdt(condition);

        sleep(1);
        //wait(NULL);
        for(i=0;i<ni+nd;i++) kill(children[i], 10);
        
        

        // RELEASE ALL SEMAPHORES AND SHARED MEMORY AREAS
        sem_wait(parent_sem, ni+nd);

        printf("ALL CHILDREN HAS FINISHED\n");

        semctl(parent_sem, 0, IPC_RMID, 0);
        semctl(tour_sem, 0, IPC_RMID, 0);
        semctl(child_sem, 0, IPC_RMID, 0);
        semctl(shm_id, 0, IPC_RMID, 0);
        semctl(shm_id2, 0, IPC_RMID, 0);
        
    }
    else if(forknum == 0 && is_incrementer==1){

        printf("PID: %d, IS INCREMENTER: %d\n", getpid(), is_incrementer);
        int order = i;
        pause();
        // Run only incrementer processes
        tour_sem = semget(SEMKEY2, 1, 0);
        parent_sem = semget(SEMKEY1, 1, 0);
        child_sem = semget(SEMKEY3, 1, 0);

        shm_id = shmget(SHMKEY1, sizeof(int), 0);
        shm_id2 = shmget(SHMKEY2, sizeof(int), 0);
        money_shm = (int*)shmat(shm_id,0,0);
        condition = (int*)shmat(shm_id2,0,0);
        
        // Run before decrementers

        while(*condition){
            sem_wait(tour_sem, 1);
            if(*condition!=0){
                for(i=0;i<ti;i++){
                    // CRITICAL REGION
                    tempval = *money_shm;
                    sleep(1);
                    printf("INCREMENTER PROCESS --> Order: %d, PID: %d, Current Money: %d, ", order, getpid(), tempval);
                    if(getpid()%2==0)tempval += 10;
                    if(getpid()%2==1)tempval += 15;
                    printf("Next Money: %d\n", tempval);
                    *money_shm = tempval;
                }
            }
            if(order==ni-1){
                if(tempval >= N)*condition=0;
            }
            sem_release(tour_sem, 1);
            sleep(1);
        }
        shmdt(money_shm);
        
        sem_release(child_sem, 1);

        

        // Run after decrementers



        sem_release(parent_sem, 1);


    }
    else if(forknum == 0 && is_incrementer==0){
        
        printf("PID: %d, IS INCREMENTER: %d\n", getpid(), is_incrementer);
        int order = i;
        int fib1=1, fib2=1;
        int tempfib;
        pause();
        // Run only decrementer processes
        tour_sem = semget(SEMKEY2, 1, 0);
        child_sem = semget(SEMKEY3, 1, 0);
        parent_sem = semget(SEMKEY1, 1, 0);

        shm_id = shmget(SHMKEY1, sizeof(int), 0);
        shm_id2 = shmget(SHMKEY2, sizeof(int), 0);
        money_shm = (int*)shmat(shm_id,0,0);
        condition = (int*)shmat(shm_id2,0,0);

        // Wait incrementer processes to finish
        sem_wait(child_sem, ni);
        // CRITICAL REGION

        //while(*condition != 1){
            sem_wait(tour_sem, 1);
            if(*condition==0 && getpid()%2==0 && *money_shm%2==0){
                
                for(i=0;i<td;i++){
                    tempval = *money_shm;
                    sleep(1);
                    printf("DECREMENTER PROCESS --> Order: %d, PID: %d, Current Money: %d, ", order, getpid(), tempval);
                    tempval = tempval - fib2;
                    tempfib = fib2;
                    fib2 = fib1+fib2;
                    fib1=tempfib;
                    printf("Next Money: %d\n", tempval);
                    *money_shm = tempval;
                }
                
            }
            else if(*condition==0 && getpid()%2==1 && *money_shm%2==1){

                for(i=0;i<td;i++){
                    tempval = *money_shm;
                    sleep(1);
                    printf("DECREMENTER PROCESS --> Order: %d, PID: %d, Current Money: %d, ", order, getpid(), tempval);
                    tempval = tempval - fib2;
                    tempfib = fib2;
                    fib2 = fib1+fib2;
                    fib1=tempfib;
                    printf("Next Money: %d\n", tempval);
                    *money_shm = tempval;

                }
                
            }

            sem_release(tour_sem, 1);
            //*condition=1;

        //}
        

        shmdt(money_shm);
        shmdt(condition);

        sem_release(child_sem, ni);



        sem_release(parent_sem, 1);
    }

    return 0;
}