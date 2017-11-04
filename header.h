#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>


#define VECTOR_SIZE 10 //number of patients
#define DEBUG

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

pthread_t *my_thread;
int *triageIds;
int shmid;
Stats *shared_var;
Config *config;


void createTriage();
void doctorGoToWork();
void createDoctorProcs();
void readConfig();
void createShm();
void *triageGoToWork(void * id);
void writeStatsDocs(clock_t start, clock_t end);
void writeStatsTriage(clock_t end);
