#include "header.h"

void doctorGoToWork() {
	//insto funciona?
	clock_t start = clock();
	clock_t end = clock();
	int shift_time = (end - start) / CLOCKS_PER_SEC;
	
	while (/*getPatient() && */((int)shift_time < config->shift_dur)){
		end = clock();
		shift_time = (end - start) / CLOCKS_PER_SEC;
	}
	
	shared_var->time_beg_app[shared_var->appointment_total] = start;
	shared_var->time_end_app[shared_var->appointment_total] = clock();
	#ifdef DEBUG
	printf("start: %lu\n", (*(shared_var->time_beg_app + shared_var->appointment_total))/CLOCKS_PER_SEC);
	printf("end: %lu\n", (*(shared_var->time_end_app + shared_var->appointment_total))/CLOCKS_PER_SEC);
	printf("shift duration: %lu\n", (*(shared_var->time_end_app + shared_var->appointment_total) - *(shared_var->time_beg_app + shared_var->appointment_total))/CLOCKS_PER_SEC);
	#endif
	shared_var->appointment_total++;


}