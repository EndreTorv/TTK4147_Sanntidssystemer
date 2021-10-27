
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
#include <native/mutex.h>
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
RT_TASK H_task;
RT_TASK sync_task;

RT_MUTEX A_MUTEX;
RT_MUTEX B_MUTEX;

int cpu_freq;
const int L_Original_Prio = 30;
const int H_Original_Prio = 50;
const int L_Boosted_Prio = 60;
const int H_Boosted_Prio = 70;

const unsigned long long duration = 100*1000*1000; // 100 msecond timeout

void L_work(void *input)
{
	char* name = (char*) input;
	
	rt_mutex_acquire(&A_MUTEX, TM_INFINITE);
	rt_printf(" %s locked resource A\n", name);
	rt_task_set_priority(&L_task, L_Boosted_Prio);
	rt_printf(" %s changed priority to %.2d\n", name, L_Boosted_Prio);
	for (int i = 0; i < 3; i++){
		rt_printf(" %s Busywaiting\n", name);
	      	rt_timer_spin((RTIME)(duration));        //busy wait
	}
	rt_mutex_acquire(&B_MUTEX, TM_INFINITE);
	rt_printf(" %s locked resource B\n", name);
	for (int i = 0; i < 3; i++){
		rt_printf(" %s Busywaiting\n", name);
	      	rt_timer_spin((RTIME)(duration));        //busy wait
	}
	rt_printf(" %s opens resource B\n", name);
	rt_mutex_release(&B_MUTEX);
	rt_printf(" %s opens resource A\n", name);
	rt_mutex_release(&A_MUTEX);
	rt_task_set_priority(&L_task, L_Original_Prio);
	rt_printf(" %s changed priority to %.2d\n", name, L_Original_Prio);
	rt_timer_spin((RTIME)(duration));        //busy wait	

	rt_printf(" %s done\n", name);
	
    return;
}

void H_work(void *input)
{
	char* name = (char*) input;
	
	rt_task_sleep((RTIME)(duration));	
	rt_mutex_acquire(&B_MUTEX, TM_INFINITE);
	rt_printf(" %s locked resource B\n", name);
	rt_task_set_priority(&H_task, H_Boosted_Prio);
	rt_printf(" %s changed priority to %.2d\n", name, H_Boosted_Prio);
	rt_printf(" %s busywaiting\n", name);
	rt_timer_spin((RTIME)(duration));        //busy wait

	rt_mutex_acquire(&A_MUTEX, TM_INFINITE);
	rt_printf(" %s locked resource A\n", name);
	for (int i = 0; i < 2; i++){
		rt_printf(" %s busywaiting\n", name);
	      	rt_timer_spin((RTIME)(duration));        //busy wait
	}

	rt_printf(" %s opens resource A\n", name);	
	rt_mutex_release(&A_MUTEX);

	rt_printf(" %s opens resource B\n", name);
	rt_mutex_release(&B_MUTEX);
	rt_task_set_priority(&H_task, H_Original_Prio);
	rt_printf(" %s changed priority to %.2d\n", name, H_Original_Prio);
	rt_printf(" %s done\n", name);
	
    return;
}
void sync_work(void *input)
{
	// int rt_sem_create	(	RT_SEM * 	sem, const char* name, unsigned long icount,int 	mode )
	rt_printf("Creating and locking semaphore..\n");
	rt_mutex_create(&A_MUTEX, "A_MUTEX");
	rt_mutex_create(&B_MUTEX, "B_MUTEX");
	rt_task_start(&H_task, &H_work, (void *)"Task H");
	rt_task_start(&L_task, &L_work, (void *)"Task L");
	for (int i = 0; i < 30; i++){
		rt_printf("tick!\n");
	      rt_task_sleep((RTIME)(duration));        //sleep. yield cpu.
	}      
	rt_printf("Broadcasting/Deleting mutex..\n");
	rt_mutex_delete(&A_MUTEX);
	rt_mutex_delete(&B_MUTEX);



    return;
}

int main(int argc, char *argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);
	cpu_freq = (int)((double)2.67*1000*1000*1000);



	rt_print_auto_init(1);
	//rt_task_create (RT_TASK *task, const char *name, int stksize, int prio, int mode)
	rt_task_create(&sync_task, "Task sync", 0, 99, T_CPU(1));

	rt_task_create(&L_task, "Task L", 0, L_Original_Prio, T_CPU(1));
	rt_task_create(&H_task, "Task H", 0, H_Original_Prio, T_CPU(1));

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
