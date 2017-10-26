#include "header.h"

void createDoctorProcs() {
	if (fork() == 0) {
		printf("\nDoctor process with PID:%d created.", getpid());
		doctorGoToWork();
		printf("\nDoctor process with PID: %d --- Shift finished.\nDoctor process leaving work.", getpid());
		exit(0);
		// quando um doutor morre, outro tem que ser criado

	}
}