#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>


#define QUEUE_NAME	"/test_queue"


int main() {
    // Erzeuge die Message Queue zum Schreiben
    // Zum Erzeugen ist es besser, die mq_open() Methode mit den 4 Eingabeargumenten zu verwenden
    // Manche Systeme mÃ¶gen es nicht, wenn man keine Permissions angibt.
    mqd_t queue = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0666, NULL);
    if (queue == -1) {
        perror("Could not create message queue");
        exit(1);
    }

    // Die Nachricht
    char* message = "Hallo";

    // Sende die Nachricht mit Prioritaet 3
    int status = mq_send(queue, message, 6 /*5 Zeichen + Nullterminierungssymbol*/, 3);
    if (status == -1) {
        perror("Could not send message");
        exit(1);
    }

    // Schliesse die Message Queue
    status = mq_close(queue);
    if (status == -1) {
        perror("Could not close message queue");
        exit(1);
    }
}
