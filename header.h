#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>


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

int n_triage;
int n_doctors;
int shift_dur; //in seconds
int mq_max;

int shmid;
Stats *shared_var;


void createTriage();
void getPatient();
void doctorGoToWork();
void createDoctorProcs();
void readConfig();
void createShm();