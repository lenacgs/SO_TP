#include "header.h"

//process that reads input from console and writes it to pipe
int main() {
	char *line;
	char patient[20];
	int array[3], array2;
	char triage[5];
	int pipe_fd;

	if ((pipe_fd = open(PIPE, O_WRONLY|O_NONBLOCK)) < 0) {
		perror ("pipe_fd open:");
	}
	else { //pipe blocks until both ends are opened
		printf("Pipe has been opened for writing successfully\nListening for input in console...\n");
		line = (char*)malloc(sizeof(char) * BUFFER_SIZE);
		while(1) {
			if (fgets(line, BUFFER_SIZE, stdin)) {
				fflush(stdin);
				// input error
				if (sscanf(line, "%s %d %d %d", patient, &array[0], &array[1], &array[2]) != 4) { //not a patient
					if (sscanf(line, "%s %d", triage, &array2) != 2) { //not a triage command
						printf("INPUT USAGE\n->Single patient: name triage_time appointment_time priority\nPatient group: n_patientrs triage_time appointment_time priority\n->Alter number of trige threads: TRIAGE:n\n");
					}
				}
				else {
					if (write(pipe_fd, line, BUFFER_SIZE) == BUFFER_SIZE) {
						printf("input written to input_pipe: %s\n", line);
					}
				}
			}
			else {
				printf("failed reading input\n");
				close(pipe_fd);
			}
		}	
	}
}