/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

#include "common.h"
#include "queue.h" // Required for empty, enqueue, dequeue functions
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"
#include <string.h> // Required for strcmp function
#include <stdlib.h> // Required for free function

int __sys_killall(struct pcb_t *caller, struct sc_regs *regs)
{
    char proc_name[100]; // Holds the name of the process to terminate
    uint32_t data;
    uint32_t memrg = regs->a1; // Region ID pointing to the memory containing the process name

    /* Read the process name from the specified memory region */
    int i = 0;
    data = 0;
    while (data != -1)
    {
        libread(caller, memrg, i, &data);
        proc_name[i] = data;
        if (data == -1)
            proc_name[i] = '\0';
        i++;
    }
    printf("The process name retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* Iterate through the running_list */
    struct pcb_t *current_proc;
    while (!empty(caller->running_list))
    {                                                 // Correct: remove & to pass proper queue_t pointer
        current_proc = dequeue(caller->running_list); // Correct: remove & to pass proper queue_t pointer
        if (strcmp(current_proc->path, proc_name) == 0)
        {
            printf("Terminating process: %s\n", current_proc->path);
            free(current_proc); // Free the memory associated with the process
        }
        else
        {
            enqueue(caller->running_list, current_proc); // Correct: remove & to pass proper queue_t pointer
        }
    }

    /* Iterate through mlq_ready_queue if MLQ scheduler is enabled */
#ifdef MLQ_SCHED
    for (int prio = 0; prio < MAX_PRIO; prio++)
    {
        while (!empty(&caller->mlq_ready_queue[prio]))
        {                                                                             // Correct: use & to obtain queue_t pointer
            struct pcb_t *current_mlq_proc = dequeue(&caller->mlq_ready_queue[prio]); // Correct: use & to obtain queue_t pointer
            if (strcmp(current_mlq_proc->path, proc_name) == 0)
            {
                printf("Terminating MLQ process: %s\n", current_mlq_proc->path);
                free(current_mlq_proc); // Free the memory associated with the process
            }
            else
            {
                enqueue(&caller->mlq_ready_queue[prio], current_mlq_proc); // Correct: use & to obtain queue_t pointer
            }
        }
    }
#endif

    return 0;
}
