#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100   // Maximum number of processes that can be handled
#define MAX_CORES 10        // Maximum number of CPU cores

// Structure to represent a process
typedef struct {
    int startTime;          // Time when the process starts
    int cpuTimeUsed;        // Total CPU time used by the process
    int ssdAccesses;        // Number of SSD accesses
    int userInteractions;   // Number of user interactions
    char status[10];        // Status of the process (READY, RUNNING, BLOCKED, TERMINATED)
} Process;

// Structure to represent an event in the simulation
typedef struct {
    int eventTime;          // Time when the event occurs
    int processID;          // ID of the process associated with the event
    char eventType[20];     // Type of event (NEW, CPU_COMPLETE, SSD_COMPLETE, USER_COMPLETE)
} Event;

// Arrays to store process information and event queue
Process processTable[MAX_PROCESSES];
Event eventQueue[MAX_PROCESSES * 10];

int nCores = 0, freeCores = 0;   // Number of CPU cores and the count of free cores
int totalEvents = 0;             // Total number of events in the queue
int currentTime = 0;             // Current simulated time

// Function to add an event to the event queue
void addEvent(int time, int pid, const char* type) {
    eventQueue[totalEvents].eventTime = time;
    eventQueue[totalEvents].processID = pid;
    strcpy(eventQueue[totalEvents].eventType, type);
    totalEvents++;
}

// Function to sort the event queue by event times (chronological order)
void sortEvents() {
    int i, j;
    Event temp;

    // Simple bubble sort for sorting events based on time
    for (i = 0; i < totalEvents - 1; i++) {
        for (j = 0; j < totalEvents - i - 1; j++) {
            if (eventQueue[j].eventTime > eventQueue[j + 1].eventTime) {
                temp = eventQueue[j];
                eventQueue[j] = eventQueue[j + 1];
                eventQueue[j + 1] = temp;
            }
        }
    }
}

// Function to simulate the process scheduling
void simulate() {
    while (totalEvents > 0) {
        int i;
        Event event;
        int pid;

        // Sort events to handle the next event in chronological order
        sortEvents();
        event = eventQueue[0];  // Fetch the next event
       
        // Remove the event from the queue (shift remaining events)
        for (i = 0; i < totalEvents - 1; i++) {
            eventQueue[i] = eventQueue[i + 1];
        }
        totalEvents--;
       
        currentTime = event.eventTime;  // Update the current time to the event time
        pid = event.processID;          // Get the process ID associated with the event
       
        // Handle different types of events
        if (strcmp(event.eventType, "NEW") == 0) {
            // A new process starts
            if (freeCores > 0) {
                // If a core is free, assign it to the process
                freeCores--;
                strcpy(processTable[pid].status, "RUNNING");
                addEvent(currentTime + processTable[pid].cpuTimeUsed, pid, "CPU_COMPLETE");
            } else {
                // If no core is free, set the process status to READY
                strcpy(processTable[pid].status, "READY");
            }
        } else if (strcmp(event.eventType, "CPU_COMPLETE") == 0) {
            // A process completes its CPU execution step
            strcpy(processTable[pid].status, "BLOCKED");
            addEvent(currentTime + 1, pid, "SSD_COMPLETE");  // Assume SSD access takes 1ms
        } else if (strcmp(event.eventType, "SSD_COMPLETE") == 0) {
            // A process completes its SSD access
            processTable[pid].ssdAccesses++;
            strcpy(processTable[pid].status, "READY");
            if (freeCores > 0) {
                // Assign a free core to the process if available
                freeCores--;
                strcpy(processTable[pid].status, "RUNNING");
                addEvent(currentTime + processTable[pid].cpuTimeUsed, pid, "CPU_COMPLETE");
            }
        } else if (strcmp(event.eventType, "USER_COMPLETE") == 0) {
            // A process completes its user interaction
            processTable[pid].userInteractions++;
            strcpy(processTable[pid].status, "READY");
            if (freeCores > 0) {
                // Assign a free core to the process if available
                freeCores--;
                strcpy(processTable[pid].status, "RUNNING");
                addEvent(currentTime + processTable[pid].cpuTimeUsed, pid, "CPU_COMPLETE");
            }
        }
    }
}

int main() {
    int pid = 0;  // Process ID counter
    int i;
    char command[10];
   
    // Read the number of CPU cores from input
    scanf("NCORES %d", &nCores);
    freeCores = nCores;  // Initially, all cores are free

    while (1) {
        // Read the next command
        scanf("%s", command);
       
        if (strcmp(command, "NEW") == 0) {
            // Handle new process creation
            int startTime;
            scanf("%d", &startTime);
            processTable[pid].startTime = startTime;
            processTable[pid].cpuTimeUsed = 0;
            processTable[pid].ssdAccesses = 0;
            processTable[pid].userInteractions = 0;
            addEvent(startTime, pid, "NEW");
            pid++;
        } else if (strcmp(command, "CPU") == 0) {
            // Handle CPU time request
            int cpuTime;
            scanf("%d", &cpuTime);
            processTable[pid - 1].cpuTimeUsed += cpuTime;
        } else if (strcmp(command, "SSD") == 0) {
            // Handle SSD access request
            addEvent(currentTime + 1, pid - 1, "SSD_COMPLETE"); // SSD access trigger
        } else if (strcmp(command, "USER") == 0) {
            // Handle user interaction request
            addEvent(currentTime + 900, pid - 1, "USER_COMPLETE");  // Assume USER interaction takes 900ms
        } else if (strcmp(command, "END") == 0) {
            // End of input, break the loop
            break;
        }
    }
   
    // Start the simulation based on the events collected
    simulate();
   
    // Output the final process table and results
    for (i = 0; i < pid; i++) {
        printf("Process %d terminated at time %d\n", i, currentTime);
        printf("Start Time: %d, CPU Time Used: %d ms, SSD Accesses: %d, User Interactions: %d\n",
            processTable[i].startTime, processTable[i].cpuTimeUsed, processTable[i].ssdAccesses, processTable[i].userInteractions);
    }
   
    return 0;
}
