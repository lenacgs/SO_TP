#include "header.h"

void readConfig() {
	int val[4], i=0;
	FILE * file;
	file = fopen("Config.txt", "r");

	if (file) {
		while (fscanf(file, "%d", &val[i]) != EOF)
			i++;

		shared_var->config->n_triage = val[0];
		shared_var->config->n_doctors = val[1];
		shared_var->config->shift_dur = val[2];
		shared_var->config->mq_max = val[3];
	}
	fclose (file);
}