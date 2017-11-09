#include "header.h"

void terminate() {
	int i;
	#ifdef DEBUG
	printf("\nWaiting for all doctors to finish work...");
	#endif
	wait(NULL);
	free(shared_var->semaphores->processes);

	printf("\nAll doctors went home!");

	sem_unlink(SEM_PROCESSES);
	printf("\nSEM_PROCESSES unlinked.");

	shmctl(shmid, IPC_RMID, NULL);
	#ifdef DEBEUG
	printf("\nShared memory detached.");
	#endif

	for (i=0; i<shared_var->config->n_triage; i++) {
		if (threads[i] != (pthread_t)NULL) {
			pthread_join(threads[i], NULL);
		}
	}
	#ifdef DEBUG
	printf("\nAll threads joined.");

	printf("\nAll resources have been terminated.");
	#endif
	exit(0);
}