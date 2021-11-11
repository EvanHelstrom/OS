// P3 user library functions, for user programs on our operating system.
//Author: Brian Law

// Print out the c-string s to the console.
int print(char* s) {
	return interrupt(0x21, 0, s, 0, 0);
}

// Read in a single character from user input and save it to the buffer.
int readSingleChar(char* buf) {
	return interrupt(0x21, 0x11, buf, 0, 0);
}

// Read in a single line from user input and save it to the buffer.
int readSingleString(char* buf) {
	return interrupt(0x21, 0x01, buf, 0, 0);
}

// Read in the file with the given filename into the provided buffer.
// fileBuffer should be size 13312 for maximum-size (26-sectors) files.
// Will return the number of sectors in the file read, -1 if no file read.
int readSingleFile(char* filename, char* fileBuffer) {
	return interrupt(0x21, 0x03, filename, fileBuffer, 0);
}

// Execute the program with the given filename by loading it into the specified
// memory segment.
// Will return -1 if program not found, -2 if segment invalid.
int executeProgram(char* filename, int segment) {
	return interrupt(0x21, 0x04, filename, segment, 0);
}

// Terminate this program and return to shell.
void terminateProgram() {
	interrupt(0x21, 0x05, 0, 0, 0);
}
