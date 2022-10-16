#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>


#define SHM_NAME	"/bankkonten"
#define SHM_SIZE	sizeof(shm_data_t)


// Struct containing the counter and a mutex
// Represents the memory layout of the shared-memory segment
typedef struct {
    int konto[12];
    pthread_mutex_t mutex;
} shm_data_t;


int main() {
    printf("My PID is %d\n", getpid());
    printf("shm_size = %lu\n", SHM_SIZE);

    // Create the shared memory object
    // We create the shared memory segment once in the parent process
    // All child processes then inherit the shared memory segment
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Could not create shared memory segment");
        exit(1);
    }

    void clientAccessAcount(int geldsumme, int konto);
    int serverReadAccounts();
    // Configure the size of the shared memory segment
    int status = ftruncate(shm_fd, SHM_SIZE);
    if (status == -1) {
        perror("Could not configure the size of the shared memory segment");
        exit(1);
    }

    // Map the shared memory segment into memory
    void* shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Could not map the shared memory segment into memory");
        exit(1);
    }

    // Cast shared memory pointer to whatever data type we need it to be
    // The programmer needs to manually manage the shared-memory segment
    // Best practice is to use a struct that represents the memory layout
    shm_data_t* shm_data = (shm_data_t*) shm_ptr;

    // Initialize the counter ???
    ///shm_data->konto = 0;

    // Initialize mutex attributes
    // The process-shared attribute needs to be set when the mutex is used among several processes
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    // Initialize mutex
    // The mutex needs to reside in the shared-memory segment so that all processes can access it
    pthread_mutex_init(&shm_data->mutex, &attr);

    // Create 10 child processes which add one to the counter 10000-times each
    pid_t children[10];
    for (int i = 0; i < 10; i++) {
        children[i] = fork();
        if (children[i] == -1) {
            perror("Error while forking");
            exit(1);

            // child-process
        } else if (children[i] == 0) {
            for (int j = 0; j < 11; j++) {
                pthread_mutex_lock(&shm_data->mutex);
                // critical region start
                shm_data-> konto[j] = 1 ;
                // critical region end
                pthread_mutex_unlock(&shm_data->mutex);
            }
            exit(0);
        }
    }

    // Wait for children
    for (int i = 0; i < 10; i++) {
        waitpid(children[i], NULL, 0);
    }

    // Print counter value
    printf("counter = %u\n", shm_data->konto[0]);

    // Delete mutex
    pthread_mutex_destroy(&shm_data->mutex);

    // Unmap the shared memory segment from memory
    status = munmap(shm_ptr, SHM_SIZE);
    if (status == -1) {
        perror("Could not unmap the shared memory segment from memory");
        exit(1);
    }

    // Delete the shared memory object
    status = shm_unlink(SHM_NAME);
    if (status == -1) {
        perror("Could not delete the shared memory segment");
        exit(1);
    }

}

