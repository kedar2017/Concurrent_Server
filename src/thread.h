#include <pthread.h>


typedef struct pool pool_t;

void create_threads(pool_t* pool);

pool_t* pool_init(int max_size, int num_threads);

void pool_add_job(pool_t* pool, void* func, void* arg);

