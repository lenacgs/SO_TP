#include "header.h"

void * thread_worker(void * triage_id) {
	queue_node * node;
	while (1) {
		sem_wait(sem_queue);
		node = get_node();
		if (node != NULL) {
			printf("Verification: got patient from triage queue. These are its specifications\n");
			printf("name: %s arrival time: %lu triage dur: %d appt dur: %d priority: %d\n", node->patient->name, node->patient->arrival_time, node->patient->triage, node->patient->appointment, node->patient->priority);
		}

		printf("Patient %s is going through triage...\n", node->patient->name);
	}


	write_stats_triage(clock());
	return(NULL);
}

//pops node from queue
queue_node * get_node() {
	printf("getting node from triage queue\n");
	queue_node * node;

	//gets node from the begginig of the list. theres a header node
	if (triage_queue == NULL) printf("triage queue is null\n");
	if (triage_queue->next == NULL) printf("traige queue next is null\n");
	printf("triage queue->next->patient->name: %s\n", triage_queue->next->patient->name);

	node = triage_queue->next; //header_node->next
	if (node->patient == NULL) printf("nde->patient is null\n");
	printf("this is the first node! name: %s\n", node->patient->name);
	//taking the patient off the queue 
	triage_queue->next = triage_queue->next->next;

	return node;
}

void doctor_worker() {

	clock_t start = clock();
	clock_t end = clock();
	int shift_time = (end - start) / CLOCKS_PER_SEC;

	#ifdef DEBUG
	printf("Doctor (PID %d) started working\n", getpid());
	#endif

	while (/*getPatientQ() && */((int)shift_time < shared_var->config->shift_dur)){
		end = clock();
		shift_time = (end - start) / CLOCKS_PER_SEC;
	}
	write_stats_docs(start, clock());
	#ifdef DEBUG
	printf("Doctor (PID %d) stopped working\n", getpid());
	#endif
}