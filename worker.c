#include "header.h"

void * thread_worker(void * triage_id) {
	int id = *((int*)triage_id);
	//falta passar como argumento o parametro arrival_time do paciente, para ser escrito nas estatisticas
	write_stats_triage(clock());
	return(NULL);
}

void doctor_worker() {
	clock_t start = clock();
	clock_t end = clock();
	int shift_time = (end - start) / CLOCKS_PER_SEC;
	#ifdef DEBUG
	printf("\nDoctor: %d in", getpid());
	#endif
	while (/*getPatientQ() && */((int)shift_time < shared_var->config->shift_dur)){
		end = clock();
		shift_time = (end - start) / CLOCKS_PER_SEC;
	}
	#ifdef DEBUG
	printf("\nDoctor: %d out", getpid());
	#endif
	write_stats_docs(start, clock());
}