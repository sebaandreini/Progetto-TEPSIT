#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    // Buffer per memorizzare i dati ricevuti
    char mittente[100];       // Chi invia il messaggio
    char destinatario[100];   // A chi è destinato (@utente o #canale)
    char messaggio[1024];     // Il contenuto del messaggio
    char buffer[2048];        // Buffer per la lettura dalla FIFO

    const char *fifo_path = "server_fifo";  // Percorso della FIFO

    printf("Server in ascolto...\n");

    while (1) {
        // Apre la FIFO in modalità lettura (bloccante)
        int fd = open(fifo_path, O_RDONLY);
        if (fd != -1) {
            // Legge i dati dalla FIFO
            int bytes_read = read(fd, buffer, sizeof(buffer));
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';  // Termina la stringa
                
                // Estrae mittente, destinatario e messaggio
                sscanf(buffer, "%s %s %[^\n]", mittente, destinatario, messaggio);

                // Gestione messaggi per canali (inizio con #)
                if (destinatario[0] == '#') {
                    // Apre il file del canale in append
                    FILE *canale = fopen(destinatario + 1, "a");
                    if (canale != NULL) {
                        // Scrive il messaggio nel file del canale
                        fprintf(canale, "[%s -> %s]: %s\n", mittente, destinatario, messaggio);
                        fclose(canale);
                    }
                    // Stampa anche sul server per monitoraggio
                    printf("[%s -> %s]: %s\n", mittente, destinatario, messaggio);
                } 
                // Gestione messaggi privati (inizio con @)
                else if (destinatario[0] == '@') {
                    // Apre il file dell'utente destinatario in append
                    FILE *utente = fopen(destinatario + 1, "a");
                    if (utente != NULL) {
                        // Scrive il messaggio privato
                        fprintf(utente, "[%s]: %s\n", mittente, messaggio);
                        fclose(utente);
                    }
                }
            }
            close(fd);  // Chiude la FIFO
        } else {
            perror("Impossibile aprire la FIFO");
        }

        sleep(1);  // Pausa per ridurre il carico CPU
    }

    return 0;
}
