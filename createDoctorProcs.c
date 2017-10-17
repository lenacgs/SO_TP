void createDoctorProcs() {
	if (fork() == 0) {
		printf("Doctor process with PID:%d created.\n", getpid());
		goToWork();
		printf("Doctor process with PID: %d --- Shift finished.\n Doctor process leaving work.\n", getpid());
		exit(0);
	}
}