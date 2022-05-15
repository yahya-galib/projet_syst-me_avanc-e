#include "m_file.h"


MESSAGE *m_connexion(const char *nom, int options,...){


  va_list arguments;
  va_start(arguments,options);

  size_t nb_msg;
  size_t len_max;
  mode_t mode;
  int IS_CREATE = false;

 
 
  //memory *mem = malloc(sizeof(memory));
  
  if( (nb_msg = (int) va_arg(arguments,size_t)) == FIN){
    
      IS_CREATE = false;      
    

  }else{
    if(((options>>6)&1)==1){ //O_CREAT
      IS_CREATE = true;
      len_max = (size_t) va_arg(arguments,size_t);
      mode = (mode_t) va_arg(arguments,mode_t);       
    }
    else {
      printf("Vous devez inclure O_CREAT");
      return NULL;//retourner une erreur plutôt.
    }
     
  }
  MESSAGE *message = malloc(sizeof(MESSAGE)+nb_msg*sizeof(mon_message));
  message->options = 0;
  
  if(IS_CREATE == true){

    if(nom == NULL){
      memory *mem = mmap(NULL, sizeof(memory), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS ,-1,0);
      int code;      
      mem->first=-1;
      mem->last=-1;
      mem->len_max_msg = len_max;
      mem->capacite = nb_msg;
      mem->size = 0;
      mem->CAN_SEND_MESSAGE = 1;

       //ajoute initialisation des nouveaux variables condition et mutex
        code = initialiser_mutex( &mem->mutex_ecriture_message );
        if( code != 0 )
            thread_error(__FILE__, __LINE__, code, "init_mutex");
        code = initialiser_cond( &mem->cond_signal_nouveau_message );
        if( code != 0 )
            thread_error(__FILE__, __LINE__, code, "init_rcond");


      message->m = mem;
      message->options = O_RDWR;
      
      return message;
    }else{

    char *shm_name = prefix_slash(nom);
    char dest[50] = "/dev/shm/";

    strcat(dest,shm_name);
    if( (access( dest, F_OK ) != -1) && (((options>>6) & 1) == 1) && (((options>>5) & 1) == 1))
    {
       printf("Object memory exist");
    }  

    /* open and create */
    int fd = shm_open(shm_name,options,mode);
    if( fd < 0 )
      PANIC_EXIT("shm_open");
    
    int i = ftruncate(fd, sizeof(memory));
    if(i < 0)
      PANIC_EXIT("ftruncate");
    /* projection de shared memory object dans
    * la mémoire                              */
    memory *mem = mmap(NULL, sizeof(memory), PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);
    if( (void *)mem == MAP_FAILED )
      return NULL;
    //initialiser la mémoire
    int code;
    mem->first=-1;
    mem->last=-1;
    mem->len_max_msg = len_max;
    mem->capacite = nb_msg;
    mem->size = 0;
    mem->CAN_SEND_MESSAGE = 1;
    //mem->messages = malloc(nb_msg*sizeof(mon_message));

      //ajoute initialisation des nouveaux variables condition et mutex
        code = initialiser_mutex( &mem->mutex_ecriture_message );
        if( code != 0 )
            thread_error(__FILE__, __LINE__, code, "init_mutex");
        code = initialiser_cond( &mem->cond_signal_nouveau_message );
        if( code != 0 )
            thread_error(__FILE__, __LINE__, code, "init_rcond");


    message->m = mem;
    if( ( ((options&O_RDWR)!=0)&&((options&O_WRONLY)==0)&&((options&O_RDONLY)==0) )||
      ( ((options&O_RDWR)==0)&&((options&O_WRONLY)!=0)&&((options&O_RDONLY)==0) )||
      ( ((options&O_RDWR)==0)&&((options&O_WRONLY)==0)&&((options&O_RDONLY)!=0) )
      ){
      message->options = options;  
    }
    else{
      printf("Vous devez inclure O_RDWR ou bien O_WRONLY ou bien O_RDONLY");
      return NULL;
      //erreur
    }
    
      
    return message;
    }
    
  }else
  {
      char *shm_name = prefix_slash(nom);
      
      int fd = shm_open(shm_name, options,0);
      if( fd < 0 )
        PANIC_EXIT("shm_open");
    
      //projection en mémoire
      
      memory *mem = mmap(NULL, sizeof(memory), PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);
      if( (void *)mem == MAP_FAILED)
        return NULL;
      

      message->m = mem;
      message->options = options;

      return message;

    }
  
}


int m_deconnexion(MESSAGE *file){
  
    int i = munmap(file->m, sizeof(*(file->m)));
    if(i < 0 ){
      return -1;
    }
  
    return 0;
  
  
}

int m_destruction( const char *nom){

  int f = shm_unlink(nom);

  return f;

}

int m_envoi(MESSAGE *file, const void *msge, size_t len, int msgflag){
  

      if(len > file->m->len_max_msg){
                      errno = EMSGSIZE;
                      return -1;
      }

      if((file->m->first == -1) && (file->m->last == -1)){
        file->m->first = 0;
        file->m->last = 0;

         copie_message_dans_la_file(file,msge,len);

      }
      else if((file->m->last + 1) % file->m->capacite == file->m->first){
              if(msgflag == O_NONBLOCK){
                      errno = EAGAIN;
                      return -1;
              }else if(msgflag == 0){
                while(1){
                  if((file->m->last + 1)%file->m->capacite != file->m->first){
                      
                            //file->m->last = (file->m->last + 1) % file->m->capacite;
                            // d'apres la modification du projet last sera modifie par m_reception donc pas besoin de modifier ici 
                            copie_message_dans_la_file(file,msge,len);
                      break;
                  }
                }
              }
        }else{

        file->m->last++;
        //printf("dans else de la fonction m_envoi et last = %d \n",file->m->last);
        copie_message_dans_la_file(file,msge,len);
        }
   

    return 0;
}



  void copie_message_dans_la_file(MESSAGE *file, const void *msge, size_t len){
     const mon_message *msg = msge;
      int code_messages;
     code_messages = pthread_mutex_lock(&file->m->mutex_ecriture_message);
           if( code_messages != 0){
              thread_error( __FILE__ , __LINE__ , code_messages, "mutex_lock" ); 
           }

                memmove(&file->m->messages[file->m->last].m_text, msg->m_text, len+sizeof(mon_message));
                file->m->messages[file->m->last].type =   msg->type;
                file->m->messages[file->m->last].size_msg = len;
                file->m->size++;
                file->m->CAN_SEND_MESSAGE = 0;

        code_messages = pthread_mutex_unlock(&file->m->mutex_ecriture_message);
        if( code_messages != 0)          
            thread_error( __FILE__ , __LINE__ , code_messages, "mutex_unlock" );

          /* signaler le lecteur */
        code_messages = pthread_cond_signal(&file->m->cond_signal_nouveau_message );
        if( code_messages > 0 )          
            thread_error( __FILE__ , __LINE__ , code_messages, "cond_signal" );

  }


ssize_t m_reception(MESSAGE *file, void *msge, size_t len, long type, int flags){

  mon_message *msg = msge;


    if(type == 0){
      if(len < file->m->messages[file->m->first].size_msg){
        errno = EMSGSIZE;
        return -1;
      }

      if(file->m->first == -1 && file->m->last == -1){
             // le tableau est vide il n'a rien a lire 
           if(flags == 0){
               bloque_and_wait_signal_nouveau_message(file);
               m_reception(file,msge,len,type,flags);
           }else{
             // O_NONBLOCK
             errno = EAGAIN;
             return -1;
           }
      }else{
          *msg = file->m->messages[file->m->first];
          supprime_une_case(file,file->m->first,file->m->last);
          decrementation_last(file);
         
      } 
      

    }else if (type > 0){

        if(file->m->first == -1 && file->m->last == -1){
           if(flags == 0){
               bloque_and_wait_signal_nouveau_message(file);
               m_reception(file,msge,len,type,flags);
           }else{
             //  O_NONBLOCK
                errno = EAGAIN;
             return -1;
           }

        }else{
          
                int find = FALSE;
                int position;
                for(int i=0;i<=file->m->last;i++){
                    if(file->m->messages[i].type==type){
                      find= TRUE;
                      position= i;
                      break;
                    }
                }


                if(find==TRUE){
                      *msg = file->m->messages[position];
                      supprime_une_case(file,position,file->m->last);
                      //modification de la valeur de last 
                      decrementation_last(file);
                      //else le tableau est vide 
                }else{
                  // il y a pas la valeur recherchée dans le tableau
                    if(flags == 0){
                       bloque_and_wait_signal_nouveau_message(file);
                       m_reception(file,msge,len,type,flags);
                    }else{
                      //ici c'est le cas ou l'appel est non bloquant avec O_NONBLOCK
                       errno = EAGAIN;
                      return -1;
                    }
                }
                
         
      } 
      
      
    }else if( type < 0){

        if(file->m->first == -1 && file->m->last == -1){

           if(flags == 0){
               bloque_and_wait_signal_nouveau_message(file);
               m_reception(file,msge,len,type,flags);
           }else{
             // O_NONBLOCK
              errno = EAGAIN;
             return -1;
           }

        }else{
          
                int find = FALSE;
                int position;
                   for(int i=0;i<=file->m->last;i++){
                      if(file->m->messages[i].type < labs(type)){
                        find= TRUE;
                        position= i;
                        break;
                      }
                   }


                if(find==TRUE){
                       *msg = file->m->messages[position];
                       supprime_une_case(file,position,file->m->last);
                      //modification de la valeur de last 
                      decrementation_last(file);
                      //else le tableau est vide 
                }else{
                  // il y a pas la valeur recherchée dans le tableau 

                  // le tableau est vide il y'a rien à lire 
                    if(flags == 0){
                      bloque_and_wait_signal_nouveau_message(file);
                       m_reception(file,msge,len,type,flags);
                    }else{
                      // O_NONBLOCK
                        errno = EAGAIN;
                      return -1;
                    }
                }
                
         
      } 
      
      
    }
    

  
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


void supprime_une_case(MESSAGE *file,int num_case, int last){

  
 
  // num_case==last){
    // dans ce cas la boucle ne sera pas verifie, l'idee est juste de modifier la variable last comme -1 et first comme -1
    // pour dire le tableau est vide alors qu'il est pas vraiment vide
    // mais la prochaine insertion va modifier la valeur de première case 
  for(int i=num_case;i<last;i++){
     file->m->messages[i]=file->m->messages[i+1];
   }
   //le probleme ici la  case last-1 et la case last-2 sera identique  et finalement la case last -1 sera le nouveau
   //dernier case et sera modifie quant il aura un nouveau message
   // mais vu que 
  


   //En gros supprimer une case dans un tableau selon moi ctd changer sa valeur par une autre si ce dernier est possible 
   // le fait que le tableau est vide ou pas on va mettre à jour first et last mais pas le contenu du tableau 
}

void decrementation_last(MESSAGE *file){

    if(file->m->first == file->m->last){
            //ctd c'etait le dernier case à lire et il faut maintenant dire que le tableau est vide 
            file->m->first = -1;
            file->m->last = -1;
    }else{
              file->m->last--;
    }
}


void bloque_and_wait_signal_nouveau_message(MESSAGE *file){

   printf("\n dommage il y'a pas un message à lire, on va etre bloque.\n");
    int code_messages;
    code_messages = pthread_mutex_lock(&file->m->mutex_ecriture_message);
    if( code_messages != 0 )
      thread_error( __FILE__ , __LINE__ , code_messages, "mutex_lock" );
    
    code_messages = pthread_cond_wait(&file->m->cond_signal_nouveau_message,&file->m->mutex_ecriture_message);

    code_messages = pthread_mutex_unlock(&file->m->mutex_ecriture_message);
        if( code_messages != 0)
            thread_error( __FILE__ , __LINE__ , code_messages, "mutex_unlock" );
   

   printf("\n un nouveau message vient d'etre inserer, verifiant si on peut lire \n "); 

}


