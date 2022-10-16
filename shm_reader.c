#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>


#define SHM_NAME	"/shm_testsegment"
#define SHM_SIZE	sizeof(shm_data_t)


// Struct representing the memory layout of the shared-memory segment
typedef struct {
	char buffer[32];
} shm_data_t;


int main() {
	printf("My PID is %d\n", getpid());
	printf("shm_size = %lu\n", SHM_SIZE);

	// Open the shared memory object
	// No need to configure the size of an existing shared-memory segment
	int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
	if (shm_fd == -1) {
		perror("Could not create shared memory segment");
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

	// Read and print data from shared-memory segment
	printf("shm-data: \"%s\"\n", shm_data->buffer);

	// Unmap the shared memory segment from memory
	int status = munmap(shm_ptr, SHM_SIZE);
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


