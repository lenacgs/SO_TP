#include "header.h"

void * thread_worker(void * triage_id) {
	//falta passar como argumento o parametro arrival_time do paciente, para ser escrito nas estatisticas
	write_stats_triage(clock());
	return(NULL);
}

void doctor_worker() {

	clock_t start = clock();
	clock_t end = clock();
	clock_t shift_time = end - start;

	#ifdef DEBUG
	printf("Doctor (PID %d) started working\n", getpid());
	#endif

	do {
		getPatientQ();
		end = clock();
		shift_time = end - start;
		write_stats_docs(start, end);
	} while (shift_time < (clock_t)shared_var->config->shift_dur);

	
	#ifdef DEBUG
	printf("Doctor (PID %d) stopped working\n", getpid());
	#endif
}