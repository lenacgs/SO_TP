#include "header.h"

void catch_ctrlc(int signum) {

	if(signum == SIGINT)
		#ifdef DEBUG 
		printf("\nSignal received. Cleaning up..."); 
		#endif
		terminate();
}

void init_signal_handling() {

	signal(SIGINT, catch_ctrlc);

}

void create_semaphores() {

	shared_var->semaphores->sem_processes = (sem_t*)malloc(sizeof(sem_t));
	shared_var->semaphores->sem_processes = sem_open(SEM_PROCESSES, O_CREAT, 0666, shared_var->config->n_doctors);

	if (shared_var->semaphores->sem_processes == SEM_FAILED) {
		perror("SEM_OPEN: ");
		return;
	}
	#ifdef DEBUG
	printf("\nSem_processes created with initial value of %d", shared_var->config->n_doctors);
	#endif

	shared_var->semaphores->nextin = 0;
	shared_var->semaphores->processes = (pid_t*)malloc(shared_var->config->n_doctors*sizeof(pid_t));
}

void read_config() {
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

void create_process() {
	int i = shared_var->semaphores->nextin;

	if ((shared_var->semaphores->processes[i] = fork()) == 0) {
		#ifdef DEBUG
		printf("New doctor (PID %d) created\nextin", getpid());
		#endif

		doctor_worker();

		sem_post(shared_var->semaphores->sem_processes);
		exit(0);
	}
	else if (shared_var->semaphores->processes[i] < 0) {
		perror("Error creating new doctor: ");
		return;
	}
	else if (shared_var->semaphores->processes[i] < 0) return;
}

void create_thread(int i) {

	threadIds[i] = i;

	if (pthread_create(&threads[i], NULL, thread_worker, &threadIds[i]) != 0) {
		perror("Error criating triage thread!");
		exit(0);
	}
	
	#ifdef DEBUG
	printf("\nTriage thread created.");
	#endif

}

void init_stats() {

	shared_var->stats = (Stats*)malloc(sizeof(struct stats));
	shared_var->stats->time_queue = (clock_t*)malloc(VECTOR_SIZE * sizeof(clock_t));
	shared_var->stats->time_mqueue = (clock_t*)malloc(VECTOR_SIZE * sizeof(clock_t));
	shared_var->stats->time_beg_app = (clock_t*)malloc(VECTOR_SIZE * sizeof(clock_t));
	shared_var->stats->time_end_app = (clock_t*)malloc(VECTOR_SIZE * sizeof(clock_t));
	shared_var->stats->triage_total = 0;
	shared_var->stats->appointment_total = 0;

}

void create_shm() {

	if ((shmid = shmget(IPC_PRIVATE, sizeof(struct mem_cell), IPC_CREAT|0777)) <= 0){
		perror("SHMGET: ");
		return;
	}
	shared_var = shmat(shmid, NULL, 0); //attach shared mem

}

void init() {
	int i;

    pthread_mutex_init(&mutex, NULL);

	pthread_mutex_lock(&mutex);

	init_signal_handling();

	create_shm();

	init_stats();

	sleep(2);

	#ifdef DEBUG
	printf("triage_total: %d\nappointment_total:%d\n", shared_var->stats->triage_total, shared_var->stats->appointment_total);
	#endif

	shared_var->config = (Config*)malloc(sizeof(Config));
	read_config();
	#ifdef DEBUG
	printf("n_triage: %d\nn_doctors: %d\nshift_dur: %d\nmq_max: %d\n", shared_var->config->n_triage, shared_var->config->n_doctors, shared_var->config->shift_dur, shared_var->config->mq_max);
	#endif

	shared_var->semaphores = (Semaphores*)malloc(sizeof(struct semaphores));
	create_semaphores();

	// threadIds = (int*)malloc(shared_var->config->n_triage * sizeof(int)); //aloca memoria para o numero de triagens em struct config
	// threads = (pthread_t*)malloc(shared_var->config->n_triage * sizeof(pthread_t));
	// for (i=0; i<shared_var->config->n_triage; i++)
	// 	create_thread(i);

	pthread_mutex_unlock(&mutex);

	while (1) {
			sleep(1);
			if (sem_wait(shared_var->semaphores->sem_processes) == -1) perror("\nsem_wait error: ");
			create_process();
	}
}


//processo principal
int main() {

	init();

}