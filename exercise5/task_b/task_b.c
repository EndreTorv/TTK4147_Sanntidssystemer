#define _GNU_SOURCE
#include "io.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include <sched.h>
int set_cpu(int cpu_number);

void* threadA_work(){
  set_cpu(1);
  while(1){
    if (io_read(1) == 0)
    {
      io_write(1, 0);
      usleep(1);
      io_write(1, 1);
    }
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
  }
}

int main(){
  printf("Hello world\n");
  io_init();


  pthread_t threadA;
  pthread_t threadB;
  pthread_t threadC;


  pthread_create(&threadA, NULL, &threadA_work, (void*)1);
  pthread_create(&threadB, NULL, &threadB_work, (void*)2);
  pthread_create(&threadC, NULL, &threadC_work, (void*)3);


  pthread_join(threadA, NULL);
  pthread_join(threadB, NULL);
  pthread_join(threadC, NULL);

  return 0;
}


int set_cpu(int cpu_number){
cpu_set_t cpu;
CPU_ZERO(&cpu);
CPU_SET(cpu_number, &cpu);
return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}
