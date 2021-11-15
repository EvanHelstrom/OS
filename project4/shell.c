// P4 shell program
// Built on P3 shell by Brian Law
// Author: Evan Quist

void main() {
	char command[256];		// Hopefully users won't enter commands >255 character
	char filename[6];
	char fileBuffer[13312];
	int status = 0;
	int iterator = 0;

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
		// If user command is delete ______
		else if (command[0] == 'd' && command[1] == 'e' && command[2] == 'l' && command[3] == 'e'
			  && command[4] == 't' && command[5] == 'e' && (command[6] == ' ' || command[6] == 0)) {
		
			// Block off null filenames
			if (command[7] == 0) {
				status = -1;
			}
			// Try to delete the specified file
			else {
				status = deleteFile(command + 7);
			}

			// If program errors, print out error message
			if (status == -1) {
				print("Error: File not found\n\r");
			}

		}
		// If user command is copy ______
		else if (command[0] == 'c' && command[1] == 'o' && command[2] == 'p' && command[3] == 'y'
			  && (command[4] == ' ' || command[4] == 0)) {
		
			// Block off null filenames
			if (command[5] == 0) {
				status = -3;
			}
			// Seperate First file name
			else {
				while (command[iterator+5] != 0x00 && command[iterator+5] != ' ') {
					filename[iterator] = command[iterator+5];
					iterator++;
				}
				status = readSingleFile(filename, fileBuffer);
				// Block if the file isn't found
				if (status == -1) {
					print("Error: File not found\n\r");
				}
				// Copy the file
				else {
					status = writeFile(command+iterator+6, fileBuffer, status);
					// Catch errors from Write
					if (status == -1) {
						print("Error: Disk directory is full\n\r");
					}
					else if (status == -2) {
						print("Error: Disk is full\n\r");
					}
				}
			}
			// If program errors, print out error message
			if (status == -3) {
				print("Error: File cannot be null\n\r");
			}

		}
		// If user command is dir
		else if (command[0] == 'd' && command[1] == 'i' && command[2] == 'r' && (command[3] == ' ' || command[3] == 0)) {
			// Try to run dir
			status = directory(fileBuffer);
			// Output the result
			print(fileBuffer);
		}
		// Invalid command.
		else {
			print("Unrecognized command\n\r");
		}
	}
}
