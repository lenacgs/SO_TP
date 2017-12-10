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

	sem_unlink(SEM_PATIENTS_QUEUE);
	sem_queue = sem_open(SEM_PATIENTS_QUEUE, O_CREAT|O_EXCL, 0700, 0);
	if (sem_queue == SEM_FAILED) {
		perror("sem_patients_queue creation;");
		exit(0);
	}

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
	// if shget return <= 0 , it's an error
	if ((shmid = shmget(IPC_PRIVATE, sizeof(struct mem_cell), IPC_CREAT|0777)) <= 0){
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
 	if (mkfifo(PIPE, O_CREAT|O_EXCL|0666) < 0) perror("mkfifo:");
 	else {
 		#ifdef DEBUG
 		printf("Named pipe input_pipe created with success\n");
 		#endif
 	}
}

void create_mq() {
	if ((mq_id = msgget(IPC_PRIVATE, IPC_CREAT|0777)) < 0) {
		perror("msgget:");
	}

}

void new_patient(char * line) {
	Patient new;
	queue_node *node = (queue_node*)malloc(sizeof(queue_node));
	new.name = (char*)malloc(sizeof(char)*50);
	strcpy(new.name, line);
	new.arrival_time = clock();
	line = strtok(NULL, " ");
	new.triage = atoi(line);
	line = strtok(NULL, " ");
	new.appointment =  atoi(line);
	line = strtok(NULL, " ");
	new.priority = atoi(line);

	printf("New patient entered the hospital:\n->name: %s\n->arrival_time: %lu\n->triage time: %d seconds\n->appointment time: %d seconds\n->priority: %d", new.name, new.arrival_time, new.triage, new.appointment, new.priority);


	//putting the new client in the triage Queue
	node->patient = &new;
	node->next = NULL;
	add_node(node);
}

void add_node(queue_node * node) {
	queue_node * aux;
	aux = triage_queue;
	printf("hello\n");
	printf("yo\n");
	//percorre a lista
	while (aux->next != NULL) {
		printf("inside while: aux->next!=NULL\n");
		printf("%s\n", aux->next->patient->name);
		aux = aux->next;
	}
	printf("end of linked list\n");
	//aux is the last node of the list
	aux->next = node;
	printf("node has been added to list!: lets check: priority %d, name %s\n", triage_queue->next->patient->priority, triage_queue->next->patient->name);
	sem_post(sem_queue);
	printf("--------------------semaphore value increased---------------\n");
}

void create_patient(int id, int t_triage, int t_appt, int priority) {
	Patient *new = (Patient*)malloc(sizeof(Patient));
	queue_node *node = (queue_node*)malloc(sizeof(queue_node));
	new->name = (char*)malloc(sizeof(char)*10);

	snprintf(new->name, 16, "%d", id);
	new->arrival_time = clock();
	new->triage = t_triage;
	new->appointment = t_appt;
	new->priority = priority;


	node->patient = new;
	node->next = NULL;
	add_node(node);
	return;


}

void new_group(char * line) {
	int i, n_patients = (int)malloc(sizeof(int));
	int triage = (int)malloc(sizeof(int));
	int appt = (int)malloc(sizeof(int));
	int priority = (int)malloc(sizeof(int));

	printf("yo im goin to create a new ol' group oh patients nigga\n");
	n_patients = atoi(line);
	line = strtok(NULL, " ");
	triage = atoi(line);
	line = strtok(NULL, " ");
	appt = atoi(line);
	line = strtok(NULL, " ");
	priority = atoi(line);

	for (i=0; i<n_patients; i++) {
		create_patient(patient_id, triage, appt, priority);
		printf("patient created\n");
		patient_id++;
	}
}


//thread for reading input from named pipe
void * listening() {
	int pipe_fd;
	char * input = NULL, * line = NULL;
	input = (char*)malloc(sizeof(char)*BUFFER_SIZE);
	line = (char*)malloc(sizeof(char)*BUFFER_SIZE);

	pipe_fd = open(PIPE, O_RDONLY|O_NONBLOCK);
	if (pipe_fd < 0) {
		perror ("pipe_fd open:");
		exit(0);
	}
	else {
		printf("Pipe has been opened for reading successfully\n");
		while (1) {
			if (read(pipe_fd, input, BUFFER_SIZE) > 0) {
				printf("----->>>>>>>......input: %s\n", input);
				line = strtok(input, " ");
				if (isdigit(line[0])) {
					new_group(line);
				}
				else {
					printf("new patient just entered the hospital!\n");
					new_patient(line);
				}
			}
		}
	}
}

void init() {
	int i;
	parentpid = getpid();
	patient_id = 1000;
	triage_queue = (queue_node*)malloc(sizeof(queue_node));
	triage_queue->next = NULL;
	triage_queue->patient = NULL;

	init_signal_handling();

	create_shm();

	init_stats();
	
	read_config();

	create_semaphores();

	create_pipe();

	listening_thread = (pthread_t)malloc(sizeof(pthread_t));
	if (pthread_create(&listening_thread, NULL, listening, NULL) != 0) {
		perror("error creating listening thread:");
		exit(0);
	}

	create_mq();

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