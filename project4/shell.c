// P3 shell program for our simple OS
// Author: Brian Law

void main() {
	char command[256];		// Hopefully users won't enter commands >255 characters
	char fileBuffer[13312];
	int status = 0;

	// Shell never stops
	while (1) {
		// Shell prompt
		print("Shell>");

		// Read in user command for processing
		readSingleString(command);

		// Printing a newline for appearance
		print("\n\r");

		// Process user command: if it's type _____...
		if (command[0] == 't' && command[1] == 'y' && command[2] == 'p' && command[3] == 'e'
		 && (command[4] == ' ' || command[4] == 0)) {

			// Block off null filenames
			if (command[8] == 0) {
				status = -1;
			}
			// Read in the file requested by the user
			else {
				status = readSingleFile(command + 5, fileBuffer);
			}

			// Print out error message or the file contents if no error.
			if (status == -1) {
				print("Error: File not found\n\r");
			}
			else {
				print(fileBuffer);
			}
		}
		// If user command is execute ________
		else if (command[0] == 'e' && command[1] == 'x' && command[2] == 'e' && command[3] == 'c'
			  && command[4] == 'u' && command[5] == 't' && command[6] == 'e' &&
			    (command[7] == ' ' || command[7] == 0)) {

			// Block off null filenames
			if (command[8] == 0) {
				status = -1;
			}
			// Try to execute the specified program
			else {
				status = executeProgram(command + 8, 0x2000);
			}

			// If program errors, print out error message
			if (status == -1) {
				print("Error: Program not found\n\r");
			}
			else if (status == -2) {
				print("Error: segment invalid\n\r");
			}
		}
		// Invalid command.
		else {
			print("Unrecognized command\n\r");
		}
	}
}