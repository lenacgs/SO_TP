#include "header.h"
//processo principal
int main() {
	int i, n_procs=0;
	
	//creating shared memory & print stats
	createShm();
	#ifdef DEBUG
	printf("triage_total: %d\nappointment_total:%d\n", shared_var->stats->triage_total, shared_var->stats->appointment_total);
	#endif

	//reading file config.txt and writing it to shared memory
	shared_var->config = (Config*)malloc(sizeof(Config));
	readConfig();
	
	#ifdef DEBUG
	printf("n_triage: %d\nn_doctors: %d\nshift_dur: %d\nmq_max: %d\n", shared_var->config->n_triage, shared_var->config->n_doctors, shared_var->config->shift_dur, shared_var->config->mq_max);
	#endif


	//creating triage threads
	triageIds = (int*)malloc(shared_var->config->n_triage * sizeof(int)); //aloca memoria para o numero de triagens em struct config
	my_thread = (pthread_t*)malloc(shared_var->config->n_triage * sizeof(pthread_t));
	for (i=0; i<shared_var->config->n_triage; i++)
		createTriage(i);

	//sychronizing creation of doctor processes

	//reading shared memory & printint stats

	//destroying resources when signaled
	shmdt(shared_var);
	wait(NULL);
	for (i=0; i<shared_var->config->n_triage; i++) {
		pthread_join(my_thread[i], NULL);
		printf("Thread joined.\n");
	}
	return(0);

}