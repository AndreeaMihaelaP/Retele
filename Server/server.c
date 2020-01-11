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
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
/* portul folosit */
#define PORT 2908
/* codul de eroare returnat de anumite apeluri */
extern int errno;
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
  while (1)
    {
      int client;
      thData * td; //parametru functia executata de thread
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}

        /* s-a realizat conexiunea, se astepta mesajul */

	int idThread; //id-ul threadului
	int cl; //descriptorul intors de accept

	td=(struct thData*)malloc(sizeof(struct thData));
	td->idThread=i++;
	td->cl=client;

	pthread_create(&th[i], NULL, &treat, td);

	}//while
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

// Console shopper
void raspunde(void *arg) {
  int len1, len2;
	// Thread
	struct thData tdL;
	tdL= *((struct thData*)arg);
  int operatie;
  char nume[50], parola[50];
	// Citim operatia
  read(tdL.cl, &operatie, 4);
  printf("Operatie : %d \n", operatie);
  // Logare
  if (operatie == 1) {
      int cnt;
      FILE *fis = fopen("cnt.txt", "r");
      fscanf(fis, "%d", &cnt);
      fclose(fis);

			// Citim numele utilizatorului
      read(tdL.cl, &len1, 4);
      read(tdL.cl, nume, len1);
      nume[len1] = '\0';

			// Citim parola
      read(tdL.cl, &len2, 4);
      read(tdL.cl, parola, len2);
      parola[len2] = '\0';

      printf("\n");
      printf("%s %s", nume, parola);
      printf("\n");
      printf("%d %d", len1, len2);
      printf("\n");
      printf("CNT: %d", cnt);
      printf("\n");

      FILE *fis2 = fopen("useri.txt", "r");
      int ok = 0;

			// Cautam utilizatorul
      for (int i = 0; i < cnt; i++){
          char one[50], two[50];
          fscanf(fis2, "%s", one);
          fscanf(fis2, "%s", two);

          if(strcmp(one, nume) == 0 && strcmp(two, parola) == 0){
            ok = 1;
          }
      }

			// Vedem daca s-a logat sau nu
      write(tdL.cl, &ok, 4);
  }

  // Sign up - Aceeasi logica ca la Login pentru prima parte
  if (operatie == 2) {
      int cnt;
      FILE *fis = fopen("cnt.txt", "r");
      fscanf(fis, "%d", &cnt);
      fclose(fis);

      read(tdL.cl, &len1, 4);
      read(tdL.cl, nume, len1);
      nume[len1] = '\0';

      read(tdL.cl, &len2, 4);
      read(tdL.cl, parola, len2);
      parola[len2] = '\0';

      printf("\n");
      printf("%s %s", nume, parola);
      printf("\n");
      printf("%d %d", len1, len2);
      printf("\n");
      printf("CNT: %d", cnt);
      printf("\n");
      FILE *fis2 = fopen("useri.txt", "r");
      int ok = 0;

			// Cautam sa vedem daca utilizatorul se afla in baza de date
      for(int i = 0; i < cnt; i++){
          char one[50], two[50];
          fscanf(fis2, "%s", one);
          fscanf(fis2, "%s", two);

          if(strcmp(one, nume) == 0 && strcmp(two, parola) == 0){
            ok = 1;
          }
      }

      write(tdL.cl, &ok, 4);

			// Daca nu am gasit utilizatorul, il bagam in baza de date
      if (ok == 0){
          fis = fopen("cnt.txt", "w");
          cnt++;
          fprintf(fis, "%d", cnt);
          fclose(fis);
          fis2 = fopen("useri.txt", "a+");
          fprintf(fis2, "%s %s\n", nume, parola);
          fclose(fis2);
      }
  }

  int g = 1;

  while (g) {
    g = 0;
    char categorii[5000];
    memset(categorii, 0, sizeof(categorii));

		// Afisam categoriile
    strcat(categorii, "1.Imbracaminte.\n");
    strcat(categorii, "2.Incaltaminte.\n");
    strcat(categorii, "3.Telefonie.\n");
    int len3 = strlen(categorii);
    write(tdL.cl, &len3, 4);
    write(tdL.cl, categorii, len3);
    int cat;
    read(tdL.cl, &cat, 4);
    printf("Categorie : %d\n", cat);
    FILE *fis3;

    if (cat == 1) {
      fis3 = fopen("imbracaminte.txt", "r");
		} else if(cat == 2) {
      fis3 = fopen("incaltaminte.txt", "r");
		} else {
      fis3 = fopen("telefonie.txt", "r");
		}

    int n;
    char p[50][50];
    char c[50][50];

    fscanf(fis3, "%d", &n);
    printf("N = %d\n", n);
    for(int i = 0; i < n; i++){
        fscanf(fis3, "%s %s",p[i], c[i]);
    }
    fclose(fis3);

		// Afisam produsele (p, c vectorii care contin produsele)
    write(tdL.cl, &n, 4);
    printf("\n");
    write(tdL.cl, p, 2500);
    printf("\n");
    write(tdL.cl, c, 2500);
    printf("\n");
    read(tdL.cl, &g, 4);
  }
}
