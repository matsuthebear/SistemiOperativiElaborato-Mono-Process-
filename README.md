# Elaborato Sistemi Operativi
### Introduzione
Questa documentazione è essenziale per comprendere al meglio i vari elementi del progetto, quali struttura, funzioni non insite di C e create a custom per risolvere alcuni problemi affrontati e ideologia nella scrittura del codice. 
### Descrizione
L'elaborato di Sistemi Operativi dell'anno accademico 2019/2020 richiedeva di creare una applicazione per il trasferimento di messaggi tra 5 dispositivi mobili, denominati device D1, ..., D5, di cui ognuno è processo figlio del server
Questi devices si muovono all'interno di una scacchiera 10x10 seguendo le posizioni designate in un file __file_posizioni.txt__. Ogni cella contiene __0 se vuota__ o il __PID del processo__ (del device). Un client (o multipli) può connettersi tramite la FIFO del device per passargli un messaggio. 
Quando il client riceve il messaggio nella coda di messaggi genera un file __out_message_id.txt__ e vi scrive la lista di 5 acknowledgement che identificano i passaggi datti dal messaggio, con i relativi istanti di tempo, terminando. Ogni step viene eseguito __ogni due secondi__ e i movimenti sono eseguiti in ordine. Quando non sono presenti ulteriori posizioni, il server termina tutti i processi, le memorie e tutto ciò che è collegato al programma. 
Si possono vedere i dettagli all'interno del documento __progetto_syscalls.pdf__.

### Gestione degli errori
All'interno del programma ci sono due tipologie di errori: quelli che vengono descritti da ErrExit e quelli descritti personalmente, che non avevano bisogno (in parte) di essere richiamati con la funzione ErrExit. Questa scelta e' stata effettuata per trovare meglio gli errori all'interno del programma e per sostituire alcune mancanze nell'ErrExit (ad esempio, per la gestione degli argomenti, se l'argomento e' nullo da segmentation fault, ma ErrExit dara' comunque risultato positivo per qualche motivo). 

### Memorie Condivise e Devices 
Nel programma si puo' notare che e' presente una __ulteriore__ memoria condivisa abilitata solo per i devices. Tramite la memoria condivisa, abbiamo potuto creare delle strutture ad-hoc dei Devices. I vari devices possono comunicare tra di loro aprendo le corrispondenti fifo molto piu' facilmente ( la struttura Device ha pid, name e fifo), essendo salvate direttamente nel Device corrispondente, senza ogni volta richiamare funzioni che facevano la stessa cosa ma in modo molto piu' complicato.

### Processi e Semafori
La sfida principale di questo progetto e' la presenza, rispetto agli elaborati degli anni scorsi, di piu' processi (invece del classico modello client-server) creati all'interno del processo server (5 devices + 1 ack), che devono in primis comunicare tra di loro tramite fifo e sfruttare 3 set di semafori per essere sincronizzati. L'utilizzo dei segnali e' stato __essenziale__ per mettere in avvio/pausa i vari devices. La sincronizzazione tra i devices e' realizzata, in parte, dalla presenza della funzione __pause()__ al momento della creazione del processo. Difatti, ogni processo non fa nulla dopo la creazione fino a quando non si spostano all'interno della scacchiera, dove devono usare un semaforo per l'accesso. 
Quando deve iniziare il primo processo, il server invia un segnale ai processi per "svegliarli". Questi processi sono quindi sincronizzati e gestiti tramite il semaforo.

### Struttura del Programma 