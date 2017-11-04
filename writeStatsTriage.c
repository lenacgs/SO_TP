#include "header.h"

void writeStatsTriage(clock_t end) {
	int i;

	i = shared_var->triage_total;

	shared_var->time_mqueue[i] = end;

	shared_var->triage_total++;
}