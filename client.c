#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_NAME_Send "/printqueueSend"
#define QUEUE_NAME_Receive "/printqueueReceive"




typedef struct Content{
    const char* name;
    char nachricht[30];
}Content;

int main(int argc,  char* argv[]){
    if (argc < 3)
    {
        perror("ERROR! Nicht genügend KommandozeilenArgumente!");
        return 1;
    }
    mqd_t queue = mq_open(QUEUE_NAME_Send,  O_CREAT | O_WRONLY, 0666, 0 );
    if (queue == -1) {
        perror("Message Queue konnte nicht geöffnet werden");
        exit(1);
    }
    Content argument;
    argument.name = argv[1];
    char *p;
    int prio = (int)strtol(argv[2], &p, 10);
    strcpy(argument.nachricht, "Drucke Datei Löwe.jpg auf A4");


    printf("name: %s\n", argv[1]);
    printf("name: %s\n", argument.name);
    printf("name: %s\n", argument.nachricht);


    //Nachricht senden
    int length = (int)strlen(argument.nachricht);
    printf("length = %d", length);
    sleep(5);
    int status = mq_send(queue, argument.nachricht, length+1, prio);
    if (status == -1) {
        perror("Message konnte nicht gesendet werden");
        exit(1);
    }
    status = mq_close(queue);
    if (status == -1) {
        perror("Could not close message queue");
        exit(1);
    }
    return 0;
}
