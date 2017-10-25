#include "header.h"

void readConfig(Config *config) {
	int val[4], i=0;
	FILE * file;
	file = fopen("Config.txt", "r");

	if (file) {
		while (fscanf(file, "%d", &val[i]) != EOF)
			i++;

		config->n_triage = val[0];
		config->n_doctors = val[1];
		config->shift_dur = val[2];
		config->mq_max = val[3];
	}
	fclose (file);
}