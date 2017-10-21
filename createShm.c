#include "header.h"

void createShm() {
	shmid = shmget(IPC_PRIVATE, sizeof(struct stats), IPC_CREAT|0777); //no idea
	shared_var = shmat(shmid, NULL, 0); //attach shared mem

	//initialize shared var
	shared_var->triage_total = 0;
	shared_var->appointment_total = 0;
	shared_var->time_queue = (float*)malloc(VECTOR_SIZE * sizeof(float));
	shared_var->time_mqueue = (float*)malloc(VECTOR_SIZE * sizeof(float));
	shared_var->time_beg_app = (float*)malloc(VECTOR_SIZE * sizeof(float));
	shared_var->time_end_app = (float*)malloc(VECTOR_SIZE * sizeof(float));
}