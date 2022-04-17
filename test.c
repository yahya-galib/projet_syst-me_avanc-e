#include "m_file.h"

int main( int argc , char *argv[]){

    MESSAGE* file = malloc(sizeof(MESSAGE));
    file = m_connexion("yahya",O_CREAT|O_RDWR,1,1,0666);
    
    //int t[2] = {-12, 99};
    int t = 2;
    mon_message *m = malloc( sizeof( mon_message ) + sizeof( int ));
    if( m == NULL ){ exit(EXIT_FAILURE);}
    
    m->type = (long)getpid(); 
    memmove(m->m_text, t, sizeof( int ));
    
    int i = m_envoi( file, m, sizeof( t ), O_NONBLOCK);

    //printf("%d",file->m->size);
    //printf("%d",file->m->len_max_msg);

    printf("%d\n",file->m->last);
    printf("%d\n",file->m->first);
    printf("%d\n",file->m->size);
    printf("%d\n",file->m->capacite);
    
    if( i == 0 ){ printf("message envoyé");}
    else if( i == -1 && errno == EAGAIN ){  printf("file pleine");}
    else if(i == -1 && errno == EMSGSIZE){printf("taille du message autorisé ");}
    else{ printf("erreur"); }
/*
    int j = m_envoi( file, m, sizeof( t ), O_NONBLOCK);

    //printf("%d",file->m->size);
    //printf("%d",file->m->len_max_msg);

    printf("%d\n",file->m->last);
    printf("%d\n",file->m->first);
    printf("%d\n",file->m->size);
    printf("%d\n",file->m->capacite);

    
    if( j == 0 ){ printf("message envoyé");}
    else if( j == -1 && errno == EAGAIN ){  printf("file pleine");}
    else{ printf("erreur"); }
*/

    return 0;
}