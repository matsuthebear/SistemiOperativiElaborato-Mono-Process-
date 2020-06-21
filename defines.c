/// @file defines.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche del progetto.

#include "defines.h"

void delay(unsigned int secondi){
  unsigned int fine = time(0) + secondi;
  while(time(0) < fine); //Aspetta n secondi prima 
}

void get_posizioni(char * buffer, int * posizioni, int numero_posizioni, int lunghezza_buffer){
  char numero;
  int contatore = 0;
  //Ottengo le posizioni che mi servono
  for (int carattere = 0; carattere < lunghezza_buffer; carattere++){
    numero = *(buffer + carattere);
    if (isdigit(numero)){
      // Se il numero e' un intero, lo inserisco nelle posizioni e lo casto correttamente
      posizioni[contatore] = (int)numero - '0';
      contatore++;
      if (contatore == numero_posizioni)
        break; //Se ho raggiunto le posizioni richieste, non ho bisogno di proseguire
    }
  }
}
