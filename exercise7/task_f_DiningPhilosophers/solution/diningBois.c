#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <sched.h>
#include <stdlib.h>

int set_cpu(int cpu_number);

const int n_philosophers = 5;
const char* names_philosopher[5] = {"Socrates", "Plato", "Aristotle", "Thales", "Pythagoras"};


pthread_mutex_t mtx_resource[n_philosophers];


void* philosopher_work(void* id){
  set_cpu(1);
  printf("%.10s wants to eat\n", names_philosopher[*((int*)id)]);
  fflush(stdout);
  int right_cutlery = *((int*)id);
  int left_cutlery = *((int*)id);
  if (left_cutlery >= n_philosophers)
    left_cutlery = 0;
  pthread_mutex_lock(&mtx_resource[right_cutlery]);
  while (pthread_mutex_trylock(&mtx_resource[left_cutlery]))
    {
      pthread_mutex_unlock(&mtx_resource[right_cutlery]);   //only works with mutex; thread has ownership
      sched_yield();
    }
  printf("%.10s is eating\n", names_philosopher[*((int*)id)]);
  fflush(stdout);
  sleep(2); // Skulle hatt en slags vDelay men klarte ikke å finne dette for linux os
  pthread_mutex_unlock(&mtx_resource[right_cutlery]);
  pthread_mutex_unlock(&mtx_resource[left_cutlery]);
  sched_yield();
  return &philosopher_work;
}


int main(){
  for (int i = 0; i < n_philosophers; i++) {
    pthread_mutex_init(&mtx_resource[i], NULL);
  }
  pthread_t thread_philosopher[n_philosophers];
  for (int i = 0; i < n_philosophers; i++) {
    int* arg = malloc(sizeof(*arg));
    *arg = i;
    pthread_create(&thread_philosopher[i], NULL, &philosopher_work, arg);
  }

  for (int i = 0; i < n_philosophers; i++) {
    pthread_join(thread_philosopher[i], NULL);
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
