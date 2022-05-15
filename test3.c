#include "m_file.h"
#include <sys/types.h>

int main( int argc , char * const argv[]){

 

    MESSAGE *file = malloc(sizeof(MESSAGE));
    int taille=10;
    int compteur = 0;
    int choix;
    pid_t pid;
    pid=fork();
    if(pid==0){

        //MESSAGE *file = malloc(sizeof(MESSAGE));
        file = m_connexion("yahya",O_CREAT|O_RDWR,taille,40,0666);
        char *message = "Message 2";
        mon_message *m;
        
        system("clear");
        do
        {
        printf("\n\t\t********************Menu Principal******************\n\n");
        puts("1- Ajouter un  message dans la file.");
        puts("2- Afficher les messages dans la file.");
        puts("3- quitter le programme");

        do{
            printf("\nSaisir votre choix  :\t");
            scanf("%d",&choix); //préférez read stdin;
            if(choix>3 || choix<1)
            {
                printf("votre choix doit etre compris entre 1 et 3 \n");
            }
        }while(choix>3 || choix<1);

        switch(choix)
          {
            case 1: {  
                        system("clear");
                        m = malloc( sizeof( mon_message ) + sizeof(message));
                        if( m == NULL ){ exit(EXIT_FAILURE);}
                        m->type = (long)getpid(); 
                    
                        snprintf(m->m_text,sizeof(message)+sizeof(int),"%s %d\n",message,(int)compteur);
                        
                        compteur++;

                        int i = m_envoi( file, m, sizeof(message)+sizeof(int), O_NONBLOCK);
                        if( i == 0 ){ printf("message envoyé\n");}
                        else if( i == -1 && errno == EAGAIN ){printf("file pleine");}
                        else if(i == -1 && errno == EMSGSIZE){printf("taille du message autorisé ");}
                        else{ printf("erreur"); }

                     }break;
            case 2: { 
                         system("clear");
                        for (int k = 0; k <= file->m->last; k++)
                        {
                            printf("Message n %d : %s\n",k,file->m->messages[k].m_text);
                        }
                    }break;
         
          }

        }while(choix !=3);
        if(choix ==3){
            return 0;
        }
        if( m_deconnexion(file) < 0 ){
           printf("echec");
        }else{
        printf("reussi");
        }
        m_destruction("yahya"); 
   
    }


if(pid>0){
    file = m_connexion("yahya",O_CREAT|O_RDWR,4,40,0666);
    char *message = "Message 1";
    mon_message *m;
    int choix;
    system("clear");
    do
    {
    printf("\n\t\t********************Menu Principal******************\n\n");
    puts("1- Ajouter un  message dans la file.");
    puts("2- Afficher les messages dans la file.");
    puts("3- quitter le programme");

    do{
        printf("\nSaisir votre choix  :\t");
        scanf("%d",&choix); //préférez read stdin;
        if(choix>3 || choix<1)
        {
            printf("votre choix doit etre compris entre 1 et 3 \n");
        }
    }while(choix>3 || choix<1);

    switch(choix)
      {
        case 1: {  
                    system("clear");
                    m = malloc( sizeof( mon_message ) + sizeof(message));
                    if( m == NULL ){ exit(EXIT_FAILURE);}
                    m->type = (long)getpid(); 
                
                    snprintf(m->m_text,sizeof(message)+sizeof(int),"%s %d\n",message,(int)compteur);
                    
                    compteur++;

                    int i = m_envoi( file, m, sizeof(message)+sizeof(int), O_NONBLOCK);
                    if( i == 0 ){ printf("message envoyé\n");}
                    else if( i == -1 && errno == EAGAIN ){printf("file pleine");}
                    else if(i == -1 && errno == EMSGSIZE){printf("taille du message autorisé ");}
                    else{ printf("erreur"); }

                 }break;
        case 2: { 
                     system("clear");
                    for (int k = 0; k <= file->m->last; k++)
                    {
                        printf("Message n %d : %s\n",k,file->m->messages[k].m_text);
                    }
                }break;
     
      }

    }while(choix !=3);
    if(choix ==3){
        
   if( m_deconnexion(file) < 0 ){
       printf("echec");
   }else{
       printf("reussi");
   }
    m_destruction("yahya"); 
        return 0;
    }
  

}
 

    return 0;
}