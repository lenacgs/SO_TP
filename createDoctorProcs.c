#include "header.h"

void createDoctorProcs() {
	if (fork() == 0) {
		printf("Doctor process with PID:%d created.\n", getpid());
		//DoctorgoToWork();
		printf("Doctor process with PID: %d --- Shift finished.\nDoctor process leaving work.\n", getpid());
		exit(0);
	}
}