#include "header.h"

void writeStatsDocs(clock_t start, clock_t end) {
	int i = shared_var->stats->appointment_total;

	shared_var->stats->time_beg_app[i] = start;
	shared_var->stats->time_end_app[i] = end;
	shared_var->stats->appointment_total++;
}