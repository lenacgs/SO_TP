#include "header.h"

void readConfig() {
	int val[4], i=0;
	FILE * file;
	file = fopen("Config.txt", "r");

	if (file) {
		while (fscanf(file, "%d", &val[i]) != EOF)
			i++;

		n_triage = val[0];
		n_doctors = val[1];
		shift_dur = val[2];
		mq_max = val[3];
	}
	fclose (file);
}