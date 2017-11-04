#include "header.h"

void writeStatsDocs(clock_t start, clock_t end) {
	int i = shared_var->appointment_total;

	shared_var->time_beg_app[i] = start;
	shared_var->time_end_app[i] = end;
	shared_var->appointment_total++;
}