// User library for system calls.
// Author: Brian Law

int print(char* s) {
	return interrupt(0x21, 0, s, 0, 0);
}

int readSingleChar(char* buf) {
	return interrupt(0x21, 0x11, buf, 0, 0);
}

int readSingleString(char* buf) {
	return interrupt(0x21, 0x01, buf, 0, 0);
}

int readSingleFile(char* filename, char* fileBuffer) {
	return interrupt(0x21, 0x03, filename, fileBuffer, 0);
}

int executeProgram(char* filename, int segment) {
	return interrupt(0x21, 0x04, filename, segment, 0);
}

void terminateProgram() {
	interrupt(0x21, 0x05, 0, 0, 0);
}


int deleteSingleFile(char* filename) {
	return interrupt(0x21, 0x07, filename, 0, 0);
}

int writeSingleFile(char* filename, char* fileBuffer, int sectors) {
	return interrupt(0x21, 0x08, filename, fileBuffer, sectors);
}

void listDirectory(char* contentsBuffer) {
	return interrupt(0x21, 0xFF, contentsBuffer, 0, 0);
}
