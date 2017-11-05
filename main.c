#include "header.h"
//processo principal
int main() {
	int i;
	
	//read file config.txt
	config = (Config*)malloc(sizeof(Config));
	readConfig();
	
	#ifdef DEBUG
	printf("n_triage: %d\nn_doctors: %d\nshift_dur: %d\nmq_max: %d\n", config->n_triage, config->n_doctors, config->shift_dur, config->mq_max);
	#endif

	//create shared memory & print stats
	createShm();
	#ifdef DEBUG
	printf("triage_total: %d\nappointment_total:%d\n", shared_var->triage_total, shared_var->appointment_total);
	#endif
	
	//create Doctor processes
	int n_procs = 0;
	while (1) {
		if (n_procs<config->n_doctors) {
			createDoctorProcs();
			n_procs++;
		}
	}
	wait(NULL);

	//create triage threads
	triageIds = (int*)malloc(config->n_triage * sizeof(int)); //aloca memoria para o numero de triagens em struct config
	my_thread = (pthread_t*)malloc(config->n_triage * sizeof(pthread_t));
	for (i=0; i<config->n_triage; i++)
		createTriage(i);

	//read shared memory and print stats
	printf("End of program");
	return 0;

}