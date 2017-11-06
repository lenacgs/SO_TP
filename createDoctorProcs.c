#include "header.h"

void createDoctorProcs() {
	if (fork() == 0) {
		printf("\nDoctor process with PID:%d created.\n", getpid());
		doctorGoToWork();
		printf("\nDoctor process with PID: %d --- Shift finished.\nDoctor process leaving work.\n", getpid());
		sem_post(&sem);
		kill(getpid(), SIGKILL);
	}
}