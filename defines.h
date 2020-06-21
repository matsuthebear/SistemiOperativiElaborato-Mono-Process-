/// @file defines.h
/// @brief Contiene le definizioni di variabili
///         e funzioni specifiche del progetto.

#pragma once
//Librerie C
#include <stdbool.h> //Booleani
#include <stdio.h> //Standard Input - Output, necessaria per C
#include <time.h> // Serve per gestire il tempo (delay, timestamp)
#include <ctype.h> // Dichiara funzioni utilizzate per la classificazione dei caratteri
#include <stdlib.h> //Funzioni e costanti di utilita' generale
#include <unistd.h> // Permette l'accesso alle API dello standard POSIX
#include <string.h> //Per lavorare con le stringhe
#include <signal.h> //Segnali per chiudere il server
#include <math.h> // Funzioni matematiche pow() e sqrt()
#include <sys/types.h> // pid_t, time_t, key_t e size_t
#include <sys/stat.h> // Flags S_
#include <sys/sem.h> // Creazione e gestione dei semafori
#include <sys/shm.h> // 
#include <sys/ipc.h> //IPC : messaggi, semafori e memoria condivisa
#include <fcntl.h> //Flags F_ 
#include <sys/msg.h> // Per creazione dei processi pid_t
//#include <errno.h> // Definizioni macro per la gestione delle situazioni di errore,
// presente in ERR_EXIT.H
#include <sys/wait.h>

//Costanti
/* 
  Dopo aver controllato piu' volte nel corso delle varie versioni del programma,
  abbiamo notato che, sebbene siano presenti comunicazioni client-device e device-device, 
  gli unici che hanno la FIFO sono i devices. Il client infatti apre in scrittura la FIFO 
  del device scelto. Quindi PATHFIFO e' la FIFO per definizione 
*/
#define PATHFIFO "/tmp/dev_fifo."

//Strutture
/* 
  Device 
  name : Il nome del device. Per semplicita' abbiamo implementato questo valore 
  pid : Il PID del device, viene assegnato solo al momento del fork() nel server
  pathFIFO : La FIFO del device, viene immagazzinata per semplicita'
*/
typedef struct{
  char name[2];
  pid_t pid; 
  char fifo[20];
  int fifo_descriptor;
  int status;
}Device;
/* 
  Message
  pid_sender : Processo che ha inviato il messaggio
  pid_receiver : Processo che ricevera' il messaggio 
  message_id : Id del messaggio deciso dal client
  message : Contenuto del messaggio
  max_distance : Distanza massima in double (distanza euclidea o raggio)
*/
typedef struct{
  pid_t pid_sender;
  pid_t pid_receiver; 
  int message_id;
  char message[256]; 
  double max_distance; 
}Message;

//Funzioni

/* 
  delay 
  Input : unsigned int 
  Output : void 
  Info : La funzione permette di "bloccare" l'intero programma per N secondi, presi in input
  dalla funzione
*/
void delay(unsigned int secondi);

/*
  get_posizioni
  Input : char * buffer, 
          int * posizioni, 
          int numero_posizioni, 
          int lunghezza_buffer
  Output : void
  Info : La funzione prende in input il buffer (array di char), 
  l'array contenente le posizioni, il numero di posizioni desiderate e 
  la lunghezza del buffer. Modifica l'array posizioni, inserendo 
  all'interno i numeri trovati all'interno del buffer
*/
void get_posizioni(char * buffer, int * posizioni, int numero_posizioni, int lunghezza_buffer);
