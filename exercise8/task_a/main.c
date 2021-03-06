
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

RT_TASK A_task;
RT_TASK B_task;
RT_TASK sync_task;

RT_SEM sync_sem;

int cpu_freq;

void creep_work(void *input)
{
	char* name = (char*) input;
	//unsigned long long endTime = rt_timer_read() + 10*1000*1000*1000; // 10 second timeout
	//unsigned long sem_timeout = cpu_freq*20; //20 seconds

	rt_printf(" %s waiting for semaphore..\n", name);
	rt_sem_p(&sync_sem, TM_INFINITE);
	rt_printf(" %s done waiting for semaphore..\n", name);
	/*if(rt_timer_read() > endTime)
	{
		rt_printf("Time expired\n");
		rt_task_delete(NULL);
		}
*/
    return;
}
void sync_work(void *input)
{
    // int rt_sem_create	(	RT_SEM * 	sem, const char* name, unsigned long icount,int 	mode )
    rt_printf("Creating and locking semaphore..\n");
    rt_sem_create(&sync_sem, "sync sem", 0, S_PRIO);
	rt_task_start(&A_task, &creep_work, (void *)"Task A");
	rt_task_start(&B_task, &creep_work, (void *)"Task B");

    
 //   unsigned long long endTime = rt_timer_read() + 10*1000*1000*1000; // 10 second timeout
  
      unsigned long duration = 1000*1000*1000; // 1000 msecond timeout

      rt_task_sleep((RTIME)duration);       
      rt_printf("Broadcasting/Unlocking semaphore..\n");
      rt_sem_broadcast(&sync_sem);
      rt_task_sleep((RTIME)duration);       
      //int rt_sem_delete	(	RT_SEM * 	sem	)
      rt_printf("Deleting semaphore..\n");
      rt_sem_delete	(&sync_sem);

      /*if(rt_timer_read() > endTime)
      {
  			rt_printf("Time expired\n");
  			rt_task_delete(NULL);
      }*/


    return;
}

int main(int argc, char *argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);
	cpu_freq = (int)((double)2.67*1000*1000*1000);
	//io_init();

	rt_print_auto_init(1);
	//rt_task_create (RT_TASK *task, const char *name, int stksize, int prio, int mode)
	rt_task_create(&sync_task, "Task sync", 0, 50, T_CPU(1));
	rt_task_create(&A_task, "Task A", 0, 30, T_CPU(1));
	rt_task_create(&B_task, "Task B", 0, 40, T_CPU(1));

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
