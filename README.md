# Memory-Management-Simulator, Measuring the Performance of Page Replacement Algorithms on Real Traces

First you must make sure all files are in the same directory/program. Then to run the program, you can use the following command: gcc -o memsim memsim.c algo.c or you may use the makefile. Both of these compile the program. Then you can use the following format to run the program based on the following options: 
FIFO/LRU (4 arguments): ./memsim <tracefile> <nframes> <lru|fifo> <debug|quiet>
VMS (5 arguments): memsim <tracefile(bzip or sixpacks)> <nframes> <lru|fifo|vms> <percent(0-100)> <debug|quiet>
This will run the program and outputs the total memory frames, number of events in trace, total reads, and total writes.
