#include "header.h"

void init() {
	int i;

	init_signal_handling();

	create_shm();
	#ifdef DEBUG
	printf("triage_total: %d\nappointment_total:%d\n", shared_var->stats->triage_total, shared_var->stats->appointment_total);
	#endif

	shared_var->config = (Config*)malloc(sizeof(Config));
	readConfig();
	#ifdef DEBUG
	printf("n_triage: %d\nn_doctors: %d\nshift_dur: %d\nmq_max: %d\n", shared_var->config->n_triage, shared_var->config->n_doctors, shared_var->config->shift_dur, shared_var->config->mq_max);
	#endif

	threadIds = (int*)malloc(shared_var->config->n_triage * sizeof(int)); //aloca memoria para o numero de triagens em struct config
	threads = (pthread_t*)malloc(shared_var->config->n_triage * sizeof(pthread_t));
	for (i=0; i<shared_var->config->n_triage; i++)
		create_thread(i);

	processes = (pid_t*)malloc(shared_var->config->n_doctors*sizeof(pid_t));

	sleep(15);

}