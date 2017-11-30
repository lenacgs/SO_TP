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
	sem_unlink(SEM_PROCESSES);
	shared_var->semaphores = (Semaphores*)malloc(sizeof(struct semaphores));
	shared_var->semaphores->sem_processes = sem_open(SEM_PROCESSES, O_CREAT|O_EXCL, 0700, shared_var->config->n_doctors);
	if (shared_var->semaphores->sem_processes == SEM_FAILED) {
		perror("sem_processes creation: ");
		exit(0);
	}
	sem_unlink(MUTEX);
	shared_var->semaphores->mutex = sem_open(MUTEX, O_CREAT|O_EXCL, 0700, 1);
	if (shared_var->semaphores->mutex == SEM_FAILED) {
		perror("mutex creation:");
		exit(0);
	}
	#ifdef DEBUG
	printf("mutex created\n");
	printf("sem_processes created with initial value of %d\n", shared_var->config->n_doctors);
	#endif

	shared_var->semaphores->nextin = 0;
	shared_var->semaphores->processes = (pid_t*)malloc(shared_var->config->n_doctors*sizeof(pid_t));
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
}

void create_thread(int i) {
	threadIds[i] = i;

	if (pthread_create(&threads[i], NULL, thread_worker, &threadIds[i]) != 0) {
		perror("Error criating triage thread: ");
		exit(0);
	}
	
	#ifdef DEBUG
	printf("Triage thread created\n");
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

//creating and opening named pipe
void create_pipe() {
	unlink(PIPE);
	mkfifo(PIPE, O_CREAT|O_EXCL|0666);

	pipe_fd = open(PIPE, O_RDONLY|O_NONBLOCK);
	printf("Listening...\n");
}

// void create_mq() {
// 	mq_id = msgget(IPC_PRIVATE, IPC_CREAT|0777);
// 	if (mq_id < 0) {
// 		perror("mssget:");
// 	}

// }

// void listening() {
// 	while(1) {
// 		scanf("")
// 	}
// }

void init() {
	int i;
	parentpid = getpid();

	init_signal_handling();

	create_shm();

	init_stats();
	
	read_config();

	create_semaphores();

	// create_pipe();
	// pthread_t thrd;
	// pthread_create(&thrd, NULL, listening, NULL);

	// create_mq();

	threadIds = (int*)malloc(shared_var->config->n_triage * sizeof(int)); //aloca memoria para o numero de triagens em struct config
	threads = (pthread_t*)malloc(shared_var->config->n_triage * sizeof(pthread_t));
	for (i=0; i<shared_var->config->n_triage; i++)
		create_thread(i);

	while (1) {
		if (sem_wait(shared_var->semaphores->sem_processes) != 0) perror("sem_wait error: ");
		
		create_process();
	}
}

//processo principal
int main() {

	init();

}