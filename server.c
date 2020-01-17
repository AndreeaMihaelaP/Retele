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
#include <sqlite3.h>


#define PORT 2908

// codul de eroare returnat de anumite apeluri
extern int errno;

int n = 0;
char produse[200][200];

char cost[200][200];
int m = 0;


typedef struct thData {
	int idThread; // id-ul thread-ului tinut in evidenta de acest program
	int cl; // descriptorul intors de accept
}thData;

static void *treat(void *); // functia executata de fiecare thread ce realizeaza comunicarea cu clientii
void raspunde(void *);
int check(int, const char*);
int selectUseri(char*, char*);
void insertUser( char* , char*);
void selectCategorie(char categorie[]);


int main () {
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;
  int sd;		//descriptorul de socket
  int pid;
  pthread_t th[100]; // Identificatorii thread-urilor care se vor crea
	int i = 0;

  // crearea unui socket
  check (sd = socket (AF_INET, SOCK_STREAM, 0), "[server]Eroare la socket().\n");

  // utilizarea optiunii SO_REUSEADDR
  int on = 1;
  setsockopt(sd,SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  // pregatirea structurilor de date
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));

  /* umplem structura folosita de server
  stabilirea familiei de socket-uri */
  server.sin_family = AF_INET;
  // acceptam orice adresa 
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  // utilizam un port utilizator
  server.sin_port = htons (PORT);

  // atasam socketul
  check (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)), "[server]Eroare la bind().\n");

  // punem serverul sa asculte daca vin clienti sa se conecteze
  check (listen (sd, 2), "[server]Eroare la listen().\n");

  // servim in mod concurent clientii...folosind thread-uri
  while (1) {
    int client;
    thData * td; // parametru functia executata de thread
    int length = sizeof (from);

    printf ("[server]Asteptam la portul %d...\n",PORT);
    fflush (stdout);

    // acceptam un client (stare blocanta pina la realizarea conexiunii) */
    if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0) {
      perror ("[server]Eroare la accept().\n");
      continue;
    }

    // s-a realizat conexiunea, se astepta mesajul
    int idThread; // id-ul threadului
    int cl; // descriptorul intors de accept

    td = (struct thData*)malloc(sizeof(struct thData));
    td->idThread = i++;
    td->cl = client;

    pthread_create(&th[i], NULL, &treat, td);

	}//while
};

static void *treat(void * arg) {
  struct thData tdL;
  tdL = *((struct thData*)arg);

  printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
  fflush (stdout);

  pthread_detach(pthread_self());     FILE *fis2 = fopen("useri.txt", "r");
  raspunde((struct thData*)arg);

  // am terminat cu acest client, inchidem conexiunea
  close ((intptr_t)arg);
  return(NULL);
};


void raspunde(void *arg) {

  int len1, len2;
  int operatie;
  char nume[50], parola[50];

	// Thread
	struct thData tdL;
	tdL = *((struct thData*)arg);

  read(tdL.cl, &operatie, sizeof(int));
  printf("Selected : %d \n", operatie);

  //Logare
  if (operatie == 1) {

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


      int ok = selectUseri(nume, parola);
      printf("Rezultat %d\n", ok);
      
			// Vedem daca s-a logat sau nu
      write(tdL.cl, &ok, 4);
  }


  // Sign up - Aceeasi logica ca la Login pentru prima parte
  if (operatie == 2) {
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


	    int ok = selectUseri(nume, parola);
      printf("Rezultat %d\n", ok);

			// Daca nu am gasit utilizatorul, il bagam in baza de date
      if (ok == 0){
        printf("Inainte ok cand e 0 \n");
        insertUser(nume, parola);
        printf("Dupa ok cand e 0 \n");
      }

      write(tdL.cl, &ok, 4);
  }

  int g = 1;

  while (g) {
    g = 0;
    char categorii[5000];
    char categorie[30];
    memset(categorii, 0, sizeof(categorii));

		// Afisam categoriile
    strcat(categorii, "1.Imbracaminte.\n");
    strcat(categorii, "2.Incaltaminte.\n");
    strcat(categorii, "3.Mancare.\n");

    int len3 = strlen(categorii);

    write(tdL.cl, &len3, 4);
    write(tdL.cl, categorii, len3);

    int cat;
    read(tdL.cl, &cat, 4);
    printf("Categorie : %d\n", cat);


  if (cat == 1) {

    memset(categorie, 0, sizeof(categorie));
    strcpy(categorie, "Imbracaminte");
    selectCategorie(categorie);

		} else if(cat == 2) {

      memset(categorie, 0, sizeof(categorie));
      strcpy(categorie, "Incaltaminte");
      selectCategorie(categorie);

		} else if( cat == 3) {

     memset(categorie, 0, sizeof(categorie));
     strcpy(categorie, "Mancare");
     selectCategorie(categorie);

    } else {
      printf("Alege un numar valid.");
    }


    write(tdL.cl, produse, sizeof(produse));
    write(tdL.cl, cost, sizeof(cost));

    // printf("rezulta imbra: %s", imbracaminte[0]); 

    for( int i = 0; i < 4; i++) {
      printf("rezulta imbra: %s", produse[i]);
    }

     for( int i = 0; i < 4; i++) {
      printf("rezulta incalta: %s", cost[i]);
    }

    printf("\n");
    read(tdL.cl, &g, 4);
  }
}

int check(int exp, const char *msg){

  if (exp == -1){
    perror(msg);
    exit(1);
  }
  return exp;
}

int selectUseri(char* name, char* pass) {
    sqlite3 *db;
    char *err_msg = 0;
    sqlite3_stmt *res;
    
    int rc = sqlite3_open("shopper.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    
    char *sql = "SELECT nume, parola FROM Useri WHERE nume = ? and parola = ?";
        
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    if (rc == SQLITE_OK) {

      sqlite3_bind_text(res, 1, name, -1, SQLITE_STATIC);
      sqlite3_bind_text(res, 2, pass, -1, SQLITE_STATIC);

    } else {
        
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    
    int step = sqlite3_step(res);
    
    if (step == SQLITE_ROW) {

        // printf("%s: ", sqlite3_column_text(res, 0));
        printf("%s ", sqlite3_column_text(res, 0));
        printf("%s \n", sqlite3_column_text(res, 1));
        printf("intra aici\n");
        
    } else {
      printf("nu are rand\n");
      return 0;
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
    
    return 1;
}

void insertUser( char* nume , char* parola ){

  sqlite3 *db ;
  char *err_msg = 0;
    
  int rc = sqlite3_open("shopper.db", &db);
    
  if (rc != SQLITE_OK) {
    
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
  }
  
  char sql[200] =  "INSERT INTO Useri (nume, parola) VALUES ( '" ;
    strcat(sql , nume );
    strcat(sql , "','" );
    strcat(sql , parola );
    strcat(sql , "');" );

  printf("query %s \n", sql);

  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
  if (rc != SQLITE_OK ) {
      
      fprintf(stderr, "SQL error: %s\n", err_msg);
      
      sqlite3_free(err_msg);        
      sqlite3_close(db);
        
  } else {

    printf("S-a inserat \n");
  }
    
    sqlite3_close(db);
   
}

void selectCategorie(char categorie[]) {
    memset(produse, 0, sizeof(produse));
    memset(cost, 0, sizeof(cost));
    n = 0;
    m = 0;
    sqlite3 *db;
    char *err_msg = 0;
    sqlite3_stmt *res;
    

    int rc = sqlite3_open("shopper.db", &db);
    
    if (rc != SQLITE_OK) {
      
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
    }
    
    char sql[200] = "SELECT * FROM ";
    strcat(sql , categorie );
    strcat(sql , ";" );

    printf(" \nSQL%s \n", sql );

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    if (rc == SQLITE_OK) {

      printf("s-a facut sql \n");

    } else {
        
      fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    

    while(sqlite3_step(res) == SQLITE_ROW) {

      // printf("%s: ", sqlite3_column_text(res, 0));
      // printf("%s ", sqlite3_column_text(res, 0));
      // printf("%s \n", sqlite3_column_text(res, 1));

      strcpy(produse[n], sqlite3_column_text(res, 0) );
      strcat(produse[n] , " ");
      strcat(cost[m] , sqlite3_column_text(res, 1));
      strcat(cost[m] , "\n");

      n++;
      m++;
    } 
}

