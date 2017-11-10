#include "header.h"

void catch_ctrlc(int signum) {

	if(signum == SIGINT)
		
		if (getpid() == parentpid) {
			#ifdef DEBUG 
			printf("\nSignal received. Cleaning up...\n"); 
			#endif
			terminate();
		}
}

void init_signal_handling() {

	signal(SIGINT, catch_ctrlc);

}

void create_semaphores() {
	shared_var->semaphores = (Semaphores*)malloc(sizeof(struct semaphores));
	shared_var->semaphores->sem_processes = sem_open(SEM_PROCESSES, O_CREAT, 0666, shared_var->config->n_doctors);
	if (shared_var->semaphores->sem_processes == SEM_FAILED) {
		perror("sem_open: ");
		return;
	}
	#ifdef DEBUG
	printf("sem_processes created with initial value of %d\n", shared_var->config->n_doctors);
	#endif

	shared_var->semaphores->nextin = 0;
	shared_var->semaphores->processes = (pid_t*)malloc(shared_var->config->n_doctors*sizeof(pid_t));
	if (pthread_mutex_init(&shared_var->semaphores->mutex, NULL) != 0) perror("pthread_mutex_t error: ");
}

void read_config() {
	int val[4], i=0;
	FILE * file;
	file = fopen("Config.txt", "r");
	shared_var->config = (Config*)malloc(sizeof(Config));

	if (file) {
		while (fscanf(file, "%d", &val[i]) != EOF)
			i++;

		shared_var->config->n_triage = val[0];
		shared_var->config->n_doctors = val[1];
		shared_var->config->shift_dur = val[2];
		shared_var->config->mq_max = val[3];
	}
	fclose (file);

	#ifdef DEBUG
	printf("n_triage: %d\nn_doctors: %d\nshift_dur: %d\nmq_max: %d\n", shared_var->config->n_triage, shared_var->config->n_doctors, shared_var->config->shift_dur, shared_var->config->mq_max);
	#endif
}

void create_process() {
	int i = shared_var->semaphores->nextin;
	shared_var->semaphores->nextin++;
	shared_var->semaphores->nextin = shared_var->semaphores->nextin % shared_var->config->n_doctors;

	if ((shared_var->semaphores->processes[i] = fork()) == 0) {
		#ifdef DEBUG
		printf("New doctor (PID %d) created\n", getpid());
		#endif

		doctor_worker();
		
		sem_post(shared_var->semaphores->sem_processes);
		printf("Doctor (PID %d) leaving\n", getpid());
		exit(0);
	}
	else if (shared_var->semaphores->processes[i] < 0) {
		perror("Error creating new doctor: ");
		return;
	}
}

void create_thread(int i) {
	threadIds[i] = i;

	if (pthread_create(&threads[i], NULL, thread_worker, &threadIds[i]) != 0) {
		perror("Error criating triage thread: ");
		if (pthread_mutex_unlock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_unlock error: ");
		exit(0);
	}
	
	#ifdef DEBUG
	printf("Triage thread created\n");
	#endif
	if (pthread_mutex_unlock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_unlock error: ");
}

void init_stats() {
	shared_var->stats = (Stats*)malloc(sizeof(struct stats));
	shared_var->stats->time_queue = (clock_t*)malloc(VECTOR_SIZE * sizeof(clock_t));
	shared_var->stats->time_mqueue = (clock_t*)malloc(VECTOR_SIZE * sizeof(clock_t));
	shared_var->stats->time_beg_app = (clock_t*)malloc(VECTOR_SIZE * sizeof(clock_t));
	shared_var->stats->time_end_app = (clock_t*)malloc(VECTOR_SIZE * sizeof(clock_t));
	shared_var->stats->triage_total = 0;
	shared_var->stats->appointment_total = 0;

	#ifdef DEBUG
	printf("triage_total: %d\nappointment_total:%d\n", shared_var->stats->triage_total, shared_var->stats->appointment_total);
	#endif
}

void create_shm() {

	if ((shmid = shmget(IPC_PRIVATE, sizeof(struct mem_cell), IPC_CREAT|0777)) <= 0){
		perror("SHMGET: ");
		return;
	}
	shared_var = shmat(shmid, NULL, 0); //attach shared mem

	#ifdef DEBUG
	printf("Shared memory created\n");
	#endif

}

void init() {
	int i;
	parentpid = getpid();

	init_signal_handling();

	create_shm();

	init_stats();
	
	read_config();

	create_semaphores();

	if (pthread_mutex_lock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_lock error: ");
	threadIds = (int*)malloc(shared_var->config->n_triage * sizeof(int)); //aloca memoria para o numero de triagens em struct config
	threads = (pthread_t*)malloc(shared_var->config->n_triage * sizeof(pthread_t));
	for (i=0; i<shared_var->config->n_triage; i++)
		create_thread(i);

	while (1) {
		if (sem_wait(shared_var->semaphores->sem_processes) != 0) perror("sem_wait error: ");
		if (pthread_mutex_lock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_lock error: ");
		create_process();
		if (pthread_mutex_unlock(&shared_var->semaphores->mutex) != 0) perror("pthread_mutex_unlock error: ");
	}
}

//processo principal
int main() {

	init();

}