#include "thread.h"
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

typedef struct job job_t;

struct job{

  void* (*func)(void *);
  void* arg;
};

typedef struct pool pool_t;

struct pool{

  int max_threads;
  int active_threads;
  pthread_cond_t empty;
  pthread_cond_t fill;
  pthread_mutex_t mutex;
  int fill_ptr;
  int use_ptr;
  int count;
  int max_buf;
  job_t* job_queue;
};

static void *worker_func(void *);

void create_threads(pool_t* pool){

  int num_thr = pool->max_threads;
  pthread_t* thread_spec = malloc(sizeof(pthread_t) * num_thr);

  for(int i=0; i<num_thr; i++){

    pthread_create(&thread_spec[i], NULL, worker_func, pool);
  }

  return;
}

pool_t* pool_init(int max_size, int num_threads){

  pool_t* pool = malloc(sizeof(pool_t));

  pool->max_threads = num_threads;
  pool->max_buf = max_size;

  pool->fill_ptr = 0;
  pool->use_ptr = 0;
  pool->count = 0;

  pthread_cond_init(&(pool->empty), NULL);
  pthread_cond_init(&(pool->fill), NULL);
  pthread_mutex_init(&(pool->mutex), NULL);

  pool->job_queue = (job_t*) malloc(max_size * sizeof(job_t));


  create_threads(pool);
  
  return pool;
}

void pool_add_job(pool_t* pool, void* func, void* arg){

  job_t* job;

  job = (job_t*) malloc(sizeof (*job));
    
  job->arg = arg;
  job->func = func;

  pthread_mutex_lock(&pool->mutex);

  while(pool->count == pool->max_buf)
    pthread_cond_wait(&pool->empty, &pool->mutex);

  pool->job_queue[pool->fill_ptr % pool->max_buf] = *job;
  pool->fill_ptr = (pool->fill_ptr + 1) % pool->max_buf;
  pool->count++;
  
  pthread_cond_signal(&pool->fill);
  
  pthread_mutex_unlock(&pool->mutex);

  return;
}

void* worker_func(void* arg){

  pool_t* pool = (pool_t*) arg;

  job_t* job;
  void *(*func)(void*);
  void* argmt;

  while(1) {

    pthread_mutex_lock(&pool->mutex);

    while(pool->count == 0)
      pthread_cond_wait(&pool->fill, &pool->mutex);

    job = pool->job_queue + pool->use_ptr;
    pool->use_ptr = (pool->use_ptr + 1) % pool->max_buf;
    pool->count--;

    pthread_cond_signal(&pool->empty);
    pthread_mutex_unlock(&pool->mutex);

    func = job->func;
    argmt = job->arg;

    func(argmt);
  }
  
  return (NULL);
}
