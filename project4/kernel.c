// P4 kernel.c
// Built on P3 kernel.c solution by Brian Law
// Author: Evan

int mod(int dividend, int divisor);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);

// P3 functions
int readFile(char *filename, char *buf);
int executeProgram(char* name, int segment);
void terminate();

// P4 functions
int writeSector(char *buffer, int sector);
int deleteFile(char *fname);
int writeFile(char *fname, char *buffer, int sectors);

int handleInterrupt21(int ax, int bx, int cx, int dx);

int main(void) {
	makeInterrupt21();
	interrupt(0x21, 0x04, "shell", 0x2000, 0);


	while(1){}

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

/** Read in a file into the provided character buffer.
filename should be a char buffer of maximum length 7 with the name of the file to be loaded
buf should be a char buffer with enough room for the whole file (512 bytes * 26 sectors =
13312 bytes - no need for null character, since we're treating this as binary, not ASCII)
Returns the number of sectors read, -1 if no file read.
*/
int readFile(char *filename, char *buf) {
	char diskDirectory[512];
	int fileEntry = 0;
	int filenameLocation = 0;
	int filenameChars = 0;
	int charMatch = 0;

	// Defaulting to -1 if file read error.
	int sectorsRead = -1;

	// Null filename.
	if (filename[0] == 0) {
		return -1;
	}

	// Read in the disk directory to a buffer to analyze.
	readSector(diskDirectory, 2);

	// Search for filenames in the disk directory. Only 16 files possible.
	for (fileEntry = 0; fileEntry < 16; fileEntry++) {

		// Each file entry is 32 bytes apart in the disk directory
		filenameLocation = fileEntry * 32;

		// Check the filename in the directory against the specified filename
		for (filenameChars = 0; filenameChars < 6; filenameChars++) {
			// Compare each character in the two filenames one at a time...
			if (filename[filenameChars] == diskDirectory[filenameLocation + filenameChars]) {

				// Some filenames may be <6 characters. If both filenames terminate prematurely
				// with no differences, then it's a match. Terminate the loop.
				if (filename[filenameChars] == 0) {
					charMatch = 6;			// match found
					filenameChars = 6;		// terminate inner loop
				}
				// Count the number of matches.
				else {
					charMatch++;
				}
			}
			// Any mismatched character means this is not the right filename.
			else {
				charMatch = 0;				// match not found
				filenameChars = 6;			// terminate inner loop
			}
		}

		// filename matched! terminate outer loop
		if (charMatch == 6) {
			fileEntry = 16;
		}
	}

	// If filename found
	if (charMatch == 6) {
		// Read in each sector of the file one at a time, put byte into provided buffer
		for (sectorsRead = 0; diskDirectory[filenameLocation + 6 + sectorsRead] != 0 && sectorsRead < 26; sectorsRead++) {
			readSector(buf + (512 * sectorsRead), diskDirectory[filenameLocation + 6 + sectorsRead]);
		}
	}

	return sectorsRead;
}

// Execute the program with the filename name by loading it into the specified segment of memory.
// Return -1 if program not found, -2 if segment invalid.
int executeProgram(char* filename, int segment) {
	char fileBuffer[13312];
	int sectorsRead = 0;
	int sectorsLoaded = 0;
	int bytesLoaded = 0;

	// Null filename.
	if (filename[0] == 0) {
		return -1;
	}

	// Invalid segments return -2
	if (segment == 0x0000 || segment == 0x1000 || segment >= 0xA000 || mod(segment, 0x1000) != 0) {
		return -2;
	}

	// Read the program into buffer, save the number of sectors in the file
	sectorsRead = readFile(filename, fileBuffer);

	// If for some reason there was a file reading error, return -1
	if (sectorsRead <= 0 || sectorsRead > 26) {
		return -1;
	}

	// For each sector of the program...
	for (; sectorsLoaded < sectorsRead; sectorsLoaded++) {
		// Load up each byte of the sector, put the byte into memory at the segment specified
		for (bytesLoaded = 0; bytesLoaded < 512; bytesLoaded++) {
			putInMemory(segment, sectorsLoaded * 512 + bytesLoaded, fileBuffer[sectorsLoaded * 512 + bytesLoaded]);
		}
	}

	launchProgram(segment);
}

// Terminate the running program and resume the shell
void terminate() {
	resetSegments();
	executeProgram("shell", 0x2000);
}

/** Read in the from buf and store in chosen sector. Return 1.
*/
int writeSector(char *buf, int absSector) {
	int relSector = mod(absSector,18) + 1;
	int head = mod(absSector / 18, 2);
	int track = absSector / 36;
	interrupt(0x13,0x03*256 + 1, buf, track*256 + relSector, head*256 + 0x00);
	return 1;
}

/** Delete a file with a chosen name. Return 1 if deleted, -1 if not
*/
int deleteFile(char *fname){
	char diskDirectory[512];
	char diskMap[512];
	int fileEntry = 0;
	int filenameLocation = 0;
	int filenameChars = 0;
	int charMatch = 0;
	int sectorsRead = -1;
	// Null filename.
	if (fname[0] == 0) {
		return -1;
	}

	// Read in the disk directory and disk map to a buffer to analyze.
	readSector(diskDirectory, 2);
	readSector(diskMap, 1);

	// Search for filenames in the disk directory. Only 16 files possible.
	for (fileEntry = 0; fileEntry < 16; fileEntry++) {

		// Each file entry is 32 bytes apart in the disk directory
		filenameLocation = fileEntry * 32;

		// Check the filename in the directory against the specified filename
		for (filenameChars = 0; filenameChars < 6; filenameChars++) {
			// Compare each character in the two filenames one at a time...
			if (fname[filenameChars] == diskDirectory[filenameLocation + filenameChars]) {

				// Some filenames may be <6 characters. If both filenames terminate prematurely
				// with no differences, then it's a match. Terminate the loop.
				if (fname[filenameChars] == 0) {
					charMatch = 6;			// match found
					filenameChars = 6;		// terminate inner loop
				}
				// Count the number of matches.
				else {
					charMatch++;
				}
			}
			// Any mismatched character means this is not the right filename.
			else {
				charMatch = 0;				// match not found
				filenameChars = 6;			// terminate inner loop
			}
		}

		// filename matched! terminate outer loop
		if (charMatch == 6) {
			fileEntry = 16;
		}
	}

	// If filename found
	if (charMatch == 6) {
		// Read each sector from the directory and change its map value to 0
		for (sectorsRead = 0; diskDirectory[filenameLocation + 6 + sectorsRead] != 0 && sectorsRead < 26; sectorsRead++) {
			diskMap[diskDirectory[filenameLocation + 6 + sectorsRead]] = 0x00;
		}
		// Change the first character in the directory to 0
		diskDirectory[filenameLocation] = 0x00;
		// Write the map and directory back with new values
		writeSector(diskMap, 1);
		writeSector(diskDirectory, 2);
		return 1;
	}
	return -1;
}

/** Write a file with fname from the buffer to the number of sectors
 Return -1 if there is no disk directory entry available, -2 if insufficient sectors, else the number of sectors written
*/
int writeFile(char *fname, char *buffer, int sectors){
	char diskDirectory[512];
	char diskMap[512];
	char writingSector[512];
	int fileEntry = 0;
	int filenameLocation = 0;
	int filenameChars = 0;
	int charMatch = 0;
	int sectorsWritten = 0;
	int writeDestination = -1;
	int emptySector = -1;
	int fill = 0;
	int byteIterator = 0;
	// Null filename.
	if (fname[0] == 0) {
		return -1;
	}

	// Read in the disk directory and disk map to a buffer to analyze.
	readSector(diskDirectory, 2);
	readSector(diskMap, 1);

	// Search for filenames in the disk directory. Only 16 files possible.
	for (fileEntry = 0; fileEntry < 16; fileEntry++) {

		// Each file entry is 32 bytes apart in the disk directory
		filenameLocation = fileEntry * 32;
		// If this is a vacant directory space save it
		if (diskDirectory[filenameLocation] == 0x00) {
			writeDestination = filenameLocation;
		}
		// Check the filename in the directory against the specified filename
		for (filenameChars = 0; filenameChars < 6; filenameChars++) {
			// Compare each character in the two filenames one at a time...
			if (fname[filenameChars] == diskDirectory[filenameLocation + filenameChars]) {

				// Some filenames may be <6 characters. If both filenames terminate prematurely
				// with no differences, then it's a match. Terminate the loop.
				if (fname[filenameChars] == 0) {
					charMatch = 6;			// match found
					filenameChars = 6;		// terminate inner loop
				}
				// Count the number of matches.
				else {
					charMatch++;
				}
			}
			// Any mismatched character means this is not the right filename.
			else {
				charMatch = 0;				// match not found
				filenameChars = 6;			// terminate inner loop
			}
		}

		// filename matched! terminate outer loop
		if (charMatch == 6) {
			fileEntry = 16;
		}
	}

	// If filename found
	if (charMatch == 6) {
		deleteFile(fname);
		writeDestination = filenameLocation;
	}
	// Catch if the directory is full
	if (writeDestination == -1) {
		return -1;
	}
	// Write the name of the file to the directory
	for (sectorsWritten = 0; sectorsWritten < 6; sectorsWritten++) {
		// If we reached the end of the filename before filling directory
		if (fname[sectorsWritten] == 0x00 || fill == 1) {
			fill = 1;
			diskDirectory[writeDestination + sectorsWritten] = 0x00;
		} else {
			diskDirectory[writeDestination + sectorsWritten] = fname[sectorsWritten];
		}
	}
	// Write the file contents to the sectors
	for (sectorsWritten = 0; sectorsWritten < sectors && sectorsWritten < 26; sectorsWritten++) {
			// Check the disk map for an empty sector
			for (byteIterator = 0; byteIterator < 512; byteIterator++) {
				if (diskMap[byteIterator] == 0x00) {
					emptySector = byteIterator;
					byteIterator = 512;
				}
			}
			// If we cant find a viable sector
			if (emptySector == -1)
			{
				return -2;
			}
			// Write the new sector to the directory
			diskDirectory[writeDestination + sectorsWritten + 6] = emptySector;
			for (byteIterator = 0; byteIterator < 512; byteIterator++) {
				writingSector[byteIterator] = buffer[(sectorsWritten*512)+byteIterator];
			}
			// once the buffer has content we write it to the sector
			writeSector(writingSector, emptySector);
			// Update disk map!
			diskMap[emptySector] = 0xFF;
			emptySector = -1;
		}
	// Save the new disk map and directory
	writeSector(diskMap, 1);
	writeSector(diskDirectory, 2);
	return sectorsWritten;
}

/** Processing for DIR command in Shell 
*/
int directory(char *buf) {
	char diskDirectory[512];
	int fileEntry = 0;
	int filenameLocation = 0;
	int filenameChars = 0;
	int iterator = 0;
	readSector(diskDirectory, 2);
	for (fileEntry = 0; fileEntry < 16; fileEntry++) {
		// Each file entry is 32 bytes apart in the disk directory
		filenameLocation = fileEntry * 32;
		if (diskDirectory[filenameLocation] != 0x00) {
			// Iterate through the filename
			for (filenameChars = 0; filenameChars < 6; filenameChars++) {
				// Add the names to the buffer, if we hit the end of the name break
				if (diskDirectory[filenameLocation + filenameChars] == 0x00) {
					filenameChars = 6;
				// If there's more to add then add it to the buffer
				} else {
					buf[iterator] = diskDirectory[filenameLocation + filenameChars];
					iterator++;
				}
			}
			// If the name is finished then append newlines and returns
			buf[iterator] = '\r';
			iterator++;
			buf[iterator] = '\n';
			iterator++;
		}
	}
	// 0 it out so we can run multiple times
	while (iterator < 512) {
		buf[iterator] = 0x00;
		iterator++;
	}
	return 1;
}


/** Interrupt handler for interrupt 21.
*/
int handleInterrupt21(int ax, int bx, int cx, int dx) {
	int returnValue = 1;
	if (ax == 0x00) {
		returnValue = printString((char*)bx);
	}
	else if (ax == 0x11) {
		*((char*)bx) = (char)readChar();
	}
	else if (ax == 0x01) {
		returnValue = readString((char*)bx);
	}
	else if (ax == 0x03) {
		returnValue = readFile((char*)bx, (char*)cx);
	}
	else if (ax == 0x04) {
		returnValue = executeProgram((char*)bx, (char*)cx);
	}
	else if (ax == 0x05) {
		terminate();
	}
	else if (ax == 0x07) {
		returnValue = deleteFile((char*)bx);
	}
	else if (ax == 0x08) {
		returnValue = writeFile((char*)bx, (char*)cx, dx);
	}
	else if (ax == 0x09) {
		returnValue = directory((char*)bx);
	}
	return returnValue;
}
