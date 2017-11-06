#include "header.h"

void createDoctorProcs() {
	if (fork() == 0) {
		printf("\nDoctor process with PID:%d created.\n", getpid());
		doctorGoToWork();
		printf("\nDoctor process with PID: %d --- Shift finished.\nDoctor process leaving work.\n", getpid());
		kill(getpid(), SIGKILL);
	}
	else {
		perror("Error creating new child process: ");
		return;
	}
}