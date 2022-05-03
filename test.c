#include "m_file.h"

int main( int argc , char *argv[]){

 /*
    int options = O_EXCL;
    printf("%d",options);

    int options = O_APPEND|O_RDWR;

    if(((options>>6) & 1) == 1){
        printf("O_CREAT est dedans");
    } 
*/ 

    MESSAGE *file = malloc(sizeof(MESSAGE));
    file = m_connexion("yahya",O_CREAT|O_RDWR,3,40,0666);
    
    printf("size %ld\n",sizeof(*(file->m)));
    //int t[2] = {-12, 99};
    //int t = 2;
    char buff[30]="aaaaaaaaaaaaaaaaazzz";
    printf("%ld\n",sizeof(buff));
    mon_message *m = malloc( sizeof( mon_message ) + sizeof(buff));
    if( m == NULL ){ exit(EXIT_FAILURE);}
    
    m->type = (long)getpid(); 
    
    //sprintf(buff,"%d",t); 
    memmove(m->m_text, buff, sizeof(buff));

    printf("%s\n",m->m_text);
    printf("%ld\n",m->type);

    
    printf("last = %d\n",file->m->last);
    printf("first = %d\n",file->m->first);

    int i = m_envoi( file, &m, sizeof(buff), O_NONBLOCK);

    //printf("%d",file->m->size);
    //printf("%d",file->m->len_max_msg);

    printf("last = %d\n",file->m->last);
    printf("first = %d\n",file->m->first);
    printf("size =%d\n",file->m->size);
    printf("capacite = %d\n",file->m->capacite);

    
    
    if( i == 0 ){ printf("message envoyé\n");}
    else if( i == -1 && errno == EAGAIN ){  printf("file pleine");}
    else if(i == -1 && errno == EMSGSIZE){printf("taille du message autorisé ");}
    else{ printf("erreur"); }


    printf("first %d\n",file->m->first);
    printf("last %d\n",file->m->last);
    printf("%s\n",file->m->messages[file->m->last].m_text);

    //printf("%s\n",file->m->messages[file->m->first+1].m_text);

     char buff1[30]="bbbbbbb";

    mon_message *m1 = malloc( sizeof( mon_message ) + sizeof(buff1));
    if( m1 == NULL ){ exit(EXIT_FAILURE);}
    
    m1->type = (long)getpid(); 
    
    //sprintf(buff,"%d",t); 
    memmove(m1->m_text, buff1, sizeof(buff1));

    printf("%s\n",m1->m_text);
    //printf("%s\n",buff);


    int j = m_envoi( file, &m1, sizeof(buff1), O_NONBLOCK);

    //printf("%d",file->m->size);
    //printf("%d",file->m->len_max_msg);

    printf("last = %d\n",file->m->last);
    printf("first = %d\n",file->m->first);
    printf("size =%d\n",file->m->size);
    printf("capacite = %d\n",file->m->capacite);

    
    if( j == 0 ){ printf("message envoyé\n");}
    else if( j == -1 && errno == EAGAIN ){  printf("file pleine");}
    else{ printf("erreur"); } 

    for (int k = 0; k < file->m->capacite; k++)
    {
        printf("%s\n",file->m->messages[k].m_text);
        printf("%ld\n",file->m->messages[k].type);
    }



   if( m_deconnexion(file) < 0 ){
       printf("echec");
   }else{
       printf("reussi");
   }

   m_destruction("yahya");
    return 0;
}