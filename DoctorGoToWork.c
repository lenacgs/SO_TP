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
	writeStatsDocs(start, clock());


}