#include "m_file.h"


MESSAGE *m_connexion(const char *nom, int options,...){


  va_list arguments;
  va_start(arguments,options);

  size_t nb_msg;
  size_t len_max;
  mode_t mode;
  int IS_CREATE = true;

  MESSAGE *message = malloc(sizeof(MESSAGE));
  memory *mem = malloc(sizeof(memory));
  message->options = 0;
  message->m = NULL;

  if( (nb_msg = (int) va_arg(arguments,size_t)) == FIN){
    IS_CREATE = false;
  }else{
    len_max = (size_t) va_arg(arguments,size_t);
    mode = (mode_t) va_arg(arguments,mode_t);
  }

  if(IS_CREATE == true){

    if(nom == NULL){

      mem = mmap(NULL, sizeof(memory), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS ,-1,0);
      int code;      
      mem->first=0;
      mem->last=-1;
      mem->len_max_msg = len_max;
      mem->capacite = nb_msg;
      mem->size = 0;
      mem->CAN_SEND_MESSAGE = 1;
      mem->messages = malloc(nb_msg*sizeof(mon_message));
      code = initialiser_mutex( &mem->mutex );
        if( code != 0 )
            thread_error(__FILE__, __LINE__, code, "init_mutex");
      code = initialiser_cond( &mem->rcond );
        if( code != 0 )
            thread_error(__FILE__, __LINE__, code, "init_rcond");
      code = initialiser_cond( &mem->wcond );
        if( code != 0 )
            thread_error(__FILE__, __LINE__, code, "init_wcond");

      

      message->m = mem;
      message->options = O_RDWR;
      
      return message;
    }else{

    char *shm_name = prefix_slash(nom);
    
    /* open and create */
    int fd = shm_open(shm_name, options,mode);
    if( fd < 0 )
      PANIC_EXIT("shm_open");
    
    int i = ftruncate(fd, sizeof(memory));
    if(i < 0)
      PANIC_EXIT("ftruncate");
    /* projection de shared memory object dans
    * la mémoire                              */
    mem = mmap(NULL, sizeof(memory), PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);
    if( (void *)mem == MAP_FAILED )
      return NULL;
    //initialiser la mémoire
    int code;
    mem->first=0;
    mem->last=-1;
    mem->len_max_msg = len_max;
    mem->capacite = nb_msg;
    mem->size = 0;
    mem->CAN_SEND_MESSAGE = 1;
    mem->messages = malloc(nb_msg*sizeof(mon_message));
    code = initialiser_mutex( &mem->mutex );
      if( code != 0 )
          thread_error(__FILE__, __LINE__, code, "init_mutex");
    code = initialiser_cond( &mem->rcond );
      if( code != 0 )
          thread_error(__FILE__, __LINE__, code, "init_rcond");
    code = initialiser_cond( &mem->wcond );
      if( code != 0 )
          thread_error(__FILE__, __LINE__, code, "init_wcond");


    message->m = mem;
    message->options = options;
      
    return message;
    }
    
  }else
  {
      char *shm_name = prefix_slash(nom);
      
      int fd = shm_open(shm_name, options,0);
      if( fd < 0 )
        PANIC_EXIT("shm_open");
    
      //projection en mémoire
      
      mem = mmap(NULL, sizeof(memory), PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);
      if( (void *)mem == MAP_FAILED)
        return NULL;
      

      message->m = mem;
      message->options = options;

      return message;

    }
  
}


int m_deconnexion(MESSAGE *file){
  
  /*
    int size = offsetof(file->m, file->m->messages) + strlen(file->m->messages);

    if( munmap(file->m, size) < 0 )
      return -1;
  */ 
  return 0;
  
}

int m_destruction( const char *nom){

  int f = shm_unlink(nom);

  return f;

}

int m_envoi(MESSAGE *file, const void *msg, size_t len, int msgflag){

    int code;
    code = pthread_mutex_lock(&file->m->mutex);
    if( code != 0 ){
      perror("lock non reussi");
    }

    /* attendre jusqu'à ce que la mémoire est libre */    
    while( !file->m->CAN_SEND_MESSAGE ){
      code = pthread_cond_wait(&file->m->wcond, &file->m->mutex);
      if( code > 0 ){
	    fprintf(stderr, "process %d error\n", (int) getpid());
	     perror("pthread_cond_wait" );
      }
    }

    /* section critique */

      //verifiant si la longeur de message est 
      //plus grand que la longeur maximale supporte par le file
      if(len > file->m->len_max_msg){
                      errno = EMSGSIZE;
                      return -1;
      }


      if(file->m->capacite == file->m->size){
              if(msgflag == O_NONBLOCK){
                      errno = EAGAIN;
                      return -1;
              }else if(msgflag == 0){
                while(1){
                  if(file->m->capacite > file->m->size){
                      file->m->last = (file->m->last + 1) % file->m->capacite;
                      memmove(file->m->messages[file->m->last].m_text, msg, len);
                      file->m->messages[file->m->last].size_msg = len;
                      file->m->size++;
                      file->m->CAN_SEND_MESSAGE = 0;
                      break;
                  }
                }
              }
        }else{
          file->m->last = (file->m->last + 1) % file->m->capacite;
          memmove(file->m->messages[file->m->last].m_text, msg, len);
          file->m->messages[file->m->last].size_msg = len;
          file->m->size++;
          file->m->CAN_SEND_MESSAGE = 0; /* IL YA UN MESSAGE A LIRE */
        }
    /*fin de section critique */

     code = pthread_mutex_unlock(&file->m->mutex);
    if( code != 0)
      perror("mutex_unlock" );

    /* signaler le lecteur */
    code = pthread_cond_signal( &file->m->rcond );
    if( code > 0 )
      perror("cond_signal" );

    return 0;
}

ssize_t m_reception(MESSAGE *file, void *msg, size_t len, long type, int flags){

    int code;
    code = pthread_mutex_lock(&file->m->mutex);
    if( code != 0 ){
      perror("lock non reussi");
    }

    /* attendre jusqu'à ce que la mémoire est libre */    
    while( file->m->CAN_SEND_MESSAGE ){
      code = pthread_cond_wait(&file->m->rcond, &file->m->mutex);
      if( code > 0 ){
	    fprintf(stderr, "process %d error\n", (int) getpid());
	     perror("pthread_cond_wait" );
      }
    }

    if(type == 0){
      if(len < file->m->messages[file->m->first].size_msg){
        errno = EMSGSIZE;
        return -1;
      }
      msg = file->m->messages[file->m->first].m_text;
    }else if (type > 0){
      
    }else if( type < 0){
      
    }
    

    /* section critique */

     file->m->first = (file->m->first + 1) % file->m->capacite;
     file->m->size--;

     file->m->CAN_SEND_MESSAGE = 1; /* LE MESSAGE EST DEJA LIT DONT PRET A ENVOYE UN NOUVEAU MESSAGE AU MEMOIRE*/

    /*fin de section critique */

     code = pthread_mutex_unlock(&file->m->mutex);
    if( code != 0)
      perror("mutex_unlock" );

    /* signaler le lecteur */
    code = pthread_cond_signal( &file->m->wcond );
    if( code > 0 )
      perror("cond_signal" );
  return 0;
}


size_t m_message_len(MESSAGE *file){
    return file->m->len_max_msg;
}

size_t m_capacite(MESSAGE *file){
    return file->m->capacite;
}

size_t m_nb(MESSAGE *file){
    return file->m->size;
}




