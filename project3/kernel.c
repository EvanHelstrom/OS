/* Evan Quist, OS, 10/20/2021 */
int mod(int dividend, int divisor);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int readFile(char *filename, char *buf);
int checkMatch(char *filename, char *buffer, int subarr);
int executeProgram(char* name, int segment);
void terminate();

int main(void) {
  	
	
	
	makeInterrupt21();
	interrupt(0x21, 0x04, "shell", 0x2000, 0);
	interrupt(0x21, 0x00, "Done!\n\r", 0, 0);
	while(1); /* infinite loop */

	return 0;
}

/** Calculate dividend % divisor. Handles negative numbers properly.
*/
int mod(int dividend, int divisor) {
	// Deal with negative divisor.
	if (divisor < 0) {
		divisor = divisor * -1;
	}
	// This should probably throw an exception... but we're the OS!
	else if (divisor == 0) {
		return -1;
	}

	// Positive dividend
	if (dividend >= 0) {
		while (dividend >= divisor) {
			dividend = dividend - divisor;
		}
	}
	// Negative dividend
	else {
		while (dividend < 0) {
			dividend = dividend + divisor;
		}
	}

	return dividend;
}


/** 
Print the null-terminated C-string at memaddr str to the screen.
Return the number of characters printed.
*/
int printString(char *str) {
	// Counter for characters being printed.
	int i = 0;

	// Print until null-character.
	while (str[i] != 0x00) {
		interrupt(0x10, 0x0E * 256 + str[i], 0, 0, 0);
		i++;
	}
	return i;
}

/** Read a single character from standard input.
*/
int readChar() {
	return interrupt(0x16, 0, 0, 0, 0);
}

/** Read a string from input into the memaddr buf until the user presses Enter.
*/
int readString(char *buf) {
	// Counter for # characters read
	int i = 0;

	// Initialize by reading first character.	
	char c = readChar();

	// Continue until read character is a newline.
	while (c != 0x0d) {
		// Dealing with backspaces
		if (c == 0x08) {

			// Do not backspace past starting point.
			if (i != 0) {
				// Back up the cursor
				interrupt(0x10, 0xe*256 + c, 0, 0, 0);

				// Print blank to remove previous character
				interrupt(0x10, 0xe*256, 0, 0, 0);

				// Back cursor up again after printing!
				interrupt(0x10, 0xe*256 + c, 0, 0, 0);

				// Remove last read character from buffer
				i--;
				buf[i] = 0x00;
			}
		}
		// Normal characters go into the buffer and get echoed
		else {
			buf[i] = c;
			interrupt(0x10, 0xe*256 + c, 0, 0, 0);
			i++;

		}

		// Read next char for next loop iteration
		c = readChar();
	}

	// Terminate buffer with null character
	buf[i] = 0x00;
	return i;
}

/** Read in the selected disk sector into the memaddr at buf. Return 1.
*/
int readSector(char *buf, int absSector) {
	int relSector = mod(absSector,18) + 1;
	int head = mod(absSector / 18, 2);
	int track = absSector / 36;
	interrupt(0x13,0x02*256 + 1, buf, track*256 + relSector, head*256 + 0x00);
	return 1;
}

/** Interrupt handler for interrupt 21.
*/
int handleInterrupt21(int ax, int bx, int cx, int dx) {	
	int returnValue = 1;
	if (ax == 0x00) {
		returnValue = printString((char *)bx);
	}
	else if (ax == 0x11) {
		*((char*)bx) = (char)readChar();
	}
	else if (ax == 0x01) {
		returnValue = readString((char *)bx);
	}
	else if(ax == 0x03) {
		returnValue = readFile((char *)bx, (char *)cx);
	}
	else if(ax == 0x04) {
		returnValue = executeProgram((char *)bx, cx);
	}
	else if(ax == 0x05) {
		terminate();
	}
	return returnValue;
}
/** Read in a user selected file, returns number of sectors read, or -1 if not found
*/
int readFile(char *filename, char *buf) {
	char diskDirectory[512];
	int i;
	int x = 0;
	readSector(diskDirectory, 2); // Read in the disk directory

	for(i = 0; i < 16; i = i + 1){ // Iterate through the 16 entries
		if(checkMatch(filename, diskDirectory, i*32) == 0){ // If the entry matches the filename
			
			while(diskDirectory[(i*32)+x+6] != 0x00 && x < 26){ // 6 added to start at end of name, 26 is max number of addresses
				readSector(buf+((x)*512), diskDirectory[(i*32)+(x+6)]); // Iterate through the addresses loading them in buffer
				x++;
			}
			return x;
		}
	}
	return -1;
}
/** Handle checking for file name matches
*/
int checkMatch(char *filename, char *buffer, int subarr) {
	int i = 0;
	while(i < 6){ // Length of name
		if(filename[i] == 0x00 && buffer[i+subarr] == 0x00){ // If they end at the same time
			return 0;
		}
		if(filename[i] != buffer[i+subarr]){ // If they didn't match at any point say no
			return -1;
		} else {
			i++;
		}
	}
	return 0; // If somehow they always matched yet ended differently (i.e. hit 6 limit)
}
/** Execute A Program
*/
int executeProgram(char* name, int segment) {
	int offset = 0;
	char buffer[13312]; /* the maximum size of a file*/
	if(segment < 0x2000 || segment >= 0xA000){ // Bounds given by project
		return -2;
	}
	if(readFile(name, buffer) == -1){ // If it cannot read the file
		return -1;
	}
	while(offset < 13312){ // Dont let it overflow
		putInMemory(segment, offset, buffer[offset]);	
		offset++;
	}
	launchProgram(segment);
}
/** End the run of a user program
*/
void terminate() {
	resetSegments();
	printString("Program Terminated");
	interrupt(0x21, 0x04, "shell", 0x2000, 0);
}
