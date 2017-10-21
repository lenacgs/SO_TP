#include "header.h"

void main() {
	int i;
	//processo principal

	//read file config.txt
	readConfig(); 
	//test
	#ifdef DEBUG
	printf("n_triage: %d\nn_doctors: %d\nshift_dur: %d\nmq_max: %d\n", n_triage, n_doctors, shift_dur, mq_max);
	#endif

	//create shared memory
	createShm();
	//test
	#ifdef DEBUG
	printf("triage_total: %d\nappointment_total:%d\n", shared_var->triage_total, shared_var->appointment_total);
	#endif
	
	//create Doctor processes
	for (i=0; i<n_doctors; i++)
		createDoctorProcs();


}