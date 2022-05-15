#ifndef M_DATA_H
#define M_DATA_H
#include <pthread.h>

typedef struct mon_message{
    size_t size_msg;
    long type;
    char m_text[20];
}mon_message;


typedef  struct memory{
  int len_max_msg;
  int capacite;
  int first;
  int last;
  int size; // current capacity of the queue

  pthread_mutex_t mutex_ecriture_message;
  pthread_cond_t  cond_signal_nouveau_message;

  int CAN_SEND_MESSAGE;
  mon_message messages [30];
}memory;

typedef struct MESSAGE {
    int options;
    memory *m;
}MESSAGE;








#endif
