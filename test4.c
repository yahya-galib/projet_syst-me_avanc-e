#include "m_file.h"
#include <sys/types.h>

void lire_message(MESSAGE *file,long type,int flags){
   mon_message *m = malloc( sizeof( mon_message ) + 30*sizeof(char));
   int result = m_reception(file,m,  30*sizeof(char), type,flags);
   if(result == -1){
          printf("aucun message n'a été lu");
   }else{
        printf("message : %s",m->m_text);
   }
}
int main( int argc , char * const argv[]){
    MESSAGE *file = malloc(sizeof(MESSAGE));
    int choix;
    pid_t pid;
    pid=fork();
	if(pid==0){
		file = m_connexion("yahya",O_CREAT|O_RDWR,FIN);
		mon_message *m;
		char *message = "Message";
		system("clear");
	  	do{
		    printf("\n\t\t********************Menu Principale******************\n\n");
		    puts("1- Lire le premier message dans la file avec appel bloquant");
		    puts("2- Lire le premier message dans la file avec appel non bloquant");
		    puts("3- Lire le premier message dans le type = type avec appel bloquant");
		    puts("4- Lire le premier message dans le type = type Avec appel non bloquant");
		    puts("5- Lire le premier message dans le type < |type| appel  bloquant");
		    puts("6- Lire le premier message dans le type < |type| avec appel non bloquant");
		    puts("7- inserer un message ");
		    puts("8- quitter le programme");
		    do{
		    	printf("\nSaisir votre choix  :\t");
	        	scanf("%d",&choix);
	        	if(choix>8 || choix<1)
	        	{
	        		printf("votre choix doit etre compris entre 1 et 3 \n");
	        	}
	    	}while(choix>8 || choix<1);
	    switch(choix){
	      	
	      	case 1: {  
	        		system("clear");
	        		lire_message(file,0,0);
	      	        }break;
	      	case 2: { 
	                system("clear");
	                lire_message(file,0,O_NONBLOCK);
	                }break;
	        case 3: { 
	                system("clear");
	                lire_message(file,(long)getpid(),0);
	                }break;
	        case 4: { 
	                system("clear");
	                lire_message(file, (long)getpid() ,O_NONBLOCK);
	                }break;
	        case 5: { 
	                system("clear");
	                lire_message(file,NEGATIVE,0);
	                }break;
	        case 6: { 
	                system("clear");
	                lire_message(file,NEGATIVE,O_NONBLOCK);
	                }break;
	        case 7: { 
	                system("clear");
	                m = malloc( sizeof( mon_message ) + sizeof(message));
	                if( m == NULL ){ 
	                	exit(EXIT_FAILURE);
	                }
	                m->type = (long)getpid(); 
	                    snprintf(m->m_text,sizeof(message)+sizeof(int),"%s %d\n",message,(int) file->m->last +1);
	                    int i = m_envoi( file, m, sizeof(message)+sizeof(int), O_NONBLOCK);
	                    if( i == 0 ){ printf("message envoyé\n");}
	                    else if( i == -1 && errno == EAGAIN ){  printf("file pleine");}
	                    else if(i == -1 && errno == EMSGSIZE){printf("taille du message autorisé ");}
	                    else{ printf("erreur"); }
	                }break;
	      }
	    }while(choix !=8);
	    if(choix ==8){
	   		if( m_deconnexion(file) < 0 ){
           printf("echec");
        }else{
        printf("reussi");
        }
        m_destruction("yahya");
            return 0;
        }
 
	}
	if(pid>0){
		file = m_connexion("yahya",O_CREAT|O_RDWR,FIN);
		mon_message *m;
		char *message = "Message";
		system("clear");
	  	do{
		    printf("\n\t\t********************Menu Principale******************\n\n");
		    puts("1- Lire le premier message dans la file avec appel bloquant");
		    puts("2- Lire le premier message dans la file avec appel non bloquant");
		    puts("3- Lire le premier message dans le type = type avec appel bloquant");
		    puts("4- Lire le premier message dans le type = type Avec appel non bloquant");
		    puts("5- Lire le premier message dans le type < |type| appel  bloquant");
		    puts("6- Lire le premier message dans le type < |type| avec appel non bloquant");
		    puts("7- inserer un message ");
		    puts("8- quitter le programme");
		    do{
		    	printf("\nSaisir votre choix  :\t");
	        	scanf("%d",&choix);
	        	if(choix>8 || choix<1)
	        	{
	        		printf("votre choix doit etre compris entre 1 et 3 \n");
	        	}
	    	}while(choix>8 || choix<1);
	    switch(choix){
	      	
	      	case 1: {  
	        		system("clear");
	        		lire_message(file,0,0);
	      	        }break;
	      	case 2: { 
	                system("clear");
	                lire_message(file,0,O_NONBLOCK);
	                }break;
	        case 3: { 
	                system("clear");
	                lire_message(file,(long)getpid(),0);
	                }break;
	        case 4: { 
	                system("clear");
	                lire_message(file, (long)getpid() ,O_NONBLOCK);
	                }break;
	        case 5: { 
	                system("clear");
	                lire_message(file,NEGATIVE,0);
	                }break;
	        case 6: { 
	                system("clear");
	                lire_message(file,NEGATIVE,O_NONBLOCK);
	                }break;
	        case 7: { 
	                system("clear");
	                m = malloc( sizeof( mon_message ) + sizeof(message));
	                if( m == NULL ){ 
	                	exit(EXIT_FAILURE);
	                }
	                m->type = (long)getpid(); 
	                    snprintf(m->m_text,sizeof(message)+sizeof(int),"%s %d\n",message,(int) file->m->last +1);
	                    int i = m_envoi( file, m, sizeof(message)+sizeof(int), O_NONBLOCK);
	                    if( i == 0 ){ printf("message envoyé\n");}
	                    else if( i == -1 && errno == EAGAIN ){  printf("file pleine");}
	                    else if(i == -1 && errno == EMSGSIZE){printf("taille du message autorisé ");}
	                    else{ printf("erreur"); }
	                }break;
	      }
	    }while(choix !=8);

        if( m_deconnexion(file) < 0 ){
           printf("echec");
        }else{
        printf("reussi");
        }
        m_destruction("yahya"); 
	}

    return 0;
}