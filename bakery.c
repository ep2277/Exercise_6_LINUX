
#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define STORAGE_MAX_COUNT 30
#define STORAGE_SUPPLIER_COUNT 20


typedef struct {
    int count;
    pthread_mutex_t mutex;
} storage_t;

pthread_cond_t supplier_cond;
pthread_mutex_t supplier_mutex;

pthread_cond_t baking_cond;
pthread_mutex_t baking_mutex;

storage_t storage;


int shutdownFlag = 0;

int supplierInformed = 0;

void* bakingBread(void* arg) {
    while(!shutdownFlag) {
        // Brot backen
        pthread_mutex_lock(&storage.mutex);
        if (storage.count < STORAGE_MAX_COUNT) {
            printf("Lagerstand wird erhöht\n");
            storage.count += 1;
            if (storage.count >= STORAGE_SUPPLIER_COUNT) {
                // Lieferant benachrichtigen
                if (supplierInformed == 0) {
                    printf("Lieferant wird informiert\n");
                    pthread_mutex_lock(&supplier_mutex);
                    pthread_cond_signal(&supplier_cond);
                    pthread_mutex_unlock(&supplier_mutex);
                    supplierInformed = 1;
                }
            }
            pthread_mutex_unlock(&storage.mutex);
            sleep(1);
        } else {
            pthread_mutex_unlock(&storage.mutex);
            // Warten bis Platz im Lager
            printf("Warte bis Platz im Lager\n");
            pthread_mutex_lock(&baking_mutex);
            pthread_cond_wait(&baking_cond, &baking_mutex);
            pthread_mutex_unlock(&baking_mutex);
            printf("Warte bis Platz im Lager: Aufgeweckt\n");
        }
    }
    printf("Backstation beendet sich\n");
}


void* deliverBread(void* arg) {
    while(!shutdownFlag) {
        // Schlafen und warten auf signal
        printf("Lieferant wartet auf Anruf\n");
        pthread_mutex_lock(&supplier_mutex);
        pthread_cond_wait(&supplier_cond, &supplier_mutex);
        pthread_mutex_unlock(&supplier_mutex);
        printf("Lieferant hat Anruf bekommen\n");
        if (!shutdownFlag) {
            // Zeitlang warten (Fahrtzeit zum Lager)
            sleep(4);
            // lagerstand auf 0 setzen
        printf("Lieferant hat lager geleert\n");
            pthread_mutex_lock(&storage.mutex);
            storage.count = 0;
            pthread_mutex_unlock(&storage.mutex);
            supplierInformed = 0;
            // Backstationen aufwecken
            pthread_mutex_lock(&baking_mutex);
            pthread_cond_broadcast(&baking_cond);
            pthread_mutex_unlock(&baking_mutex);
        }
    }
    printf("Lieferant beendet sich\n");
}


void qualityCheck(int signum) {
    printf("qualityCheck()\n");
    int count = 4;
    pthread_mutex_lock(&storage.mutex);
    if (storage.count < count) {
        count = storage.count;
    }
    storage.count -= count;
    pthread_mutex_unlock(&storage.mutex);
    // Backstationen aufwecken
    pthread_mutex_lock(&baking_mutex);
    pthread_cond_broadcast(&baking_cond);
    pthread_mutex_unlock(&baking_mutex);
    alarm(5);
}


void graceful_shutdown(int signum) {
    printf("Graceful shutdown\n");
    shutdownFlag = 1;
    // Backstationen aufwecken
    pthread_mutex_lock(&baking_mutex);
    pthread_cond_broadcast(&baking_cond);
    pthread_mutex_unlock(&baking_mutex);
    // Lieferant benachrichtigen
    pthread_mutex_lock(&supplier_mutex);
    pthread_cond_broadcast(&supplier_cond);
    pthread_mutex_unlock(&supplier_mutex);
}

int main() {
    
    storage.count = 0;
    
    pthread_mutex_init(&storage.mutex, NULL);
    pthread_mutex_init(&supplier_mutex, NULL);
    pthread_mutex_init(&baking_mutex, NULL);
    pthread_cond_init(&supplier_cond, NULL);
    pthread_cond_init(&baking_cond, NULL);
    
    // Registriere Signal Handler für ALARM Signal
    struct sigaction signal;
    signal.sa_handler = qualityCheck;
    sigemptyset(&signal.sa_mask);
    sigaction(SIGALRM, &signal, NULL);

    // Register signal handlers
	signal.sa_handler = graceful_shutdown;
	sigemptyset(&signal.sa_mask);
	sigaddset(&signal.sa_mask, SIGTERM);
	sigaddset(&signal.sa_mask, SIGINT);
	sigaddset(&signal.sa_mask, SIGQUIT);
	signal.sa_flags = SA_RESTART;

	if (sigaction(SIGTERM, &signal, NULL) != 0) {
		perror("Error while registering signal handler");
		exit(1);
	}
	if (sigaction(SIGINT, &signal, NULL) != 0) {
		perror("Error while registering signal handler");
		exit(1);
	}
	if (sigaction(SIGQUIT, &signal, NULL) != 0) {
		perror("Error while registering signal handler");
		exit(1);
	}

    
    pthread_t bakingThread[6];
    for (int i = 0; i < 6; i++) {
        pthread_create(&bakingThread[i], NULL, bakingBread, NULL);
    }
    
    pthread_t supplierThread;
    pthread_create(&supplierThread, NULL, deliverBread, NULL);
    
    alarm(5);
    
    for (int i = 0; i < 6; i++) {
        pthread_join(bakingThread[i], NULL);
    }
    pthread_join(supplierThread, NULL);
    
    pthread_mutex_destroy(&storage.mutex);
    pthread_mutex_destroy(&supplier_mutex);
    pthread_mutex_destroy(&baking_mutex);
    pthread_cond_destroy(&supplier_cond);
    pthread_cond_destroy(&baking_cond);
}
