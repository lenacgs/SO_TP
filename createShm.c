void createShm() {
	Stats stats;
	shmid = shmget(IPC_PRIVATE, sizeof(struct stats), ICP_CREAT | 0766); //no idea
	stats = shmat(shmid, NULL, 0); //attach shared mem

	//initialize shared var
	stats->triage_total = 0;
	stats->appointment_total = 0;
	stats->time_queue = (int*)malloc(VECTOR_SIZE * sizeof(int));
	stats->time_mqueue = (int*)malloc(VECTOR_SIZE * sizeof(int));
	stats->time_beg_app = (int*)malloc(VECTOR_SIZE * sizeof(int));
	stats->time_end_app = (int*)malloc(VECTOR_SIZE * sizeof(int));
}