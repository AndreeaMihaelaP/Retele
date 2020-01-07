#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

/* portul folosit */
#define PORT 2908
#define NR_MAX_CLIENTI 3

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int valori[] = { 1,2,3,4,1,1,1,1,2,3 };
int counter = 0;

void displayValues(int valori[], int size) {
  for(int item = 0; item < size; ++item){
    printf("%d\n", valori[item]);
  }
}

int chooseFavoriteFood(int arr[], int n, int k) { 
  // Iterate though input array, for every element 
    // arr[i], increment arr[arr[i]%k] by k 
    for (int i = 0; i< n; i++) 
        arr[arr[i]%k] += k; 
  
    // Find index of the maximum repeating element 
    int max = arr[0], result = 0; 
    for (int i = 1; i < n; i++) 
    { 
        if (arr[i] > max) 
        { 
            max = arr[i]; 
            result = i; 
        } 
    } 
  
    /* Uncomment this code to get the original array back 
       for (int i = 0; i< n; i++) 
          arr[i] = arr[i]%k; */
  
    // Return index of the maximum element 
    return result; 
}

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int main ()
{
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i=0;
  int nrMaxClienti = 0;

  int rezult = chooseFavoriteFood(valori, sizeof(valori)/sizeof(valori[0]), 5 );

  printf("the result %d\n", rezult);

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
  /* servim in mod concurent clientii...folosind thread-uri */
  while(nrMaxClienti <= NR_MAX_CLIENTI) {
      int client;
      thData * td; //parametru functia executata de thread     
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
      {
        perror ("[server]Eroare la accept().\n");
        continue;
      }
      
        /* s-a realizat conexiunea, se astepta mesajul */
    
      // int idThread; //id-ul threadului
      // int cl; //descriptorul intors de accept

      td=(struct thData*)malloc(sizeof(struct thData));	
      td->idThread=i++;
      td->cl=client;

      pthread_create(&th[i], NULL, &treat, td);	      
      nrMaxClienti++;
	}
};				
static void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);	
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
};


void raspunde(void *arg) {
    int nr, i=0;
    struct thData tdL; 
    tdL= *((struct thData*)arg);
    if (read (tdL.cl, &nr,sizeof(int)) <= 0)
        {
          printf("[Thread %d]\n",tdL.idThread);
          perror ("Eroare la read() de la client.\n");
        
        }
    
    printf ("[Thread %d]Mesajul a fost receptionat...%d\n",tdL.idThread, nr);

    valori[counter++] = nr;
    
    for ( int i = 0; i < sizeof(valori) ; i++ ){
      printf("valori si bla %d %d \n", i, valori[i]);
    };

    /*pregatim mesajul de raspuns */     
    printf("[Thread %d]Trimitem mesajul inapoi...%d\n",tdL.idThread, nr);
            
            
            /* returnam mesajul clientului */
    if (write (tdL.cl, &nr, sizeof(int)) <= 0)
      {
      printf("[Thread %d] ",tdL.idThread);
      perror ("[Thread]Eroare la write() catre client.\n");
      }
    else
      printf ("[Thread %d]Mesajul a fost trasmis cu succes.\n",tdL.idThread);	

}