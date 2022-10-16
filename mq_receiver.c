#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>


#define QUEUE_NAME	"/test_queue"


int main() {
    // Oeffne die Message Queue zum Lesen
    mqd_t queue = mq_open(QUEUE_NAME, O_RDONLY);
    if (queue == -1) {
        perror("Could not open message queue");
        exit(1);
    }

    // Hole die maximale Nachrichtengroesse
    // Wir können uns nicht immer darauf verlassen, dass die maximale Nachrichtengroesse
    // standardmaessig 8192 Bytes ist.
    // Daher ist es besser, sich die tatsaechliche maximale Nachrichtengroesse zu holen.
    struct mq_attr queue_attr;
    int status = mq_getattr(queue, &queue_attr);
    if (status == -1) {
        perror("Could not get message queue attributes");
        exit(1);
    }

    // Buffer zum Speichern der Nachricht
    char buffer[queue_attr.mq_msgsize];
    // Hier soll die NachrichtenprioritÃ¤t hineingeschrieben werden
    unsigned priority = 0;

    // Empfange die Nachricht
    ssize_t msgsize = mq_receive(queue, buffer, queue_attr.mq_msgsize, &priority);
    if (msgsize == -1) {
        perror("Could not receive message");
        exit(1);
    }

    // Gebe die Nachricht aus
    printf("Received message \"%s\" with priority %d\n", buffer, priority);

    // SchlieÃŸe die Message Queue
    status = mq_close(queue);
    if (status == -1) {
        perror("Could not close message queue");
        exit(1);
    }

    // LÃ¶sche die Message Queue
    status = mq_unlink(QUEUE_NAME);
    if (status == -1) {
        perror("Could not unlink message queue");
        exit(1);
    }
}

