/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

#include "common.h"
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"

int __sys_killall(struct pcb_t *caller, struct sc_regs *regs)
{
    char proc_name[100];
    uint32_t data;

    // hardcode for demo only
    uint32_t memrg = regs->a1;

    /* TODO: Get name of the target proc */
    // proc_name = libread..
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
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* TODO: Traverse proclist to terminate the proc
     *       stcmp to check the process match proc_name
     */
    // caller->running_list
    // caller->mlq_ready_queu

    /* TODO Maching and terminating
     *       all processes with given
     *        name in var proc_name
     */
    /* Traverse running_list to terminate matching processes */
    struct pcb_t *current_proc = caller->running_list;
    struct pcb_t *prev_proc = NULL;

    while (current_proc != NULL)
    {
        if (strcmp(current_proc->name, proc_name) == 0)
        {
            printf("Terminating process: %s\n", current_proc->name);

            // Free resources
            if (prev_proc != NULL)
            {
                prev_proc->next = current_proc->next;
            }
            else
            {
                caller->running_list = current_proc->next;
            }
            free(current_proc);

            // Move to next process
            current_proc = (prev_proc != NULL) ? prev_proc->next : caller->running_list;
        }
        else
        {
            prev_proc = current_proc;
            current_proc = current_proc->next;
        }
    }

    /* Traverse mlq_ready_queue (if MLQ Scheduler is enabled) */
#ifdef MLQ_SCHED
    for (int prio = 0; prio < MAX_PRIO; prio++)
    {
        struct pcb_t *current_mlq_proc = caller->mlq_ready_queue[prio].head;
        struct pcb_t *prev_mlq_proc = NULL;

        while (current_mlq_proc != NULL)
        {
            if (strcmp(current_mlq_proc->name, proc_name) == 0)
            {
                printf("Terminating MLQ process: %s\n", current_mlq_proc->name);

                // Free resources
                if (prev_mlq_proc != NULL)
                {
                    prev_mlq_proc->next = current_mlq_proc->next;
                }
                else
                {
                    caller->mlq_ready_queue[prio].head = current_mlq_proc->next;
                }
                free(current_mlq_proc);

                // Move to next process
                current_mlq_proc = (prev_mlq_proc != NULL) ? prev_mlq_proc->next : caller->mlq_ready_queue[prio].head;
            }
            else
            {
                prev_mlq_proc = current_mlq_proc;
                current_mlq_proc = current_mlq_proc->next;
            }
        }
    }
#endif
    return 0;
}
