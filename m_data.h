#ifndef M_DATA_H
#define M_DATA_H
#include <pthread.h>

typedef struct mon_message{
    int size_msg;
    long type;
    char m_text[];
}mon_message;


typedef  struct memory{
  int len_max_msg;
  int capacite;
  int first;
  int last;
  int size;
  pthread_mutex_t mutex;
  pthread_cond_t  rcond; // à modifier
  pthread_cond_t  wcond; // à modifier
  int CAN_SEND_MESSAGE;
  struct mon_message *messages; 
}memory;

typedef struct MESSAGE {
    int options;
    struct memory *m;
}MESSAGE;








#endif