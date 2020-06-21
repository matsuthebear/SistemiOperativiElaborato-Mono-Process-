/// @file semaphore.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione dei SEMAFORI.
#include <sys/sem.h> //Definisce gli elementi per i semafori

#include "err_exit.h"
#include "semaphore.h"
#include <stdio.h>
// semOp: permette di bloccare/sbloccare un semaforo a seconda 
// dell'andamento del programma. Quando sem_op = -1, allora il 
// semaforo viene bloccato appena le condizioni lo permettono
// (ossia se sottraendo 1 non si ottenga un valore negativo). 
// Quando questo è uguale a 1, il semaforo (se bloccato) viene 
// sbloccato (ovvero si somma 1 al valore del semaforo)

void semOp(int semid, unsigned short sem_num, short sem_op){
  // Struttura del semaforo, spiegato in semaphore.h
  struct sembuf sop = {.sem_num = sem_num, .sem_op = sem_op, .sem_flg = 0};

  if (semop(semid, &sop, 1) == -1)
    ErrExit("semop failed");
}

void printSemaphoresValue(int semDev, int processes){
  unsigned short semVal[processes];
  union semun arg;
  arg.array = semVal;

  if(semctl(semDev,0,GETALL,arg) == -1)
    ErrExit("semctl GETALL failed");

  printf("<Server Test> Semaphore set state : \n");
  for(int i = 0; i < processes; i++)
    printf("id : %d --> %d \n",i,semVal[i]);

}