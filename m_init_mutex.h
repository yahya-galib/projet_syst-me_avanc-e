#ifndef INIT_MUTEX_H
#define INIT_MUTEX_H
#include <pthread.h>

int initialiser_mutex(pthread_mutex_t *pmutex);
int initialiser_cond(pthread_cond_t *pcond);
#endif
