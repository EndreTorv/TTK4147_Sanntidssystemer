
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

void creep_work(void *input)
{
    char* channel = (char*) input;
    unsigned long duration = 100000000000; // 10 second timeout
    unsigned long endTime = rt_timer_read() + duration;

    //while (1)
    {
      rt_printf(" %s waiting for semaphore..\n", name);
      rt_sem_p();

      if(rt_timer_read() > endTime)
      {
        rt_printf("Time expired\n");
        rt_task_delete(NULL);
      }
    }
    return;
}
void sync_work(void *input)
{
    // int rt_sem_create	(	RT_SEM * 	sem, const char* name, unsigned long icount,int 	mode )
    rt_printf("Creating and locking semaphore..\n");
    rt_sem_create(&sync_sem, "sync sem", 1, S_PRIO); //icount = 1 : lock for syncronization?

    unsigned long duration = 10000000000; // 10 second timeout
    unsigned long endTime = rt_timer_read() + duration;
  //  while (1)
    {
      unsigned long duration = 100*1000*1000; // 100 msecond timeout

      rt_timer_spin(duration);        //busy wait
      rt_printf("Broadcasting/Unlocking semaphore..\n");
      rt_sem_broadcast(&sync_sem);
      rt_timer_spin(duration);        //busy wait
      //int rt_sem_delete	(	RT_SEM * 	sem	)
      rt_printf("Deleting semaphore..\n");
      rt_sem_delete	(&sync_sem);

      if(rt_timer_read() > endTime)
      {
  			rt_printf("Time expired\n");
  			rt_task_delete(NULL);
      }
    }

    return;
}

int main(int argc, char *argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);

	io_init();

	rt_print_auto_init(1);
  //rt_task_create (RT_TASK *task, const char *name, int stksize, int prio, int mode)
  rt_task_create(&sync_task, "Task sync", 0, 50, T_CPU(1));
  rt_task_create(&A_task, "Task A", 0, 30, T_CPU(1));
	rt_task_create(&B_task, "Task B", 0, 40, T_CPU(1));

  rt_task_start(&sync_task, &sync_work, (void *)"Task sync");
	rt_task_start(&A_task, &creep_work, (void *)"Task A");
	rt_task_start(&B_task, &creep_work, (void *)"Task B");

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
