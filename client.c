#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>


// codul de eroare returnat de anumite apeluri
extern int errno;

// portul de conectare la server 
int port;

int check(int, const char*);

int main (int argc, char *argv[]) {

  int sd;	// descriptorul de socket
  char buf[10];

  struct sockaddr_in server; // structura folosita pentru conectare
  
  // exista toate argumentele in linia de comanda?
  if (argc != 3) {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
  }

  // stabilim portul
  port = atoi (argv[2]);

  /* cream socketul */
  check (sd = socket (AF_INET, SOCK_STREAM, 0),"Eroare la socket().\n");

  /* umplem structura folosita pentru realizarea conexiunii cu serverul
  familia socket-ului */
  server.sin_family = AF_INET;
  // adresa IP a serverului
  server.sin_addr.s_addr = inet_addr(argv[1]);
  // portul de conectare
  server.sin_port = htons (port);

  /* ne conectam la server */
  check(connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)), "[client]Eroare la connect().\n" );



  int operatie;
  int succes;
  int lungimeNume;
  int lungimeParola;
  int gen;
  int sumaTotala = 0;
  int sumaCategorii[5];
  int lungimeCategorii;
  char categorii[500];
  char categorie[30];
  char produse[200][200];
  char cost[200][200];
  char nume[50], parola[50];

  printf("\n \t \t Bine ati venit la Shopper! \n \
  \t >-------------------------------------< \n \
  Pentru a putea continua selectati una din cele doua variante.\n\n\
  1. Logare \n\n\
  2. Inregistrare\n\n");

  printf(" Operatia: "); 
  scanf("%d", &operatie);

  if (operatie != 1 && operatie !=2 ) {

    printf("\n Ups.Selecteaza operatia 1 pentru logare sau 2 pentru inregistrare.\n");
    printf(" \n Operatia: ");
    scanf("%d", &operatie);

  }

  switch (operatie) {
    case 1:

      write(sd, &operatie, sizeof(int));

      printf("\n Utilizator: "); 
      scanf("%s", nume);

      printf("\n Parola: ");
      scanf("%s", parola);

      lungimeNume = strlen(nume);
      write(sd, &lungimeNume, sizeof(int));
      write(sd, nume, lungimeNume);

      lungimeParola = strlen(parola);
      write(sd, &lungimeParola, sizeof(int));
      write(sd, parola, lungimeParola);

      read(sd, &succes, sizeof(int));

      if (succes == 1) {
        printf("\n Te-ai logat cu succes. \n\n");
      } else {
        printf("\n Ne pare rau. Mai incearca.\n");
        return 0;
      }
      break;

    case 2:

      write(sd, &operatie, sizeof(int));

      printf("\n Utilizator: "); 
      scanf("%s", nume);

      printf("\n Parola: "); 
      scanf("%s", parola);

     
      lungimeNume = strlen(nume);
      write(sd, &lungimeNume, sizeof(int));
      write(sd, nume, lungimeNume);

      lungimeParola = strlen(parola);
      write(sd, &lungimeParola, sizeof(int));
      write(sd, parola, lungimeParola);

      read(sd, &succes, sizeof(int));

      if (succes == 0) {
        printf("\n Te-ai inregistrat cu succes. \n\n");
      } else {
        printf("\n Exista deja contul in baza de date. \n");
        return 0;
      }
      break;

    default:
      break;
  }

  int continua = 1;
  while (continua) {

    memset(categorii, 0, sizeof(categorii));
    continua = 0;

    read(sd, &lungimeCategorii, sizeof(int));
    read(sd, categorii, lungimeCategorii);

    printf("Categoriile: \n %s \n\n", categorii);

    printf("Alege categoria: ");
    scanf("%d", &gen);
    write(sd, &gen, sizeof(int));

    read(sd, produse, sizeof(produse));
    read(sd, cost, sizeof(cost));

    printf("\n Produse   Cost(Lei) \n\n");
    for(int i = 0; i < 4; i++){
        printf("%d. %s  %s\n", i + 1, produse[i], cost[i]);
    }

    memset(sumaCategorii, 0, sizeof(sumaCategorii));

    int loop = 1;
    int numarProduse = 0;
    int sumaProduse = 0;
    while(loop) {

      printf(" Selecteaza ce produs vrei. Apasa 0 cand termini. \n");
      printf("\n Numar produs: ");
      scanf("%d", &loop);

      if (loop > 0 && loop < 5) {

        sumaCategorii[numarProduse++] = loop;
        printf("\n A fost adaugat produsul %d in cos. \n\n", loop);

      } else {
        printf("\n Produsul nu exista in cos. \n\n");
      }
    }

    // for( int i = 0; i < numarProduse; i++) {
    //   printf(" Produsele pe care le-ai ales: %d \n", sumaCategorii[i] );
    // }


    for(int i = 0; i < numarProduse; i++) {
        sumaProduse = sumaProduse  + atoi(cost[sumaCategorii[i] - 1]);
    }

    printf("\nSuma totala din categoria asta = %d LEI\n\n\n", sumaProduse);
    sumaTotala = sumaTotala + sumaProduse;
    
    printf("Daca vrei sa alegi produse din alta categorie apasa 1. \n \
    Daca vrei sa iesi apasa 0. \n");

    printf("\n Operatie: ");
    scanf("%d", &continua);
    write(sd, &continua, sizeof(int));
 }

  printf("Suma produselor din toate categoriile: %d\n", sumaTotala);
}

int check(int exp, const char *msg){

  if (exp == -1){
    perror(msg);
    exit(1);
  }
  return exp;
}
