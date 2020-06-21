/// @file client.c
/// @brief Contiene l'implementazione del client.

#include "defines.h"
#include "err_exit.h"

char fifoPID[50] = "";
char charPID[20] = "";
char int_to_char[11];
int msqid = -1;

int main(int argc, char * argv[]) {
  
    if (argc != 2) {
      ErrExit("<Client Error> Elementi non corretti");
      return 1;
    }
    Message messaggio;
    //Inserimento PID
    printf("Inserire il PID, l'id del messaggio e la massima distanza:\n");
    scanf("%d %d %lf", &messaggio.pid_receiver, &messaggio.message_id, &messaggio.max_distance);
    printf("Inserire il messaggio (massimo 256 caratteri):\n");
    scanf("%s", messaggio.message);
    // read the FIFO's pathname
    int msq_queue_key = atoi(argv[1]);
    strcpy(fifoPID, PATHFIFO);
    sprintf(int_to_char, "%d", messaggio.pid_receiver);
    strcat(fifoPID, int_to_char);
    printf("<Client> Apertura FIFO %s...\n", fifoPID);
    // Open the FIFO in write-only mode
    int deviceFIFO = open(fifoPID, O_WRONLY);
    if (deviceFIFO == -1)
        ErrExit("<Client Error> Apertura della FIFO fallita");
    printf("<Client> Scrittura del messaggio nella FIFO\n");
    // Wrinte  two integers to the opened FIFO
    if (write(deviceFIFO, &messaggio, sizeof(Message)) != sizeof(Message))
        ErrExit("<Client Error> Scrittura Fallita");

    // Close the FIFO
    if (close(deviceFIFO) != 0)
        ErrExit("<Client Error> Chiusura Fallita");

   // read the massage queue
    //Acknowledgment acknowledgment;

    for(int i; i < 5; i++)
      //size_t mSize = (sizeof(acknowledgment) - sizeof(long));
      //if(msgrcv(msqid, &acknowledgment, mSize, 0, 0) == -1)
    return 0;
}