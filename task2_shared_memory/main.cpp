// Oytun Demirbilek
// 150150032

// Geliştirme Ortamı: ITU SSH Servers
// Compile:
// Execute:

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>
#include <vector>
#define KEY1 ftok(get_current_dir_name(),150150032)
using namespace std;

// Utility function to create a piece of vector
vector <int> slice(int begin, int end, int n_piece, int index){
    vector <int> vec;
    //cout<<begin<<end<<n_piece<<index<<endl;
    int start=begin + index*(end-begin)/n_piece;
    int finish=begin + (index+1)*(end-begin)/n_piece;
    for(int i=start;i<finish;i++){
        //cout << vec_len << " "<< cond << " " << index << " "<< i << endl;
        vec.push_back(i);
    }
    return vec;

}

void* find_primes(void* args){
    vector<int> numberarray = *static_cast<vector<int>*>(args);
    vector<int> primelist;
    bool isprime=true;
    //cout<<"Thread Works!";
    for(int i=0;i<numberarray.size();i++){
        //cout << " " << numberarray[i];
        for (int j=2;j<numberarray[i];j++){
            if(numberarray[i] % j == 0)isprime=false;
        }
        if(isprime)primelist.push_back(numberarray[i]);
        //cout << " is prime: "<<isprime<< endl;
        isprime=true;
    }
    //cout << endl;
    vector<int>* result = (vector<int>*) malloc(sizeof(vector<int>));
    *result = primelist;
    return result;
}
vector <int> create_threads(int nt, vector<int> numbers)
{
    
    vector <int> temp;
    //if(nt==0)return temp;
    vector <int> child_results;
    void* thread_results;
    //cout << "Child Works!" << endl;
    pthread_t th1;
    for(int i=0;i<nt;i++){
        temp = slice(numbers.front(), numbers.back()+1, nt, i);
        pthread_create(&th1, NULL, find_primes, &temp);
        pthread_join(th1, &thread_results);
        vector<int> primes_thread = *(vector<int>*) thread_results;
        for(int j=0;j<primes_thread.size();j++){
            //cout << primes_thread[j]<< " ";
            child_results.push_back(primes_thread[j]);
        }
        //cout<<endl;
    }
    cout << "Threads Executed."<<endl;
    return child_results;
}    

int main(int argc, char **argv)
{
    int min_interval=atoi(argv[1]), max_interval=atoi(argv[2]), n_process=atoi(argv[3]), n_threads=atoi(argv[4]);
    vector< vector <int> > numbers_to_children;
    vector<int> vec;
    

    // Create Children
    pid_t pid;
    for(int i =0;i<n_process;i++){
        if(n_process==0)break;
        
        numbers_to_children.push_back(slice(min_interval, max_interval, n_process, i));

        pid = fork();
        if(pid==0){
            
            //primes_in_shared_memory = (char*)shmat(seg_id, (void*)1, 0);
            break;
        }
        //else cout << pid << endl;

    }
    // Run Children
    if (pid == 0)
    {
        // child process
        vector<int> input = numbers_to_children.back();
        numbers_to_children.pop_back();
        vector<int> output = create_threads(n_threads, input);

        int seg_id = shmget(KEY1,(max_interval-min_interval)*sizeof(int), IPC_CREAT);
        int* primes_in_shared_memory = (int*)shmat(seg_id, 0, 0);

        cout << "Child Results: " << endl;
        for(int i=0;i<output.size();i++){
            cout << output[i] << " ";
            for(int j=0;;j++){
                if(primes_in_shared_memory[j]==0){
                    primes_in_shared_memory[j]=output[i];
                    break;
                }
            }
        }
        cout << endl;
        for(int i =0;i<6;i++){
            cout << primes_in_shared_memory[i] << " ";
        }
        cout << endl;
        pthread_exit(NULL);
    }
    // Run Parent
    else if (pid > 0)
    {
        // parent process waits for its children to finish
        wait(NULL);
        sleep(1);

        int seg_id = shmget(KEY1,(max_interval-min_interval)*sizeof(int), IPC_EXCL);
        int* primes_in_shared_memory = (int*)shmat(seg_id, 0, SHM_RDONLY);
        cout << seg_id << endl;
        cout << "Latest Findings: "<<endl;
        
        /*for(int i =0;i<2;i++){
            cout << primes_in_shared_memory[i] << " ";
        }*/
        
        
        cout << endl;
        shmdt(&primes_in_shared_memory);
        
    }

    return 0;
}