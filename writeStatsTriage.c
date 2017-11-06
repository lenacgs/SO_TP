#include "header.h"

void writeStatsTriage(clock_t end) {
	int i;

	i = shared_var->stats->triage_total;

	shared_var->stats->time_mqueue[i] = end;

	shared_var->stats->triage_total++;
}