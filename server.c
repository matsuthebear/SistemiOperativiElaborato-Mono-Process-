/// @file server.c
/// @brief Contiene l'implementazione del SERVER.

// Librerie Custom
#include "err_exit.h"
#include "defines.h"
#include "shared_memory.h"
#include "semaphore.h"
#include "fifo.h"

//Funzioni
void handler(int sig){
  _exit(0);
}
//Costanti 
#define NUMERO_DEVICES 5 //Numero dei devices all'interno del file posizioni
#define GRANDEZZA_SCACCHIERA 10
#define NUMERO_ACK_MAX 100 //Numero di ack massimi nell'array acklist
#define LUNGHEZZA_BUFFER 20 //Numero caratteri per riga
#define _BOOL true //Per la gestione di Test senza impattare il programma

//Variabili 
/// Device
Device * devices; //Storage dei devices 
pid_t pid; //pid per gestire i devices
pid_t pid_server;
int shared_memory_devices; //memoria condivisa devices
int fifo_device; //Descrittore fifo del device
int status;
/// Message Queue Key
int message_queue_key;
/// Messaggio
Message messaggio; //Struttura base, permette di copiare il messaggio
int message_fifo; //Lettura della FIFO 
/// Acknowledgment
Acknowledgment * acknowledgment; //lista ack
int shared_memory_acknowledgment; //memoria condivisa ack
/// Scacchiera
Scacchiera * scacchiera; //Scacchiera 10x10
int shared_memory_scacchiera;  //memoria condivisa scacchiera
int posizioni[NUMERO_DEVICES  * 2]; //Ogni device ha x,y. 
int x,y; //Le posizioni dei devices
double raggio, distanza_massima, distanza_minima; //Raggio tra i device, raggio massimo, raggio minimo
pid_t device_min; //Il device con il raggio minimo trovato
/// Semaforo
int semaforoDevices; //semaforo per l'accesso alla m.c devices
int semaforoAckList; //semaforo per l'accesso alla m.c. acklist
int semDevInitVal[NUMERO_DEVICES] = {1,0,0,0,0}; //valori iniziali del semaforo
union semun arg; //Unione descritta in semaphore.h
int semAckInitVal[2] = {1,0};
/// File
int file_posizioni; //file delle posizioni
char buffer[LUNGHEZZA_BUFFER + 1]; //buffer in lettura per le posizioni
ssize_t buffer_reader = 0;
int fifo_write_descriptor; //Descrittore in apertura della fifo del device
/// Altro
char int_to_char[11]; //variabile di supporto per convertire int a char/string
double max_distance; //distanza max tra i device, descritta dal client
int check; //check per vari scopi
int code = 2;

int main(int argc, char *argv[]){
  //Inizio Programma
  printf("<Server> Inizio Programma...\n");
  printf("<Server> Ottenimento Message Queue Key... ");
  //Ottenimento Message Queue Key dato in input da terminale
  if (argv[1] != NULL){
      printf("OK!\n");
      message_queue_key = atoi(argv[1]); //Converte la stringa in intero
  }
  else{
    printf("Errore!\n<Server Error> La Message Queue Key e' nulla!\n");
    return 1; //ErrExit non funziona per questo
  }
  //Ottenimento Path Posizioni e Apertura File in lettura
  printf("<Server> Apertura file posizioni... ");
  file_posizioni = open(argv[2], O_RDONLY | O_NONBLOCK);
  if (file_posizioni == -1){
    printf("Errore!\n");
    ErrExit("<Server Error> Apertura fallita");
  }else{
    printf("OK!\n");
  }
  delay(2);
  //Creazione Memorie Condivise  
  ///Acknowledgment
  printf("<Server> Creazione Memorie Condivise... ");
  shared_memory_acknowledgment = alloc_shared_memory(message_queue_key, sizeof(Acknowledgment) * NUMERO_ACK_MAX);
  acknowledgment = (Acknowledgment *)shmat(shared_memory_acknowledgment,0,0);
  ///Scacchiera
  shared_memory_scacchiera = alloc_shared_memory(message_queue_key + 1, sizeof(Scacchiera));
  scacchiera = (Scacchiera *)shmat(shared_memory_scacchiera,0,0);
  /// Lista Devices
  shared_memory_devices = alloc_shared_memory(message_queue_key + 2, sizeof(Device) * NUMERO_DEVICES);
  devices = (Device *)shmat(shared_memory_devices,0,0); 
  printf("OK!\n");
  delay(2);
  printf("OK!\n");
  //Creazione Devices / FORK() e assegnazione nella 
  //memoria condivisa devices
  printf("<Server> Creazione devices e fifo... ");
  for (int i = 0; i < NUMERO_DEVICES; i++){
    sleep(2);
    pid = fork();
    if(pid == 0){
      devices[i].pid = (pid_t)(getpid());
      //Ottenimento FIFO
      strcpy(devices[i].fifo, PATHFIFO);
      sprintf(int_to_char,"%d", devices[i].pid);
      strcat(devices[i].fifo, int_to_char);
      //Creazione FIFO tramite MKFIFO
      if (mkfifo(devices[i].fifo, S_IRUSR | S_IWUSR | S_IWGRP ) == -1){
        printf("Errore!\n");
        ErrExit("<Server Error> MKFIFO fallita");
      }
      //Ottenimento NAME
      sprintf(int_to_char, "D%d", i + 1);
      strcpy(devices[i].name, int_to_char);
      printf("<Server> Device %s con PID %d generato correttamente!\n", devices[i].name, devices[i].pid);
      //Signal handler
      signal(SIGCONT,handler);
      exit(code);
    }else{
      pid_server = getpid();
    }
  }
  
  printf("OK!\n");
  delay(1);
  printf("<Server> Apertura FIFO devices... ");
  for (int i = 0; i < NUMERO_DEVICES; i++){
      devices[i].fifo_descriptor = open(devices[i].fifo, O_RDONLY | O_NONBLOCK);
      if (devices[i].fifo_descriptor == -1){
        printf("Errore!\n");
        ErrExit("<Server Error> Apertura FIFO fallita");
      }
  }
  printf("OK!\n");
  delay(1);
  //Creazione Semafori
  printf("<Server> Creazione semafori... ");
  semaforoDevices = semget(IPC_PRIVATE, 5, S_IRUSR | S_IWUSR);
  if (semaforoDevices == -1){
    printf("Errore!\n");
    ErrExit("<Server Error> Semaforo non generato");
  }
  arg.array = semDevInitVal;
  if (semctl(semaforoDevices, 0, SETALL, arg) == -1){
    printf(" Errore!\n");
    ErrExit("<Server Error> semctl fallito");
  }
  printf("OK!\n");
  delay(1);
  /*
    Alla creazione dei devices, senza il delay di 1 secondo 
    i devices apparivano o in ordine distorto, o senza FIFO,
    oppure con PID inferiori a quelli creati. La apertura delle 
    FIFO e' stata fatta al di fuori del ciclo. Senza, i descrittori
    sarebbero rimasti nulli / corrotti.
  */
  // RESET scacchiera
  printf("<Server> Reset Scacchiera... ");
  for (int i = 0; i < GRANDEZZA_SCACCHIERA; i++)
    for (int j = 0; j < GRANDEZZA_SCACCHIERA; j++)
      scacchiera -> board[i][j] = 0;
  printf("OK!\n");
  // Ottenimento testo
  printf("<Server> Movimento devices in corso...\n"); 
  do{
    //Legge dal file la stringa, assegna a buffer. 
    //buffer_reader serve per controllare che non legga NULL
    //Controllo aggiuntivo 
    buffer_reader = read(file_posizioni, buffer, LUNGHEZZA_BUFFER);
    if (buffer_reader > 0){
      //get_posizioni e' definito in defines.h
      get_posizioni(buffer, posizioni, NUMERO_DEVICES  * 2, LUNGHEZZA_BUFFER);
      //Ciclo for per i caratteri
      for (int posizione = 0; posizione < NUMERO_DEVICES  * 2; posizione+=2){
        int numero_device = posizione / 2; //Per semplicita'
        //Lock del semaforo per l'utilizzo della scacchiera
        
        //printSemaphoresValue(semaforoDevices, NUMERO_DEVICES);
        semOp(semaforoDevices, (unsigned short)numero_device, -1);
        printf("PID : %d\n", getpid());
        
        semOp(semaforoDevices, (unsigned short)numero_device == 4? 0 : numero_device + 1, 1);
        printSemaphoresValue(semaforoDevices, NUMERO_DEVICES);
        //Spostamento Device
        /*
          Questa sezione di codice permette di eliminare tutti 
          i doppioni di PID nel caso in cui potesse succedere.
          Si hanno gia' x,y, quindi tenere le precedenti copie in 
          posizioni non corrispondenti alla riga posizioni non
          ha senso. Che abbia trovato doppioni o meno, inserisce 
          all'interno della scacchiera la posizione.
        */
        check = 0;
        x = posizioni[posizione];
        y = posizioni[posizione + 1];
        for (int i = 0; i < GRANDEZZA_SCACCHIERA; i++){
         for (int j = 0; j < GRANDEZZA_SCACCHIERA; j++){
           //Se il pid e' uguale a quello del device
           if (scacchiera -> board[i][j] == devices[numero_device].pid){
             if (x != i || y != j){
               //Controlla che non ci siano possibili doppioni
               scacchiera -> board[x][y] = devices[numero_device].pid;
               scacchiera -> board[i][j] = 0;
               check = 1;
               //Spostamento effettuato, non c'e' bisogno di altri cicli
               break;
             }
           }else{
             //Se x = i e y= j, allora lo riscrivo per sicurezza
             scacchiera -> board[x][y] = devices[numero_device].pid;
           }
         }
         if (check > 0)
            break;
        }
        //Lettura FIFO
        /* 
          Legge dalla FIFO fino a quando non ci sono piu' messaggi, 
          quindi per ogni messaggio letto (sequenzialmente) controlla 
          che il messaggio non sia gia' stato recepito da altri device
          trovati. 
        */
        do{
          message_fifo = read(devices[numero_device].fifo_descriptor, &messaggio, sizeof(Message));
          if (message_fifo == -1){
            printf("<Server Error> FIFO %s corrotta!\n", devices[numero_device].fifo);
            //ErrExit("<Server Error>");
            continue;
          }
          else if(message_fifo == sizeof(Message) || message_fifo != 0){
            printf("PID: %d ha un messaggio!\n", devices[numero_device].pid);
            //Invia a ACK LIST la ricezione del messaggio
            for (int ack = 0; ack < NUMERO_ACK_MAX; ack++){
              if (acknowledgment[ack].message_id == messaggio.message_id && acknowledgment[ack].pid_receiver == messaggio.pid_receiver)
                break;
              
              if (acknowledgment[ack].message_id == 0){
                printf("ACK vuoto in %d, ci scrivo sopra\n", ack);
                //La posizione trovata e' vuota, ci scrivo l'ack
                acknowledgment[ack].pid_sender = messaggio.pid_sender;
                acknowledgment[ack].pid_receiver = messaggio.pid_receiver;
                acknowledgment[ack].message_id = messaggio.message_id;
                //Ottengo il timestamp quando invio l'ack
                acknowledgment[ack].timestamp = 0;
                break;
              }
            }
            //Sostituisce i valori di messaggio per l'invio
            messaggio.pid_sender = devices[numero_device].pid;
            //Controlla il device piu' vicino
            max_distance = messaggio.max_distance; //max distanza tra devices
            distanza_minima = max_distance;
            device_min = 0;
            for (int i = 0; i < GRANDEZZA_SCACCHIERA; i++){
              for (int j = 0; j < GRANDEZZA_SCACCHIERA; j++){
                pid = scacchiera -> board[i][j];
                //Il pid non deve essere se stesso e neanche 0
                if (pid > 0 && pid != devices[numero_device].pid){
                  raggio = sqrt((i - x) * (i - x) + (j - y) * (j - y));
                  //La distanza dei device deve essere compresa in max_distance
                  if (raggio <= max_distance){
                    check = 0;
                    if (distanza_minima == 0 || raggio < distanza_minima){
                      //Il device piu' vicino e' stato trovato
                      for (int ack = 0; ack < NUMERO_ACK_MAX; ack++){
                        if (acknowledgment[ack].pid_receiver == pid && acknowledgment[ack].message_id == messaggio.message_id){
                          check = 1;
                          break;
                        }
                      }
                      if (check == 0){
                        device_min = pid;
                        distanza_minima = raggio;
                      }
                    }
                  }
                }
              }
            }
          }
        if (device_min > 0){
          for (int i = 0; i < NUMERO_DEVICES; i++){
            if (device_min == devices[i].pid){
              printf("Scrivo a %d un messaggio\n", devices[i].pid);
              messaggio.pid_receiver = devices[i].pid;
              if ((fifo_write_descriptor = open(devices[i].fifo, O_WRONLY)) == -1){
                printf("<Server Error> Apertura della FIFO in scrittura fallita!\n");
                ErrExit("<Server Error> Errore");
              }
              if (write(fifo_write_descriptor, &messaggio, sizeof(Message)) != sizeof(Message)){
                ErrExit("<Server Error> Scrittura Fallita");
              }
              if (close(fifo_write_descriptor) != 0){
                ErrExit("<Server Error> Chiusura fallita");
              }
              i = NUMERO_DEVICES;
              break;
            }
          }
        }
        }while(message_fifo != 0);
        //Unlock del semaforo, il device ha fatto tutto quello che doveva fare
        
      }
    }
  printf("\n");
  delay(2);
  }while(buffer_reader > 0);
  // TEST : stampa i devices con PID, nome e FIFO
  if (_BOOL)
  for (int i = 0; i < NUMERO_DEVICES; i++)
    printf("Device : %s PID : %d FIFO: %s\n", 
    devices[i].name, devices[i].pid, devices[i].fifo);
  //Terminazione dei processi e FIFO
  for (int i = 0; i < NUMERO_DEVICES; i++){
    close(devices[i].fifo);
    if (unlink(devices[i].fifo) != 0)
      ErrExit("<Server Error> Unlink fallito");
  }
  
  //Chiusura File
  close(file_posizioni);
  //Rimozione processi
  printf("OK!\n<Server> Chiusura processi... ");
  while (wait(NULL) != -1);
  //Rimozione dei semafori creati

  //Free delle memorie condivise 
  printf("<Server> Free delle memorie condivise... ");
  free_shared_memory(devices);
  free_shared_memory(acknowledgment);
  free_shared_memory(scacchiera);
  printf("OK!\n<Server> Rimozione delle memorie condivise... ");
  remove_shared_memory(shared_memory_scacchiera);
  remove_shared_memory(shared_memory_acknowledgment);
  remove_shared_memory(shared_memory_devices);
 
  printf("OK!\n<Server> Terminazione programma...\n");


  return 0;
}