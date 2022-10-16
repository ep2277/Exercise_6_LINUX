#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<string.h>

#define QUEUE_NAME_Client "/printqueueSend"
#define QUEUE_NAME_Receive "/printqueueReceive"


int main() {

    mqd_t queue = mq_open(QUEUE_NAME_Client, O_RDONLY);
    if (queue == -1) {
        perror("Message Queue konnte nicht geöffnet werden");
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
    printf("29");
    // Buffer zum Speichern der Nachricht
    char buffer[queue_attr.mq_msgsize];
    // Hier soll die NachrichtenprioritÃ¤t hineingeschrieben werden
    unsigned priority = 0;

    // Empfange die Nachricht
 /*   ssize_t msgsize = mq_receive(queue, buffer, queue_attr.mq_msgsize, &priority);
    if (msgsize == -1) {
        perror("Could not receive message");
        exit(1);
    }

    // Gebe die Nachricht aus
    printf("Druckauftrag \"%s\" wird bearbeitet" , buffer);

    // Schliesse die Message Queue
    status = mq_close(queue);
    if (status == -1) {
        perror("Could not close message queue");
        exit(1);
    }
    //Für die Bearbeitung jedes Auftrags soll der Server eine Zeit zwischen 1 und 10 Sekunden
    // (zufällig) benötigen.
    int t = rand() % 10;
    sleep(t);
    //Es soll allerdings eine Chance von 10 % bestehen, dass ein Fehler geworfen wird, in welchem Fall
    // Fehler bei der Bearbeitung des Druckauftrags <name>!\n ausgegeben werden.
    int chance = rand() % 10;
    const char * nachricht;
    if (chance == 5){
        nachricht = "Fehler bei der Bearbeitung des Druckauftrags %s!\\n", buffer;
    }
    else
    {
        nachricht = "Bearbeitung Druckauftrag %s abgeschlossen!\\n", buffer;
    }
    //In jedem Fall soll aber der entsprechende Text über die Mailbox des Clients gesendet
    // und dort ausgegeben werden.
   /*

    priority = 9;
    status = mq_send(queue, nachricht, strlen(nachricht), priority);
    if (status == -1) {
        perror("Message konnte nicht gesendet werden");
        exit(1);
    }
    status = mq_close(queue);
    if (status == -1) {
        perror("Could not close message queue");
        exit(1);
    }
*/
printf("Ende");

}