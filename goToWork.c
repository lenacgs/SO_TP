void goToWork() {
	//insto funciona?
	clock_t start = clock();
	clock_t end = clock();
	shift_time = end - start;
	while (getPatient() && (shift_time != SHIFT_LENGTH)){
		end = clock();
		shift_time = end - start;
	}
}