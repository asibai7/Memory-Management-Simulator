#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "algo.h"
int i;
int isNumber(const char* str) { //function which I use later on to check that if the user chooses vms, then the fourth argument, the percentage must be a number greater than or equal 0 and less than or equal to 100
    for (i = 0; str[i] != '\0'; i++) 
    {
        if (!isdigit(str[i])) 
        {
            return 0;
        }
    }
    return 1;
}
int main(int argc, char* argv[]) 
{
    char* fileName = argv[1]; //assigns each argument the proper variable name
    int numFrames = atoi(argv[2]);
    char* algo = argv[3];
    char* mode = argv[argc - 1]; 
    if(numFrames <= 0) //checks that the number of frames in a page is valid
    {
        printf("Number of page frames must be more than 0!\n");
    }
    else if(strcmp(mode, "quiet") != 0 && strcmp(mode, "debug") != 0) //checks that mode is either quiet or debug
    {
        printf("The last argument is the mode, which must be quiet or debug!\n");
    }
    else
    {
        if (strcmp(algo, "fifo") == 0) //checks that the inputted algorithm is either fifo, lru, or vms. Calls the proper function with the parameters based on the input
        {
            fifo(fileName, numFrames, mode);
            printf("\n");
        }
        else if (strcmp(algo, "lru") == 0)
        {
            lru(fileName, numFrames, mode);
            printf("\n");
        }
        else if (strcmp(algo, "vms") == 0) 
        {
            if (argc == 4) 
            {
                printf("When using vms, you must enter a percentage.\n");
            }
            else 
            {
                char* percent = argv[4]; //if vms, checks that the fourth argument is a percent which is a number between 0 and 100
                if (!isNumber(percent)) 
                {
                    printf("Invalid percentage! The percentage must be a number.\n"); //error message if not a number
                }
                else {
                    int percentage = atoi(percent);
                    if (percentage < 0 || percentage > 100) 
                    {
                        printf("Invalid percentage! The percentage must be between 0 and 100.\n"); //error message if percent is not within 0 and 100
                    }
                    else 
                    {
                        vms(fileName, numFrames, percentage, mode);
                        printf("\n");
                    }
                }
            }
        }
        else 
        {
            printf("Choose from fifo, lru, or vms (segmented fifo).\n");
        }
    }
}