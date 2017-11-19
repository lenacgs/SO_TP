#include "header.h"

void write_stats_docs(clock_t start, clock_t end) {
	int i = shared_var->stats->appointment_total;
	if (pthread_mutex_lock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_lock error: ");
	shared_var->stats->appointment_total++;
	shared_var->stats->time_beg_app[i] = start;
	shared_var->stats->time_end_app[i] = end;
	if (pthread_mutex_unlock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_unlock error: ");
}

void write_stats_triage(clock_t end) {
	int i;
	if (pthread_mutex_lock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_lock error: ");
	i = shared_var->stats->triage_total;

	shared_var->stats->time_mqueue[i] = end;

	shared_var->stats->triage_total++;
	if (pthread_mutex_unlock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_unlock error: ");
}