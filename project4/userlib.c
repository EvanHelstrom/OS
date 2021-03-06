// P4 user library functions, for user programs on our operating system.
// Build on P3 user library functions by Brian Law
//Author: Evan Quist

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

// delete a file from the directory by name
// Will return -1 if file not found, 1 if it is deleted.
int deleteFile(char* filename) {
	return interrupt(0x21, 0x07, filename, 0, 0);
}

// Write a new file into the directory by name
// fileBuffer should be size 13312 for maximum-size (26-sectors) files.
// Sectors is the max number of sectors to write to
// Will return -2 if sectors run out, -1 if no directory room, else the number of written sectors
int writeFile(char* filename, char* fileBuffer, sectors) {
	return interrupt(0x21, 0x08, filename, fileBuffer, sectors);
}

// Handle the dir command and get files in directory
// fileBuffer will be a max of 512 bytes
// Returns 1
int directory(char* fileBuffer) {
	return interrupt(0x21, 0x09, fileBuffer);
}
