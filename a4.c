/*
* Author 1: Daniel Austin
* Email: Aust2920@mylaurier.ca
* Student Number: 180472920
*
* Author 2: Angela Tran
* Email: Tran2140@mylaurier.ca
* Student Number: 180392140
*
* July 31, 2020
* CP386 Assignment 4
*
* The goal of this assignment is to implement Bankers algorithmm for shared resource allocation in C.
* 
* Customers are retrieved from the input file, an nxm matrix where a rows correspond to customers, and 
* columns correspond to the maximum number of resources customers need to execute.
*
* The number available resources is defined by user argument at runtime.
*
* Commands:
*   'RQ [int customer] [int input 1] [int input 2] ... [int input n]'
*       Requests resources inputed be allocated to customer. 
*       If the request leaves the system unsafe it will be denied
*
*   'RL [int customer] [int input 1] [int input 2] ... [int input n]'
*       Requests resources inputed to be de-allocated from customer
*
*   '*' 
*       Displays all arrays and matrices used.
*
*   'Run'
*       Executes customers as threads in a safe sequence. Each thread requests the resources it needs,
*       releases them, and lets the next thread in the sequence run
*
*    Run with: gcc -pthread a4.c
*    ./a.out 10 6 7 8 (Or use other input values as needed)
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h> 
#include <pthread.h>

#define true 1
#define false 0
#define file "sample4_in.txt"

int n = 0;
int m = 0;
sem_t **lock;
sem_t work;


int* available;
int** maximum;
int** allocation;
int** need;
char input[100] = { 0 };


void listResources();
int readFile(char* fileName);
void custCount(char *fileName);

void* threadRun(void*  t);
int* safety();
int rq(int threadID, int arr[]);
int rl(int threadID, int arr[]);



int main(int argc, char *argv[]){
    custCount(file);
    //TODO: documentation. Everything is perfect


    printf("BEGIN%s","\n");

    lock = malloc(sizeof(sem_t*) * n);
    maximum = malloc(sizeof(int*) * n);
    allocation = malloc(sizeof(int*) * n);
    need = malloc(sizeof(int*) * n);
    available = malloc(sizeof(int) * m);

    for (int i = 0; i < n; i++){
        maximum[i] = malloc(sizeof(int) * m);
        allocation[i] = malloc(sizeof(int) * m);
        need[i] = malloc(sizeof(int) * m);
    }

   //Gets the number of resource types from the user when the program is started
    for (int rows = 0; rows < m; rows++){
       available[rows] = atoi(argv[rows+1]); 
    }
    //reads input frmo the file and stores in maximum
    readFile(file);

    //updates allocation and need accordingly
    for (int rows = 0; rows < n; rows++){
        for (int cols = 0; cols < m; cols++){
           allocation[rows][cols] = 0;
           need[rows][cols] = maximum[rows][cols] - allocation[rows][cols];
        }
    }
    sem_init(&work, 0, 0);
    for (int i = 0; i < n; i++){
        lock[i] = (sem_t*) malloc(sizeof(sem_t));
    }

    listResources(available, maximum, allocation, need);

    while(1){
        fgets(input, 100, stdin);
        if (strlen(input) > 0) input[strlen(input) - 1] = '\0'; //remove newline

        char *command = strtok(input, " ");
        if (strcmp(command, "Run") == 0 || strcmp(command, "run") == 0 ){
            //RUN using bankers algorithm
            
            int* sequence;
            int a[n];
            int threadID[n];
            pthread_t tID[n];
            
            for (int i = 0; i < n; i++){
                sem_init(lock[i], 0, 0);
                a[i] = i;
                pthread_create(&tID[n], NULL, threadRun, (void*) &a[i]);

            }
            
            sleep(1);
            
            sequence = safety();
            
            if (sequence[0] == -1){
                printf("%s", "No safe sequence exists.\n");
            }
            else{
                for (int i = 0; i < n; i++){
                    sem_post(lock[sequence[i]]);
                    sem_wait(&work);
                    //free(lock[i]);
                }
            }
            free(sequence);
        }
        else if ((strcmp(command, "RQ") == 0) || (strcmp(command, "rq") == 0)){

            command = strtok(NULL, " ");
            int thread = atoi(command), rval;
            int arr[m];

            for (int x = 0; x < m; x++){
                arr[x] = atoi(strtok(NULL, " "));
            }

            rval = rq(thread, arr);
            if (rval == 1){
                printf("Request granted\n");
            }
            else{
                printf("Request denied\n");
            }
            
        }
        else if ((strcmp(command, "RL") == 0 ) || (strcmp(command, "rl") == 0)){
            //Release resource
            command = strtok(NULL, " ");
            int thread = atoi(command);
            int arr[m], t = true ,j = 0;

            for (int x = 0; x < m; x++){
                arr[x] = atoi(strtok(NULL, " "));
            }
            for (int x = 0; x < m; x++){
                if ((arr[x] > allocation[thread][x]) || (arr[x] < 0)) t = false;
            }
            if (t == true){
                rl(thread, arr);
                printf("Release %s", "success\n");
            }
            else{
                printf("Release %s", "failed.\n");
            }
        }
        else if (strcmp(command, "*") == 0){
            listResources();
        }
        else{
            printf("%s", "Invalid Input\n");
        }
    }
}

int* safety(){ //I think safety works
    /*
    Returns a safe sequence to run threads. If none exists, then the sequence begins with '-1'
    */
    int Finish[n];
    int Work[m];
    int ind = 0, modified = true;
    int* sequence = malloc(sizeof(int) * n);
    for (int i = 0; i < m; i++){
        Work[i] = available[i];
    }
    for (int i = 0; i < n; i++) {
        Finish[i] = false;
    }

    while (modified == true){
        modified = false;
        for (int i = 0; i < n; i++){
            int rval = 1;
            for (int x = 0; x < m; x++){
                if (need[i][x] > Work[x]){
                    rval = 0;
                }
            }
            if ((Finish[i] == false) && rval == 1){
                sequence[ind] = i;
                ind++;
                for (int j = 0; j < m; j++){
                    Work[j] += allocation[i][j];
                    Finish[i] = true;
                }
                modified = true;
            }
       }
    }
    for (int i = 0; i < n; i++){
        if (Finish[i] == false){
            sequence[0] = -1;
            return sequence;
        }
    }
    return sequence;
}

int rq(int threadID, int arr[]){
    /*
        Request customer threadID be allocated arr[] resources.
    */
    for (int i = 0; i < m; i++){
        if ((allocation[threadID][i] + arr[i] > available[i]) 
        || (allocation[threadID][i] + arr[i] > maximum[threadID][i]) || (arr[i] < 0)){
            return -1;
        }
    }
    int *sequence;
    for (int x = 0; x < m; x++){
        allocation[threadID][x] += arr[x];
        available[x] -= arr[x];
        need[threadID][x] = maximum[threadID][x] - allocation[threadID][x];
    }
    sequence = safety();
    if (*sequence == -1){
        for (int x = 0; x < m; x++){
            allocation[threadID][x] -= arr[x];
            available[x] += arr[x];
            need[threadID][x] = maximum[threadID][x] - allocation[threadID][x];
        }
        return -1;
    }
    return 1;
}

int rl(int threadID, int arr[]){
    /*
        Request customer threadID be de-allocated arr[] resources.
    */
    for (int x = 0; x < m; x++){
        allocation[threadID][x] -= arr[x];
        available[x] += arr[x];
        need[threadID][x] = maximum[threadID][x] - allocation[threadID][x];
    }
    
}

void custCount(char *fileName){
    /*
        Idenities how many customers exist, and how many resources types there are. 
        Number of customers = n, number of resource types = m.
    */
    FILE *in = fopen(fileName, "r");
    char line[100];

	if(!in)
	{
		printf("Child A: Error in opening input file...exiting with error code -1\n");
        fclose(in);
	}

    
    while(!feof(in))
	{
        n++;
        fgets(line,100,in);
    }
    char *command = strtok(line,",");
    while (command != NULL){
        m++;
        command = strtok(NULL, ",");
    }
    fclose(in);
}
int readFile(char* fileName){
//Reads from the file and creates maximum array
    FILE *in = fopen(fileName, "r");
	if(!in)
	{
		printf("Child A: Error in opening input file...exiting with error code -1\n");
        fclose(in);
		return -1;
	}

    
    int row = 0;
    while(!feof(in))
	{
        int col = 0;
		char line[100];
		if(fgets(line,100,in)!=NULL) //reads line
		{
			char *command = strtok(line,",");
            while (command != NULL){
                maximum[row][col] = atoi(command);
                col++;
                command = strtok(NULL, ",");
            }
		}
        row++;
	}
    fclose(in);
    return 1;
}

void* threadRun(void* t){
    /*
        Runs the thread. Only called in a safe sequence, so each requests the maximum number
        of resources it needs
    */
    int* thread = (int*) t;
    int arr[m];
    int arr2[m];
    sem_wait(lock[*thread]); //The threa is now waiting with the mutex at its thread ID.
    printf("Thread %d executing:\n", *thread);

    for (int i = 0; i < m; i++){
        arr[i] = need[*thread][i];
    }
    rq(*thread, arr);
    for (int i = 0; i < m; i++){
        arr2[i] = allocation[*thread][i];
    }
    rl(*thread, arr2);    
    sem_post(&work);
    printf("Thread %d has completed:\n", *thread);
    pthread_exit(0);
}

void listResources(){
    /*
    Displays all matrices and arrays used.
    */
    printf("%s", "Available: \n");
    for (int i = 0; i < m; i++){
        printf("%d  " , available[i]);
    }
    printf("%s", "\n");

    printf("%s", "Maximum:       Allocation:    Need:             \n");
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            printf("%d  " , maximum[i][j]);
        }
        printf("%s", "   ");
        for (int j = 0; j < m; j++){
            printf("%d  " , allocation[i][j]);
        }
        printf("%s", "   ");
        for (int j = 0; j < m; j++){
            printf("%d  " , need[i][j]);
        }
        printf("%s", "\n");
    }
}

