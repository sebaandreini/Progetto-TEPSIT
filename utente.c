#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char nome[100];          // Nome dell'utente
    char destinatario[100];  // Destinatario del messaggio
    char messaggio[1024];    // Contenuto del messaggio
    char buffer[2048];       // Buffer per la scrittura nella FIFO

    const char *fifo_path = "server_fifo";  // Percorso della FIFO

    // Richiede all'utente di inserire il proprio nome
    printf("Inserisci il tuo nome: ");
    scanf("%s", nome);
    printf("Ciao %s! Scrivi messaggi con @utente o #canale.\n", nome);

    while (1) {
        // Controlla se ci sono messaggi ricevuti
        FILE *miei_messaggi = fopen(nome, "r");
        if (miei_messaggi != NULL) {
            char riga[1024];
            // Legge tutti i messaggi dal file
            while (fgets(riga, sizeof(riga), miei_messaggi)) {
                printf("\nHai ricevuto: %s", riga);
            }
            fclose(miei_messaggi);
            remove(nome);  // Elimina il file dopo la lettura
        }

        // Chiede all'utente se vuole inviare un messaggio
        printf("\nVuoi scrivere un messaggio? (s/n): ");
        char scelta;
        scanf(" %c", &scelta);

        if (scelta == 's' || scelta == 'S') {
            // Chiede destinatario e messaggio
            printf("A chi vuoi scrivere? (@utente o #canale): ");
            scanf("%s", destinatario);

            printf("Scrivi il messaggio: ");
            scanf(" %[^\n]", messaggio);

            // Apre la FIFO in scrittura e invia il messaggio
            int fd = open(fifo_path, O_WRONLY);
            if (fd != -1) {
                // Formatta il messaggio nel buffer
                snprintf(buffer, sizeof(buffer), "%s %s %s", nome, destinatario, messaggio);
                write(fd, buffer, strlen(buffer));
                close(fd);
                printf("Messaggio inviato!\n");
            } else {
                perror("Errore apertura FIFO");
            }
        }

        sleep(1);  // Pausa per ridurre il carico CPU
    }

    return 0;
}
