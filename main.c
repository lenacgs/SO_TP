#include "header.h"

void signal_handling(int signo) {

	if(signo == SIGINT)
		#ifdef DEBUG 
		printf("\nSignal received. Cleaning up..."); 
		#endif
		terminate();
}

void init_signal_handling() {

	signal(SIGINT, signal_handling);

}

void create_semaphores() {

	shared_var->semaphores->sem_processes = sem_open(SEM_PROCESSES, O_CREAT, 0666, shared_var->config->n_doctors);

	if (shared_var->semaphores->sem_processes == SEM_FAILED) {
		perror("SEM_OPEN: ");
		return;
	}
	else {
		#ifdef DEBUG
		printf("\nSem_processes created with initial value of %d", shared_var->config->n_doctors);
		#endif
	}

	shared_var->semaphores->nextin = 0;
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

void create_process(int i) {

	processes[i] = fork();

	if (processes[i] < 0) {
		perror("Error creating new doctor: ");
		return;
	}
	else {
		#ifdef DEBUG
		printf("\nDoctor process with PID:%d created.", getpid());
		#endif
		doctor_worker();
		#ifdef DEBUG
		printf("\nDoctor process with PID: %d --- Shift finished.Doctor process leaving work.", getpid());
		#endif
		sem_post(shared_var->semaphores->sem_processes);
		kill(processes[i], SIGKILL);
	}
}

void create_thread(int i) {

	threadIds[i] = i;

	if (pthread_create(&threads[i], NULL, triageGoToWork, &threadIds[i]) != 0) {
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

	init_signal_handling();

	create_shm();

	init_stats();

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

	threadIds = (int*)malloc(shared_var->config->n_triage * sizeof(int)); //aloca memoria para o numero de triagens em struct config
	threads = (pthread_t*)malloc(shared_var->config->n_triage * sizeof(pthread_t));
	for (i=0; i<shared_var->config->n_triage; i++)
		create_thread(i);

	processes = (pid_t*)malloc(shared_var->config->n_doctors*sizeof(pid_t));
	while (1) {
		printf("\nWaiting...");
		sem_wait(shared_var->semaphores->sem_processes);
		printf("\nResources available! Creating process...");
		create_process(shared_var->semaphores->nextin);
	}

}

//processo principal
int main() {

	init();

}