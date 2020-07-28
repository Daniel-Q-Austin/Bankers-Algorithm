
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

void listResources();
int readFile(char* fileName);
void custCount(char *fileName);

int* available;
int** maximum;
int** allocation;
int** need;
char input[100] = { 0 };

int main(int argc, char *argv[]){
    custCount(file);


    printf("BEGIN%s","\n");

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
