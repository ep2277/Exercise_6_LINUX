#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>


#define SHM_NAME	"/shm_counter"
#define SHM_SIZE	sizeof(unsigned)


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
	unsigned* counter = (unsigned*) shm_ptr;

	// Initialize the counter
	*counter = 0;

	// Create 10 child processes which add one to the counter 10000-times each
	pid_t children[10];
	for (int i = 0; i < 10; i++) {
		children[i] = fork();
		if (children[i] == -1) {
			perror("Error while forking");
			exit(1);

		// child-process
		} else if (children[i] == 0) {
			for (int j = 0; j < 10000; j++) {
				// critical region start
				*counter += 1;
				// critical region end
			}
			exit(0);
		}
	}

	// Wait for children
	for (int i = 0; i < 10; i++) {
		waitpid(children[i], NULL, 0);
	}

	// Print counter value
	printf("counter = %u\n", *counter);

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

