#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>


#define VECTOR_SIZE 10
#define DEBUG

typedef struct patient {
	char * name;
	int arrival; //order
	int arrival_time; //in seconds
	int triage; //in seconds
	int appointment; //in seconds
	int priority; //1, 2 or 3
} Patient;

typedef struct stats { //guarda as informações de todos os pacientes
	int * attach;
	int triage_total; //incrementada sempre que um paciente é triado
	int appointment_total; //incrementada sempre que um paciente entre na consulta
	float * time_queue; //array de momentos: quando cada paciente entre na queue 
	float * time_mqueue; //array de momentos: quando cada paciente entra na MQ
	float * time_beg_app; //array de momentos: quando cada paciente entra na consulta
	float * time_end_app; //array de momentos: quando cada paciente sai na consulta
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


void createTriage();
Patient getPatient();
void doctorGoToWork();
void createDoctorProcs();
void readConfig();
void createShm();
void *triageGoToWork(int id);
