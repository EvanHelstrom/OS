// Print the given null-terminated string to screen.
int print(char* s);

// Read a single char into the specified buffer.
int readSingleChar(char* buf);

// Read a null-terminated string of user input from the terminal.
int readSingleString(char* buf);

// Read the specified file into the fileBuffer. fileBuffer should be a buffer
// of at least 13312 characters to fit file, otherwise buffer overrun may occur.
// Return -1 if file not found, # sectors read if successful.
int readSingleFile(char *filename, char *fileBuffer);

// Execute the specified program, loading it into the specified segment.
// Return -1 if file not found, -2 if invalid segment.
int executeProgram(char* filename, int segment);

// Terminate the running program. Resume the shell.
void terminateProgram();

// Delete the specified file from the disk. Return -1 if file not found.
int deleteSingleFile(char* filename);

// Write # sectors of sectors' worth of data from fileBuffer to the specified filename.
// fileBuffer should be at least 512 * sectors chars long; otherwise, write will overrun
// fillBuffer.
// If successful, will return sectors. If disk directory is full, will return -1.
// If disk is full, will write as many sectors as possible, then return -2.
int writeSingleFile(char* filename, char* fileBuffer, int sectors);

// System call for retrieving the contents of the disk. contentsBuffer should
// be a buffer of at least 127 characters.
void listDirectory(char* contentsBuffer);

// System call for killing a process at a specified segment
// Return -1 if failure, 1 if success
int killProcess(int segment);

// System call for Yielding time
void yield();

// Lists currently running processes
void showProcesses();

