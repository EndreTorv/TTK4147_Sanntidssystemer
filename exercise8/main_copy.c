
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <native/task.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <native/types.h>
#include <sched.h>
#include <stdlib.h>
#include <pthread.h>
#include <native/timer.h>
#include <native/sem.h>

#include "io.h"

RT_TASK A_task;
RT_TASK B_task;
RT_TASK C_task;

int set_cpu(int cpu_number)
{
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void *disturbance(void* input) {
    set_cpu(1);
    while (1)
    {
        asm volatile("" ::: "memory");
    }
    return NULL;
}

void printStuff(void* args) {
	while (1) {
		rt_printf("	_(X.O)_	");
		fflush(stdout);
		rt_task_wait_period(NULL);
	}
}

void busyWait(void *input)
{

    long channel = (long) input;
    unsigned long duration = 100000000000; // 100 second timeout
    unsigned long endTime = rt_timer_read() + duration;
   	rt_task_set_periodic(NULL, TM_NOW, 1000*1000);
    while (1)
    {
        if (io_read(channel) == 0)
        {
            io_write(channel, 0);
            rt_timer_spin(1000);
            io_write(channel, 1);
        }
        if(rt_timer_read() > endTime)
        {
			rt_printf("Time expired\n");
			rt_task_delete(NULL);
        }
        if(rt_task_yield())
        {
			rt_printf("Task failed to yield\n");
			rt_task_delete(NULL);
        }
        rt_task_wait_period(NULL);
    }
}

int main(int argc, char *argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);

	io_init();

	rt_print_auto_init(1);
	rt_task_create(&A_task, "Task A", 0, 50, T_CPU(1));
	rt_task_create(&B_task, "Task B", 0, 50, T_CPU(1));
	rt_task_create(&C_task, "Task C", 0, 50, T_CPU(1));



	rt_task_start(&A_task, &busyWait, (void *)1);
	rt_task_start(&B_task, &busyWait, (void *)2);
	rt_task_start(&C_task, &busyWait, (void *)3);
	/*
	int num_Disturb = 10;
	pthread_t threads[num_Disturb];
	for (int i = 1; i <= num_Disturb; i++) {
		pthread_create(&threads[i], NULL, disturbance, NULL);
	}*/

	while (1) {

	}
	return 0;
}
