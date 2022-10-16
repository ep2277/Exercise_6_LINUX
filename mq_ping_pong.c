#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>


#define QUEUE1_NAME	"/ping_queue"
#define QUEUE2_NAME	"/pong_queue"


int main() {

    pid_t cpid = fork();
    if (cpid == -1) {
        perror("Error while forking");
        exit(1);

        // Kindprozess
    } else if (cpid == 0) {
        // Oeffne die Ping Queue zum Lesen
        mqd_t queue1 = mq_open(QUEUE1_NAME, O_CREAT | O_RDONLY, 0666, NULL);
        if (queue1 == -1) {
            perror("Child: Could not open ping queue");
            exit(1);
        }
        // Oeffne die Pong Queue zum Schreiben
        mqd_t queue2 = mq_open(QUEUE2_NAME, O_CREAT | O_WRONLY, 0666, NULL);
        if (queue2 == -1) {
            perror("Child: Could not open pong queue");
            exit(1);
        }
        // Hole die maximale NachrichtengrÃ¶ÃŸe der Ping Queue
        struct mq_attr queue1_attr;
        int status = mq_getattr(queue1, &queue1_attr);
        if (status == -1) {
            perror("Child: Could not get pong queue attributes");
            exit(1);
        }
        // Spiele 10-mal Ping-Pong
        char buffer[queue1_attr.mq_msgsize]; // Buffer zum Speichern der Nachricht
        unsigned priority = 0; // Hier soll die NachrichtenprioritÃ¤t hineingeschrieben werden
        for (int i = 0; i < 10; i++) {
            // Lese Ping
            ssize_t msgsize = mq_receive(queue1, buffer, queue1_attr.mq_msgsize, &priority);
            if (msgsize == -1) {
                perror("Child: Could not receive ping");
                exit(1);
            }
            printf("Child: Received \"%s\" from parent\n", buffer);
            // Sende Pong
            int status = mq_send(queue2, "PONG", 5 /*4 Zeichen + Nullterminierungssymbol*/, 0);
            if (status == -1) {
                perror("Child: Could not send pong");
                exit(1);
            }
        }

        // SchlieÃŸe Queues
        status = mq_close(queue1);
        if (status == -1) {
            perror("Child: Could not close ping queue");
            exit(1);
        }
        status = mq_close(queue2);
        if (status == -1) {
            perror("Child: Could not close pong queue");
            exit(1);
        }


        // Elternprozess
    } else {
        // Ã–ffne die Ping Queue zum Schreiben
        mqd_t queue1 = mq_open(QUEUE1_NAME, O_CREAT | O_WRONLY, 0666, NULL);
        if (queue1 == -1) {
            perror("Parent: Could not open ping queue");
            exit(1);
        }
        // Ã–ffne die Pong Queue zum Lesen
        mqd_t queue2 = mq_open(QUEUE2_NAME, O_CREAT | O_RDONLY, 0666, NULL);
        if (queue2 == -1) {
            perror("Parent: Could not open pong queue");
            exit(1);
        }
        // Hole die maximale NachrichtengrÃ¶ÃŸe der Pong Queue
        struct mq_attr queue2_attr;
        int status = mq_getattr(queue2, &queue2_attr);
        if (status == -1) {
            perror("Parent: Could not get pong queue attributes");
            exit(1);
        }
        // Spiele 10-mal Ping-Pong
        char buffer[queue2_attr.mq_msgsize]; // Buffer zum Speichern der Nachricht
        unsigned priority = 0; // Hier soll die NachrichtenprioritÃ¤t hineingeschrieben werden
        for (int i = 0; i < 10; i++) {
            // Sende Ping
            int status = mq_send(queue1, "PING", 5 /*4 Zeichen + Nullterminierungssymbol*/, 0);
            if (status == -1) {
                perror("Parent: Could not send ping");
                exit(1);
            }
            // Lese Pong
            ssize_t msgsize = mq_receive(queue2, buffer, queue2_attr.mq_msgsize, &priority);
            if (msgsize == -1) {
                perror("Parent: Could not receive pong");
                exit(1);
            }
            printf("Parent: Received \"%s\" from child\n", buffer);
        }

        // SchlieÃŸe Queues
        status = mq_close(queue1);
        if (status == -1) {
            perror("Parent: Could not close ping queue");
            exit(1);
        }
        status = mq_close(queue2);
        if (status == -1) {
            perror("Parent: Could not close pong queue");
            exit(1);
        }

        // Warte auf Kind
        waitpid(cpid, NULL, 0);

        // LÃ¶sche die Message Queues
        status = mq_unlink(QUEUE1_NAME);
        if (status == -1) {
            perror("Could not unlink ping queue");
            exit(1);
        }
        status = mq_unlink(QUEUE2_NAME);
        if (status == -1) {
            perror("Could not unlink pong queue");
            exit(1);
        }

    }
}
