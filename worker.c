#include "header.h"

void *triageGoToWork(void * triage_id) {
	int id = *((int*)triage_id);
	//Patient patient;
	//e preciso fazer um cast para inteiro do id, caso queiramos que o ponteiro seja inteiro
	//get patient from queue
	//patient = getPatient();
	//registar paciente no log

	//atribui ao paciente uma prioridade na MQ
	//falta passar como argumento o parametro arrival_time do paciente, para ser escrito nas estatisticas
	writeStatsTriage(clock());
	return(NULL);
}

void doctor_worker() {
	clock_t start = clock();
	clock_t end = clock();
	int shift_time = (end - start) / CLOCKS_PER_SEC;
	while (/*getPatientQ() && */((int)shift_time < shared_var->config->shift_dur)){
		end = clock();
		shift_time = (end - start) / CLOCKS_PER_SEC;
	}
	writeStatsDocs(start, clock());
	waitpid(getpid(), NULL, WNOHANG);
}