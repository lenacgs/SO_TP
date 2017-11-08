#include "header.h"

void terminate() {
	int i;

	for (i=0; i<shared_var->config->n_doctors; i++) {
		if (processes[i] != (pid_t)NULL) {
			kill (processes[i], SIGKILL);
		}
	}
	free(processes);

	printf("\nAll child processes killed.");

	sem_unlink(SEM_PROCESSES);
	printf("\nSEM_PROCESSES unlinked.");

	shmctl(shmid, IPC_RMID, NULL);
	printf("\nShared memory detached.");

	for (i=0; i<shared_var->config->n_triage; i++) {
		if (threads[i] != (pthread_t)NULL) {
			pthread_join(threads[i], NULL);
		}
	}

	printf("\nAll threads joined.");
	//remove semaphores

	printf("\nAll resources have been terminated.");
	exit(0);
}