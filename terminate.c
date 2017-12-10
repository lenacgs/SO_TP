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

	pthread_kill(listening_thread, SIGKILL);

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
	else perror("sem_unlink error");
	if (sem_unlink(SEM_PATIENTS_QUEUE) == 0)  printf("SEM_PATIENTS_QUEUE unlinked\n");
	else perror("sem_unlink error: ");


	if (shmctl(shmid, IPC_RMID, NULL) == 0) printf("Shared memory detached\n");
	else perror("shmctl error");

	if (msgctl(mq_id, IPC_RMID, 0) == 0)
		printf("Message queue was deleted.\n");
	else perror("Message queue could not be deleted"); 

	//close(pipe_fd);

	if (unlink(PIPE) == 0) printf("Named pipe was unlinked\n");
	else perror("Named pipe could not be unlinked");

	#ifdef DEBUG
	printf("All threads joined\n");
	printf("All resources have been terminated\n");
	#endif

	exit(0);
}