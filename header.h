#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>


#define VECTOR_SIZE 10 //number of patients
#define DEBUG
//#define PRINT_STATS
#define SEM_PROCESSES "sem_processes"

typedef struct patient {
	char * name;
	int arrival; //order
	clock_t arrival_time; //in seconds
	int triage; //in seconds
	int appointment; //in seconds
	int priority; //1, 2 or 3
} Patient;

typedef struct stats { //guarda as informações de todos os pacientes
	int triage_total; //incrementada sempre que um paciente é triado
	int appointment_total; //incrementada sempre que um paciente entre na consulta
	clock_t * time_queue; //array de momentos: quando cada paciente entre na queue 
	clock_t * time_mqueue; //array de momentos: quando cada paciente entra na MQ
	clock_t * time_beg_app; //array de momentos: quando cada paciente entra na consulta
	clock_t * time_end_app; //array de momentos: quando cada paciente sai na consulta
} Stats;

typedef struct config {
	int n_triage;
	int n_doctors;
	int shift_dur;
	int mq_max;
} Config;

typedef struct semaphores {
	sem_t * sem_processes;
	int nextin;
	pid_t *processes;
	pthread_mutex_t mutex;
} Semaphores;

typedef struct mem_cell {
	Config * config;
	Stats * stats;
	Semaphores * semaphores;
} mem_cell;

pthread_t *threads;
int *threadIds;
int shmid;
mem_cell *shared_var;
pid_t parentpid;


void init();
void create_thread();
void create_process();
void init_signal_handling();
void signal_handling(int signo);
void read_config();
void init_stats();
void create_shm();
void create_semaphors();
void doctor_worker();
void * thread_worker(void * id);
void write_stats_docs(clock_t start, clock_t end);
void write_stats_triage(clock_t end);
void terminate();

