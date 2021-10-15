#define _GNU_SOURCE
#include "io.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <sched.h>
int set_cpu(int cpu_number);
sem_t globalis_sem;
long globalis = 0;

const long count = 50*1000*1000;
void* threadA_work(){
  set_cpu(1);
  long lokalis = 0;
  for (size_t i = 0; i < count; i++) {
    sem_wait(&globalis_sem);
    globalis++;
    sem_post(&globalis_sem);
    lokalis++;
  }
  printf("Tread A: globalis %.6li    lokalis: %.6li", globalis, lokalis);
  return &threadA_work;
}
void* threadB_work(){
  set_cpu(1);
  long lokalis = 0;
  for (size_t i = 0; i < count; i++) {
    sem_wait(&globalis_sem);
    globalis++;
    sem_post(&globalis_sem);
    lokalis++;
  }
  printf("Tread B: globalis %.6li    lokalis: %.6li", globalis, lokalis);
  return &threadB_work;
}

void* threadDisturbance_work(){
  set_cpu(1);
  while(1){
    asm volatile("" ::: "memory");
  }
}

int main(){
  printf("Hello world\n");
  io_init();
  sem_init(&globalis_sem, 1, 1);

  pthread_t threadA;
  pthread_t threadB;



  pthread_create(&threadA, NULL, &threadA_work, (void*)1);
  pthread_create(&threadB, NULL, &threadB_work, (void*)2);


  pthread_join(threadA, NULL);
  pthread_join(threadB, NULL);

  return 0;

}


int set_cpu(int cpu_number)
{
  cpu_set_t cpu;
  CPU_ZERO(&cpu);
  CPU_SET(cpu_number, &cpu);
  return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}
