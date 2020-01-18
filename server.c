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
#include <stdint.h>


#define PORT 2908

// codul de eroare returnat de anumite apeluri
extern int errno;

int n = 0;
int m = 0;
char produse[200][200];
char cost[200][200];


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

  pthread_detach(pthread_self()); 
  raspunde((struct thData*)arg);

  // am terminat cu acest client, inchidem conexiunea
  close ((intptr_t)arg);
  return(NULL);
};


void raspunde(void *arg) {

  int lungimeNume;
  int lungimeParola;
  int operatie;
  int succes;
  int continua = 1;
  char nume[50], parola[50];

	// Thread
	struct thData tdL;
	tdL = *((struct thData*)arg);

  read(tdL.cl, &operatie, sizeof(int));
  printf("Operatia selectata : %d \n", operatie);
 
  switch (operatie) {
    case 1:

      read(tdL.cl, &lungimeNume, sizeof(int));
      read(tdL.cl, nume, lungimeNume);
      nume[lungimeNume] = '\0';

      read(tdL.cl, &lungimeParola, sizeof(int));
      read(tdL.cl, parola, lungimeParola);
      parola[lungimeParola] = '\0';

      printf("\n Numele si parola: %s %s\n", nume, parola);
      printf("\n Lungime nume si lungime parola: %d %d \n", lungimeNume, lungimeParola);

      succes = selectUseri(nume, parola);
      printf("\n Se afla in baza de data userul si parola: %d\n", succes);
      
      write(tdL.cl, &succes, sizeof(int));
      break;

    case 2:

      read(tdL.cl, &lungimeNume, sizeof(int));
      read(tdL.cl, nume, lungimeNume);
      nume[lungimeNume] = '\0';

      read(tdL.cl, &lungimeParola, sizeof(int));
      read(tdL.cl, parola, lungimeParola);
      parola[lungimeParola] = '\0';

      printf("\n Numele si parola: %s %s\n", nume, parola);
      printf("\n Lungime nume si lungime parola: %d %d \n", lungimeNume, lungimeParola);

      succes = selectUseri(nume, parola);
      printf("\n Se afla in baza de data userul si parola: %d\n", succes);

      if (succes == 0) {
        insertUser(nume, parola);
      }

      write(tdL.cl, &succes, sizeof(int));
      break;

    default:
      break;
  }


  while (continua) {

    char categorii[500];
    char categorie[30];
    int lungimeCategorii;
    int gen;
    continua = 0;
  
    memset(categorii, 0, sizeof(categorii));

    strcat(categorii, "\n 1. Imbracaminte\n");
    strcat(categorii, "\n 2. Incaltaminte\n");
    strcat(categorii, "\n 3. Mancare\n");

    lungimeCategorii = strlen(categorii);

    write(tdL.cl, &lungimeCategorii, sizeof(int));
    write(tdL.cl, categorii, lungimeCategorii);

    read(tdL.cl, &gen, sizeof(gen));
    printf("\n Categorie: %d \n", gen);

    switch (gen) {

      case 1:
        memset(categorie, 0, sizeof(categorie));
        strcpy(categorie, "Imbracaminte");
        selectCategorie(categorie);
        break;

      case 2:
        memset(categorie, 0, sizeof(categorie));
        strcpy(categorie, "Incaltaminte");
        selectCategorie(categorie);
        break;

      case 3:
        memset(categorie, 0, sizeof(categorie));
        strcpy(categorie, "Mancare");
        selectCategorie(categorie);
        break;

      default:
        break;  
    }

    write(tdL.cl, produse, sizeof(produse));
    write(tdL.cl, cost, sizeof(cost));

    for ( int i = 0; i < 4; i++) {
      printf("\nRezultatul produselor: %s\n", produse[i]);
    }

    for ( int i = 0; i < 4; i++) {
      printf("\nRezultatul costului: %s\n", cost[i]);
    }

    read(tdL.cl, &continua, sizeof(int));
  }
}

int check(int exp, const char *msg) {

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

    printf("\n Numele: %s ", sqlite3_column_text(res, 0));
    printf("Parola: %s \n", sqlite3_column_text(res, 1));

  } else {

    printf("\n Nu exista acest user si parola in DB. \n");
    return 0;

  }

  sqlite3_finalize(res);
  sqlite3_close(db);
  
  return 1;
}

void insertUser( char* nume , char* parola ) {

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

  printf(" \n SQL:  %s \n", sql);

  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
  if (rc != SQLITE_OK ) {
      
      fprintf(stderr, "SQL error: %s\n", err_msg);
      
      sqlite3_free(err_msg);        
      sqlite3_close(db);
        
  } else {
    printf("\n S-a inserat un rand in DB.\n");
  }
    
  sqlite3_close(db);
   
}

void selectCategorie(char categorie[]) {

  memset(produse, 0, sizeof(produse));
  memset(cost, 0, sizeof(cost));
  sqlite3 *db;
  char *err_msg = 0;
  sqlite3_stmt *res;
  n = 0;
  m = 0;
    
  int rc = sqlite3_open("shopper.db", &db);
    
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
  }
    
  char sql[200] = "SELECT * FROM ";
    strcat(sql , categorie );
    strcat(sql , ";" );

  printf("\n SQL: %s \n", sql );

  rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
  if (rc == SQLITE_OK) {

    printf("\n S-a facut SQL. \n");

  } else {
      
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }
    

  while (sqlite3_step(res) == SQLITE_ROW) {

    strcpy(produse[n], sqlite3_column_text(res, 0) );
    strcat(produse[n] , " ");
    strcat(cost[m] , sqlite3_column_text(res, 1));
    strcat(cost[m] , "\n");

    n++;
    m++;

  } 
}

