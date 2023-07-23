#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fifo(char* fileName, int numFrames, char* setting) { //fifo function
    int* ptable = (int*)malloc(numFrames * sizeof(int)); //dedicating necessary memory space
    char* mem = (char*)malloc(numFrames * sizeof(char));
    int events = 0; //initializing variables
    int r = 0;
    int w = 0;
    unsigned a = 0;
    char rw = ' ';
    FILE* file = fopen(fileName, "r"); //opening trace file 
    if (file == NULL) { //necessary to make sure file was opened successfully so traces can start getting read
        printf("Cannot open %s, please try again!\n", fileName);
        free(ptable);
        free(mem);
    }
    while (fscanf(file, "%x %c", &a, &rw) != EOF) { //loop to read traces from file until the end is reached
        a = a / 4096; //addresss must be divided by 4096 to match the 4 KB page size
        int found = 0;
        int i;
        for ( i = 0; i < numFrames; i++) { //loop to check each element of the page table array and whether there is a hit with the current address value
            if (ptable[i] == a) { //if found in array, then found is 1 and next trace is checked
                found = 1;
                break;
            }
        }
        if (found == 0) { //if not found in array, then read is incremented as it will now be added to array
            r++;
            if (events < numFrames) { //checks for space in memory, if it is available, then it is pushed to front
                ptable[events] = a; 
                mem[events] = rw;
            }
            else { //if race is a write operation then that is incremented, removing oldest entry from array (memory)
                if (mem[0] == 'W') {
                    w++;
                }
                int i;
                for ( i = 0; i < numFrames - 1; i++) {
                    ptable[i] = ptable[i + 1];
                    mem[i] = mem[i + 1];
                }
                ptable[numFrames - 1] = a;
                mem[numFrames - 1] = rw;
            }
        }
        else { //if trace is a read operation, it updates value to R in rw
            int i;
            for (i = 0; i < numFrames; i++) {
                if (ptable[i] == a && mem[i] == 'R') {
                    mem[i] = rw;
                    break;
                }
            }
        }
        events++;
    }
    fclose(file); //close the trace file
    printf("Total memory frames: %d\n", numFrames); //output format
    printf("Events in trace: %d\n", events);
    printf("Total disk reads: %d\n", r);
    printf("Total disk writes: %d\n", w);
}
void lru(char* fileName, int numFrames, char* setting) { //lru function
    int* ptable_addr = NULL; //initializing variables
    char* ptable_rw = NULL;
    int* mem_addr = NULL;
    char* mem_rw = NULL;
    int ptable_size = 0;
    int mem_size = 0;
    int events = 0;
    int r = 0;
    int w = 0;
    unsigned a = 0;
    char rw = ' ';
    FILE* file = fopen(fileName, "r"); // Reading from the file
    if (file == NULL) { //checking that file can be opened
        printf("Cannot open %s, please try again!\n", fileName);
        return;
    }
    ptable_addr = (int*)malloc(numFrames * sizeof(int));
    ptable_rw = (char*)malloc(numFrames * sizeof(char));
    mem_addr = (int*)malloc(numFrames * sizeof(int));
    mem_rw = (char*)malloc(numFrames * sizeof(char));
    if (ptable_addr == NULL || ptable_rw == NULL || mem_addr == NULL || mem_rw == NULL) { //checking that memory was successfully allocated
        printf("Memory allocation failed. Exiting...\n");
        fclose(file);
        return;
    }
    while (fscanf(file, "%x %c", &a, &rw) != EOF) { //reads from file until the end is reached
        a = a / 4096; //addresss must be divided by 4096 to match the 4 KB page size
        // Find the address in page table
        int found = 0;
        int i;
        for (i = 0; i < ptable_size; i++) {
            if (ptable_addr[i] == a) {
                found = 1;
                break;
            }
        }
        if (!found) { // If address was not found in memory, incrememnt read because you have to read address into memory
            r++;
            if (mem_size < numFrames) { //checks if there is space
                mem_addr[mem_size] = a; //add to memory and page table
                mem_rw[mem_size] = rw;
                mem_size++;
                ptable_addr[ptable_size] = a;
                ptable_rw[ptable_size] = rw;
                ptable_size++;
            } else { //if address is W, incrememnt writes
                if (mem_rw[0] == 'W') {
                    w++;
                }
                for (i = 0; i < mem_size - 1; i++) {
                    mem_addr[i] = mem_addr[i + 1];
                    mem_rw[i] = mem_rw[i + 1];
                }
                mem_size--;
                for (i = 0; i < ptable_size - 1; i++) {
                    ptable_addr[i] = ptable_addr[i + 1];
                    ptable_rw[i] = ptable_rw[i + 1];
                }
                ptable_size--; 
                mem_addr[mem_size] = a; 
                mem_rw[mem_size] = rw;
                mem_size++;
                ptable_addr[ptable_size] = a;
                ptable_rw[ptable_size] = rw;
                ptable_size++;
            }
        } else { //erase the oldest address from memory
            for (i = 0; i < ptable_size; i++) {
                if (ptable_addr[i] == a) {
                    break;
                }
            }
            for (; i < ptable_size - 1; i++) {
                ptable_addr[i] = ptable_addr[i + 1];
                ptable_rw[i] = ptable_rw[i + 1];
            }
            ptable_size--;
            ptable_addr[ptable_size] = a;
            ptable_rw[ptable_size] = rw;
            ptable_size++;
            for (i = 0; i < mem_size; i++) {
                if (mem_addr[i] == a) {
                    mem_rw[i] = rw;
                    break;
                }
            }
        }
        events++;
    }
    fclose(file); //close the trace file
    printf("Total memory frames: %d\n", numFrames); //output format
    printf("Events in trace: %d\n", events);
    printf("Total disk reads: %d\n", r);
    printf("Total disk writes: %d\n", w);
    free(ptable_addr); //free up all the memory
    free(ptable_rw);
    free(mem_addr);
    free(mem_rw);
}
void vms(char* fileName, int numFrames, int percent, char* setting) {
    int* ptable1; // primary is for fifo, secondary is for lru
    int* ptable2; 
    int* mem1;
    int* mem2;
    int ptable1_count = 0;
    int ptable2_count = 0;
    int mem1_count = 0;
    int mem2_count = 0;
    int events = 0;
    int r = 0;
    int w = 0;
    int numFrames2 = (numFrames * percent) / 100; // formula for lru frames num
    int numFrames1 = numFrames - numFrames2; // formula for fifo frames num
    if (percent == 0 || numFrames2 == 0) { //if percent is 0, then fifo is called on 100% of all traces
        fifo(fileName, numFrames, setting);
        return;
    }
    if (percent == 100 || numFrames1 == 0) { //if percent is 100, then lruis called on 100% of all traces
        lru(fileName, numFrames, setting);
        return;
    }
    unsigned a = 0;
    char rw = ' ';
    FILE* file = fopen(fileName, "r"); //open file to read traces from it
    if (file == NULL) { //checks that file can be opened
        printf("Cannot open %s, please try again!\n", fileName);
        return;
    }
    ptable1 = (int*)malloc(numFrames1 * sizeof(int)); //allocating necessary memory
    ptable2 = (int*)malloc(numFrames2 * sizeof(int));
    mem1 = (int*)malloc(numFrames1 * sizeof(int));
    mem2 = (int*)malloc(numFrames2 * sizeof(int));
    if (ptable1 == NULL || ptable2 == NULL || mem1 == NULL || mem2 == NULL) {
        printf("Memory allocation failed!\n");
        fclose(file); // Close the file before returning
        return;
    }
    while (fscanf(file, "%x %c", &a, &rw) != EOF) { //reads from file until the end is reached
        a = a / 4096; //addresss must be divided by 4096 to match the 4 KB page size
        int fifo_index = -1;
        int i;
        for (i = 0; i < mem1_count; i++) { //finds the index of the given address in primary fifo buffer if there is a match
            if (mem1[i] == a) {
                fifo_index = i;
                break;
            }
        }
        if (fifo_index == -1) { //if address is not in primary fifo buffer
            if (mem1_count < numFrames1) { //check if there is space in fifo buffer
                r++; 
                mem1[mem1_count] = a;
                ptable1[ptable1_count] = a;
                mem1_count++;
                ptable1_count++;
            }
            else if (mem2_count < numFrames2) { //if primary fifo buffer is full and secondary lru buffer is not, check if there is space for the address
                mem2[mem2_count] = a;
                ptable2[ptable2_count] = a;
                mem2_count++;
                ptable2_count++;
            }
            else { //if both buffers are full
                r++; 
                if (rw == 'W') {
                    w++;
                }
                int evictPage = ptable1[0];
                int evictIndex = 0;
                for (i = 1; i < numFrames1; i++) {
                    ptable1[i - 1] = ptable1[i];
                    if (ptable1[i] == evictPage) {
                        evictIndex = i;
                    }
                }
                ptable1[numFrames1 - 1] = a;
                mem1[evictIndex] = a;
                int lruEvictPage = ptable2[0];
                int lruEvictIndex = 0;
                for (i = 1; i < numFrames2; i++) {
                    ptable2[i - 1] = ptable2[i];
                    if (ptable2[i] == lruEvictPage) {
                        lruEvictIndex = i;
                    }
                }
                ptable2[numFrames2 - 1] = lruEvictPage;
                mem2[lruEvictIndex] = lruEvictPage;
            }
        }
        else { //if address is in primary fifo buffer
            int lru_index = -1;
            for (i = 0; i < mem2_count; i++) {
                if (mem2[i] == a) {
                    lru_index = i;
                    break;
                }
            }
            if (lru_index != -1) {
                int lruEvictPage = ptable2[lru_index];
                int lruEvictIndex = lru_index;
                for (i = lru_index + 1; i < mem2_count; i++) {
                    ptable2[i - 1] = ptable2[i];
                }
                ptable2[mem2_count - 1] = lruEvictPage;
                mem2[lruEvictIndex] = lruEvictPage;
            }
        }
        events++;
    }
    fclose(file); // Close the file
    printf("Total memory frames: %d\n", numFrames); //output format
    printf("Events in trace: %d\n", events);
    printf("Total disk reads: %d\n", r);
    printf("Total disk writes: %d\n", w);
    free(ptable1);
    free(ptable2);
    free(mem1);
    free(mem2);
}