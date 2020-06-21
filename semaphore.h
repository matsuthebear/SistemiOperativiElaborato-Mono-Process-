/// @file semaphore.h
/// @brief Contiene la definizioni di variabili e funzioni
///         specifiche per la gestione dei SEMAFORI.

// #PRAGMA ONCE e' necessario in quanto semaphore.h e' usato
// sia su semaphore.c che su server.c
// Senza darebbe errore di conflitto. 
#pragma once

// Definizione della unione semun
union semun {
  int val;
  struct semid_ds * buf;
  unsigned short * array;
};
// semid è l'identificatore del semaforo, sem_num è l'index del 
// semaforo nel set, sem_op è l'operazione eseguita in sem_num
void semOp(int semid, unsigned short sem_num, short sem_op);

// semDev è il semaforo utilizzato, processes il numero dei processi. Questa funzione 
// permette di controllare lo stato del semaforo 
void printSemaphoresValue(int semDev, int processes);