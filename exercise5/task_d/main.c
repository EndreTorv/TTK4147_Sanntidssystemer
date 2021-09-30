#define _GNU_SOURCE
#include "io.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include <sched.h>
int set_cpu(int cpu_number);

struct timespec waketimeA;
struct timespec waketimeB;
struct timespec waketimeC;

void timespec_add(struct timespec *t, long us){
  t->tv_nsec += us*1000;
  if(t->tv_nsec > 1000000000){
    t->tv_nsec = t->tv_nsec - 1000000000;
    t->tv_sec += 1;
  }
}
//struct timespec period = {.tv_sec = 0, .tv_nsec = 500*1000*1000};
long int period = 1000;
void* threadA_work(){
  set_cpu(1);
  while(1){
    if (io_read(1) == 0)
    {
      io_write(1, 0);
      usleep(1);
      io_write(1, 1);
    }
    timespec_add(&waketimeA, period);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketimeA, NULL);
  }
}
void* threadB_work(){
  set_cpu(1);
  while(1){
    if (io_read(2) == 0)
    {
      io_write(2, 0);
      usleep(1);
      io_write(2, 1);
    }
    timespec_add(&waketimeB, period);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketimeB, NULL);
  }
}
void* threadC_work(){
  set_cpu(1);
  while(1){
    if (io_read(3) == 0)
    {
      io_write(3, 0);
      usleep(1);
      io_write(3, 1);
    }
    timespec_add(&waketimeV, period);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketimeC, NULL);
  }
}
void* threadDisturbance_work(){
  set_cpu(1);
  while(1){
    asm volatile("" ::: "memory");
  }
}

int main(){
  printf("Hello world\n");
  clock_gettime(CLOCK_REALTIME, &waketime);
  io_init();


  pthread_t threadA;
  pthread_t threadB;
  pthread_t threadC;
  pthread_t threadDisturbance[5];


  pthread_create(&threadA, NULL, &threadA_work, (void*)1);
  pthread_create(&threadB, NULL, &threadB_work, (void*)2);
  pthread_create(&threadC, NULL, &threadC_work, (void*)3);

  for (size_t i = 0; i < 5; i++) {
    pthread_create(&threadDisturbance[i], NULL, &threadDisturbance_work, (void*)0);
  }


  pthread_join(threadA, NULL);
  pthread_join(threadB, NULL);
  pthread_join(threadC, NULL);

  for (size_t i = 0; i < 5; i++) {
    pthread_join(threadDisturbance[i], NULL);
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
