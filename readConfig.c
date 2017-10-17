void readConfig() {
	int val [10] = {NULL};
	int i = 0;
	FILE * file;
	file = fopen("Config.txt", "r");
	if (file) {
		while (fscanf(file, "%d", val[i]) != EOF) {
			//change variables to values read
		}
	}
	fclose(file);
}