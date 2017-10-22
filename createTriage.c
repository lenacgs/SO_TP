#include "header.h"

void createTriage(int i) {
	triageIds[i] = i;
	//function to create triage threads
	if (pthread_create(&my_thread[i], NULL, triageGoToWork, &triageIds[i]) != 0) {
		perror("Error criating triage thread!");
		exit(0);
	}
	#ifdef DEBUG
	printf("Triage thread created.");
	#endif

}