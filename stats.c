#include "header.h"

void write_stats_docs(clock_t start, clock_t end) {
	int i = shared_var->stats->appointment_total;
	shared_var->stats->appointment_total++;
	shared_var->stats->time_beg_app[i] = start;
	shared_var->stats->time_end_app[i] = end;
}

void write_stats_triage(clock_t end) {
	int i;

	i = shared_var->stats->triage_total;

	shared_var->stats->time_mqueue[i] = end;

	shared_var->stats->triage_total++;
}

void print_stats() {
	//int m1, m2, m3;
	printf("\nTriage total: %d", shared_var->stats->triage_total);
	printf("\nAppointment total: %d", shared_var->stats->triage_total);

	// m1 = calc_moment1(); //tempo medio de espera antes do inicio da triagem
	// m2 = calc_moment2(); //tempo medio de espera entre o fim da triagem e o inicio do atendimento
	// m3 = calc_moment3(); //media do tempo total que cada user gasta do inicio ao fim
}