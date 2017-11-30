#include "header.h"

void terminate() {
	int i;

	#ifdef PRINT_STATS
	sem_wait(shared_var->semaphores->mutex);
	printf("triage_total: %d\nappointment_total: %d\n", shared_var->stats->triage_total, shared_var->stats->appointment_total);
	printf("enter queue times:\n");
	for (i=0; i<shared_var->stats->appointment_total; i++)
		printf("%lu", shared_var->stats->time_queue[i]);
	printf("enter message queue times:\n");
	for (i=0; i<shared_var->stats->appointment_total; i++)
		printf("%lu", shared_var->stats->time_mqueue[i]);
	printf("enter appointment times:\n");
	for (i=0; i<shared_var->stats->appointment_total; i++)
		printf("%lu", shared_var->stats->time_beg_app[i]);
	printf("leave appointment times:\n");
	for (i=0; i<shared_var->stats->appointment_total; i++)
		printf("%lu", shared_var->stats->time_end_app[i]);
	sem_post(shared_var->semaphores->mutex);
	#endif

	#ifdef DEBUG
	printf("Waiting for all doctors to finish work...\n");
	#endif
	wait(NULL);

	for (i=0; i<shared_var->config->n_doctors; i++)
		if (kill(shared_var->semaphores->processes[i], SIGKILL) != 0) perror("kill error: ");

	printf("All doctors went home!\n");

	for (i=0; i<shared_var->config->n_triage; i++)
		if (threads[i] != (pthread_t)NULL) pthread_join(threads[i], NULL);

	if (sem_unlink(SEM_PROCESSES) == 0)  printf("SEM_PROCESSES unlinked\n");
	else perror("sem_unlink error: ");
	if (sem_unlink(MUTEX) == 0) printf("MUTEX unlinked\n");
	else perror("sem_unlink error:");


	if (shmctl(shmid, IPC_RMID, NULL) == 0) printf("Shared memory detached\n");
	else perror("shmctl error: ");

	if (msgctl(mq_id, IPC_RMID, NULL) == -1) {
		fprintf(stderr, "Message queue could not be deleted.\n");
		exit(EXIT_FAILURE);
	}

	printf("Message queue was deleted.\n");

	close(pipe_fd);

	if (unlink(PIPE) == 0) fprintf(stderr, "Named pipe was unlinked.\n");
	else fprintf(stderr, "Named pipe could not me unlinked.\n");

	#ifdef DEBUG
	printf("All threads joined\n");
	printf("All resources have been terminated\n");
	#endif

	exit(0);
}