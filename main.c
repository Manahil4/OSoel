#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 20
#define NO_OF_FRAMES 20
#define PG_SIZE 1
#define NO_OF_PROCESSES 10

int pgsInProcesses[NO_OF_PROCESSES];
int pgRemainingList[NO_OF_PROCESSES];
int no_Of_pages;
int last_index_of_process;
int start_Process;
int pgToAllocate;

struct pgDict
{
    int pgNO;
    int processNo;
    int arrivalTime;
} memRepresentation[NO_OF_FRAMES], VM[100]; // vm is virtual mem and memRepresentation is Ram, both stoores pgs of each processes, memRepresentation not store all pgs , but we've to bring remaining ps in memRepresentation after displaying current one

// for storing processes from processsFile.txt0
struct process
{
    int processId;
    int processSize;
    int arrivalTime;

} processes[NO_OF_PROCESSES];

struct framesAllocated
{
    int processId;
    int StartingPgNo;
    int EndingPgNo;
    int StartingMemFrame;
} frames[NO_OF_PROCESSES]; // tracking processes and there pg s in mem, for frame allocation and deallocation

// helper functions
int readFromFile();
void viewMem();
void newTOReadyOrReadySuspend(int i, int limit, struct pgDict MEM[], int processNo);
void assigningLeftOverPgs();
void Swapping(int freeFrame);
int findFreeFrame()
{
    int count = 0;
    for (int i = 0; i < NO_OF_FRAMES; i++)
    {
        if (memRepresentation[i].processNo == 0 && memRepresentation[i].pgNO == 0 && i!=0)
        {
            count++;
        }
    }
    return count;
}

int readFromFile()
{
    FILE *file = fopen("processFile.txt", "r");
    for (int i = 0; i < NO_OF_PROCESSES; i++)
    {
        fscanf(file, "%d %d %d ",
               &processes[i].processId,
               &processes[i].processSize,
               &processes[i].arrivalTime);
    }
}

// printing memRepresentation for showing how frames assign to each process's pg in mem
void viewMem()
{
    // Print table header
    printf("\nMemory Representation:\n");
    printf("+------------+-------+--------------+\n");
    printf("| Process No | Page  | Arrival Time |\n");
    printf("+------------+-------+--------------+\n");

    // Print each row with proper alignment
    for (int i = 0; i < NO_OF_FRAMES; i++)
    {
        printf("| %-10d | %-5d | %-12d |\n",
               memRepresentation[i].processNo,
               memRepresentation[i].pgNO,
               memRepresentation[i].arrivalTime);
    }

    // Print table footer
    printf("+------------+-------+--------------+\n");
}

void newTOReadyOrReadySuspend(int i, int limit, struct pgDict MEM[], int processNo)
{
    for (int j = i; j < limit + i; j++)
    {

        MEM[j].pgNO = j - i;
        MEM[j].processNo = processNo;
        // as each processno is seq and arrival time is also seq, as i assume at each instance only 1 process can enter the system
        MEM[j].arrivalTime = processNo;
    }
}

void Swapping(int freeFrame) {
    int new_pg, replace_index, allocatedFrames;
    int LastFreeSlots = freeFrame;

    for (int i = 0; i < NO_OF_PROCESSES; i++) {
        if (pgRemainingList[i] == 0) {
            printf("\nNo pg to Swap in process %d\n", i);
        } else {
            printf("\nSwapping %d pgs of process %d one by one\n", pgRemainingList[i], i);

            allocatedFrames = frames[i].EndingPgNo - frames[i].StartingPgNo + 1;  // original allocation only

            int j = 0;

            // --- First use free frames ---
            for (; j < pgRemainingList[i] && LastFreeSlots > 0; j++) {
                replace_index = frames[NO_OF_PROCESSES - 1].EndingPgNo + frames[NO_OF_PROCESSES - 1].StartingMemFrame + (j % freeFrame);
                LastFreeSlots--;

                new_pg = frames[i].EndingPgNo + j;
                memRepresentation[replace_index].pgNO = new_pg;
                memRepresentation[replace_index].processNo = i;
                memRepresentation[replace_index].arrivalTime = processes[i].arrivalTime;

                printf("\nSwapping pg %d by %d of process %d into free frame\n", j, new_pg, frames[i].processId);
                viewMem();
            }

            if (LastFreeSlots == 0 && j < pgRemainingList[i]) {
                printf("\nNo free frame available for swapping, switching to process's original frames...\n");
            }

            // --- Then use original memory frames in circular manner ---
            for (int k = 0; j < pgRemainingList[i]; j++, k++) {
                replace_index = frames[i].StartingMemFrame + (k % allocatedFrames);

                new_pg = frames[i].EndingPgNo + j;
                memRepresentation[replace_index].pgNO = new_pg;
                memRepresentation[replace_index].processNo = i;
                memRepresentation[replace_index].arrivalTime = processes[i].arrivalTime;

                printf("\nSwapping pg %d by %d of process %d into original frame\n", k, new_pg, frames[i].processId);
                viewMem();
            }

            printf("Successfully swapped all remaining pgs of process %d\n", i);
        }
    }
}

void assigningLeftOverPgs()
{

    printf("\n Now, swapping remaining pgs of each Process as we've allocated all processes in mem\n");
    int freeFrame = findFreeFrame();
            if (freeFrame != 0)
            {
                printf("\n free frame available for swapping%d\n", freeFrame);
            }
            else{
                printf("\nNo free frame available for swapping, so here Swapping means replacement of initially allocated pgs\n");
                
            }
            Swapping(freeFrame);
    }

int main()
{
    int limit, fixedStart;
    printf("Welcome to memory Module\n\n");

    readFromFile();
    printf("Initially our memory is empty, all its frames storing initial value 0, have a look \n");
    viewMem();
    // finding no of pgsin each process and summing up all
    for (int j = 0; j < NO_OF_PROCESSES; j++)
    {
        pgsInProcesses[j] = processes[j].processSize / PG_SIZE;
        no_Of_pages += pgsInProcesses[j];
    }
    // _______________ Initial MEM ALLOCATION

    // case; if toatalno of pgs are more than frames then initially a fix no of pgs( pgToAllocate) of each process allocate in mem then as soon discuss below

    // pgToAllocate ; allocation of no of pg per process
    pgToAllocate = NO_OF_FRAMES / NO_OF_PROCESSES;
    // iterating through each process
    for (int i = 1; i <= NO_OF_PROCESSES; i++)
    {
        start_Process = last_index_of_process;

        frames[i - 1].StartingMemFrame = start_Process;
        frames[i - 1].processId = i - 1;
        frames[i - 1].StartingPgNo = 0;

        // for avoiding fragmentation, continue just next where last process is allocated
        fixedStart = ((i - 1) * pgToAllocate);
        if (fixedStart > start_Process)
        {
            limit = fixedStart - start_Process + pgToAllocate;
        }
        else
        {
            limit = pgToAllocate;
        }
        if (limit > pgsInProcesses[i - 1])
        { // allocating pgs in ram and vm

            // void newTOReadyOrReadySuspend(int i, int limit, struct pgDict MEM[], int processNo)
            // {
            //     for (int j = i; j < limit + i; j++)
            //     {

            //         MEM[j].pgNO = j - i;
            //         MEM[j].processNo = processNo;
            //         // as each processno is seq and arrival time is also seq, as i assume at each instance only 1 process can enter the system
            //         MEM[j].arrivalTime = processNo;
            //     }
            // }
            newTOReadyOrReadySuspend(start_Process, pgsInProcesses[i - 1], memRepresentation, i - 1);
            newTOReadyOrReadySuspend(start_Process, pgsInProcesses[i - 1], VM, i - 1);
            last_index_of_process += pgsInProcesses[i - 1];
            frames[i - 1].EndingPgNo = pgsInProcesses[i - 1];
            printf("\nAllocated pg(s) of process %d in Ram and in VM are %d\n", i - 1, pgsInProcesses[i - 1]);
        }

        else
        {

            // allocating pgs in ram partially and fully in vm

            newTOReadyOrReadySuspend(start_Process, limit, memRepresentation, i - 1);
            newTOReadyOrReadySuspend(start_Process, pgsInProcesses[i - 1], VM, i - 1);
            last_index_of_process += limit;
            pgRemainingList[i - 1] = pgsInProcesses[i - 1] - limit;
            printf("\nAllocated pg(s) of process %d in Ram and in VM are %d\n", i - 1, limit);
            printf("\nRemaing pg(s) of process %d are %d\n", i - 1, pgRemainingList[i - 1]);
            frames[i - 1].EndingPgNo = limit;
        }
        viewMem();
    }

    assigningLeftOverPgs();
}