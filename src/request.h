#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <pthread.h>

void request_handle_m(int fd);

void *request_handle_t(void* arg);

void put(int fd);

int get();

#endif // __REQUEST_H__
