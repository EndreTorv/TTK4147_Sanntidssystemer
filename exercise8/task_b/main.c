
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

//#include "io.h"


int set_cpu(int cpu_number);
void *disturbance(void* input);
void printStuff(void* args);

// ------------------------------------------------------------------------------------------
/*
    rt_sem_p()  - decrement semaphore, waits for semaphore
    rt_sem_v()  - increment semaphore, releases semaphore
    rt_sem_broadcast()  - signal all tasks that are waiting for a semaphore

*/

RT_TASK L_task;
RT_TASK M_task;
RT_TASK H_task;
RT_TASK sync_task;

RT_SEM sync_sem;

int cpu_freq;

int shared_resource;
const unsigned long long duration = 100*1000*1000; // 100 msecond timeout

void L_work(void *input)
{
	char* name = (char*) input;
	rt_sem_p(&sync_sem, TM_INFINITE);
	rt_printf(" %s locked resource\n", name);
	for (int i = 0; i < 3; i++){
	      	rt_timer_spin((RTIME)(duration));        //busy wait
	}
	rt_printf(" %s opens resource\n", name);
	rt_sem_v(&sync_sem);
	rt_printf(" %s done\n", name);
	
    return;
}

void M_work(void *input)
{
	char* name = (char*) input;
	rt_task_sleep((RTIME)duration);	
	
	for (int i = 0; i < 5; i++){
		rt_timer_spin((RTIME)(duration));        //busy wait
	}	
	
	rt_printf(" %s done\n", name);
	
	
    return;
}

void H_work(void *input)
{
	char* name = (char*) input;
	for (int i = 0; i < 2; i++){
		rt_task_sleep((RTIME)(duration));
	}	
	rt_sem_p(&sync_sem, TM_INFINITE);
	rt_printf(" %s locked resource\n", name);
	for (int i = 0; i < 2; i++){
	      	rt_timer_spin((RTIME)(duration));        //busy wait
	}
	rt_printf(" %s opens resource\n", name);	
	rt_sem_v(&sync_sem);
	rt_printf(" %s done\n", name);
	
    return;
}
void sync_work(void *input)
{
	// int rt_sem_create	(	RT_SEM * 	sem, const char* name, unsigned long icount,int 	mode )
	rt_printf("Creating and locking semaphore..\n");
	rt_sem_create(&sync_sem, "sync sem", 1, S_PRIO);
	rt_task_start(&H_task, &H_work, (void *)"Task H");
	rt_task_start(&M_task, &M_work, (void *)"Task M");
	rt_task_start(&L_task, &L_work, (void *)"Task L");
	for (int i = 0; i < 30; i++){
	      rt_task_sleep((RTIME)(duration));        //sleep. yield cpu.
	}      
	rt_printf("Broadcasting/Unlocking semaphore..\n");
	rt_sem_broadcast(&sync_sem);
	rt_task_sleep((RTIME)duration);         //sleep..
	//int rt_sem_delete	(	RT_SEM * 	sem	)
	rt_printf("Deleting semaphore..\n");
	rt_sem_delete	(&sync_sem);



    return;
}

int main(int argc, char *argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);
	cpu_freq = (int)((double)2.67*1000*1000*1000);


	shared_resource = 1;


	rt_print_auto_init(1);
	//rt_task_create (RT_TASK *task, const char *name, int stksize, int prio, int mode)
	rt_task_create(&sync_task, "Task sync", 0, 70, T_CPU(1));

	rt_task_create(&L_task, "Task L", 0, 30, T_CPU(1));
	rt_task_create(&M_task, "Task M", 0, 40, T_CPU(1));
	rt_task_create(&H_task, "Task H", 0, 50, T_CPU(1));

  	rt_task_start(&sync_task, &sync_work, (void *)"Task sync");
	
	/*
	int num_Disturb = 10;
	pthread_t threads[num_Disturb];
	for (int i = 1; i <= num_Disturb; i++) {
		pthread_create(&threads[i], NULL, disturbance, NULL);
	}*/

	while(1){}
	rt_printf("THE END\n");
	return 0;
}




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
