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


  // Shopper
  int selected;
  int response;
  char name[50], pass[50];

  printf("\n \
     Bine ati venit la Shopper \n \
  >---------------------------------< \n \
  Alegeti una din varianta alegand o cifra\n\n\
  1.Login \n");

  // Login
  scanf("%d", &selected);

  if (selected == 1) {

    // Trimitem la server operatia
    write(sd, &selected, sizeof(int));

    printf("User :"); scanf("%s", name);
    printf("Parola :"); scanf("%s", pass);

    //printf("%s %s", nume, parola);
    fflush(stdout);

    write(sd, name,  sizeof(char));
    write(sd, pass,  sizeof(int));
    read(sd, &response, sizeof(int));

    if (response == 1) {
      printf("Logat cu succes.\n\n\n");
      printf("\n");
    } else {
      printf("Mai incearca.");
      printf("\n");
      return 0;
    }
  }

  // if (operatie == 2) {
  //   write(sd, &operatie, 4);
  //   printf("User :"); scanf("%s", nume);
  //   printf("Parola :"); scanf("%s", parola);
  //   int len1 = strlen(nume);
  //   int len2 = strlen(parola);
  //   fflush(stdout);
  //   write(sd, &len1, 4);
  //   write(sd, nume, len1);
  //   write(sd, &len2, 4);
  //   write(sd, parola, len2);
  //   int ok;
  //   read(sd, &ok, 4);

  //   if (ok == 0){
  //     printf("Inregistrat cu succes.\n\n\n");
  //     printf("\n");
  //   } else {
  //     printf("Account folosit..");
  //     printf("\n");
  //     return(0);
  //   }
  // }
  //   int totSum = 0;
  //   int vector[200];
  //   int g = 1;

  //   while(g) {
  //     g = 0;
  //     char categorii[5000];
  //     memset(categorii, 0, sizeof(categorii));
  //     int len3;
  //     read(sd, &len3, 4);
  //     read(sd, categorii, len3);
  //     printf("%s",categorii);
  //     printf("\n\n");
  //     int cat;
  //     printf("Alege categoria: "); scanf("%d", &cat);
  //     // Trimitem la server categoria
  //     write(sd, &cat, 4);
  //     int n;
  //     char c[int check(int exp, const char *msg)50][50];
  //     char p[50][50];
  //     // Serverul ne intoarce numarul de categorii
  //     read(sd, &n, 4);

  //     // Serverul intoarce vectorii cu produsele
  //     read(sd, p, 2500);
  //     read(sd, c, 2500);
  //     printf("N = %d\n\n", n);

  //     // Iteram peste produse
  //     for(int i=0;i<n;i++){
  //         printf("%d. %s -> %s RONI \n", i + 1, p[i], c[i]);
  //     }

  //     // Golim vectorul
  //     memset(vector, 0, sizeof(vector));

  //     int x = 1;

  //     while(x) {
  //        printf("Selecteaza item-ul pe care vrei sa il adaugi in cos. Tastezi 0 cand termini. \n");
  //        scanf("%d", &x);
  //        vector[x]++;
  //        printf("A fost adaugat item-ul %d in cos. \n\n", x);
  //     }
  //     printf("\n----------------------------------------------\n");
  //     int sum = 0;

  //     for(int i = 1; i < 50; i++){
  //       if (vector[i]) {
  //           sum += atoi(c[i-1]) * vector[i];
  //           printf("Elementul %d este de %d ori in cos.\n\n", i, vector[i]);
  //       }
  //     }
  //     printf("-----------------------------------------------");
  //     printf("\nSuma  produselor alese din categoria asta = %d RONI\n\n\n", sum);
  //     totSum += sum;
  //     printf("0.Checkout.\n");
  //     printf("1.Alege produse din alta categorie.\n");
  //     scanf("%d", &g);
  //     write(sd, &g, 4);
  //  }

  //   printf("Total spre plata : %d\n", totSum);
}

int check(int exp, const char *msg){

  if (exp == -1){
    perror(msg);
    exit(1);
  }
  return exp;
}
