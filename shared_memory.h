/// @file shared_memory.h
/// @brief Contiene la definizioni di variabili e funzioni
///         specifiche per la gestione della MEMORIA CONDIVISA.

//IMPORT LIBRERIE
#pragma once
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h> //per definizione key_t
#include <stddef.h> //per definizione NULL


// la struttura request definisce la richiesta mandata dal client
// crea una tabella 10*10 di memoria condivisa
typedef struct {
    int board[10][10]; //scacchiera 10x10
    key_t shmKey; //chiave
}Scacchiera;

//Gli acknowledgment salvati nella memoria condivisa devono avere questa struttura
typedef struct{
  pid_t pid_sender;
  pid_t pid_receiver;
  int message_id;
  time_t timestamp;
}Acknowledgment;
 
//la funzione alloc_shared_memory crea, se non esiste un segmento di
//memoria condivisa con con grandezza size e key shmKey.
// in caso di successo ritorna l'shmkey se no termina il processo chiamante.
int alloc_shared_memory(key_t shmKey, size_t size);


//la funzione get_shared_memory attacca un porzione di memoria condivisa
// allo spazio di indirizzo logico del processo chiamante
// ritorna un puntatore al segmento di memoria o in caso negativo termina il processo
void *get_shared_memory(int shmid, int shmflg);


// la funzione free_shared_memroy libera un segmento di memoria condivisa
// dall'indirizzo logico del processo chiamante
// se fallisce , termina il processo chiamante
void free_shared_memory(void *ptr_sh);


//la funzione remove_shared_memory rimuve un segmento di memoria condivisa
// se fallisce, termina il processo chiamante.
void remove_shared_memory(int shmid);

