// Project 4 shell.c solutions
// Brian Law

void main() {
	while (1) {
		char command[256];
		char fileBuffer[13312];
		int status = 0;
		int i;

		while (1) {
			// Shell prompt
			print("Shell>");

			// Zero out the command buffer, then read in user's command.
			for (i=0; i<256; i++) {
				command[i] = 0;
			}
			readSingleString(command);
			print("\n\r");

			// Type command
			if (command[0] == 't' && command[1] == 'y' && command[2] == 'p' && command[3] == 'e' 
			 && (command[4] == ' ' || command[4] == 0)) {
				
				// Filename missing
				if (command[5] == 0) {
					status = -1;
				}
				// Try to read in the file
				else {
					status = readSingleFile(command + 5, fileBuffer);
				}

				// If file could not be read, print error message
				if (status == -1) {
					print("Error: File not found\n\r");
				}
				// Otherwise, print out file contents to screen.
				else {
					print(fileBuffer);
				}
			}
			// Execute command
			else if (command[0] == 'e' && command[1] == 'x' && command[2] == 'e' && command[3] == 'c' 
			      && command[4] == 'u' && command[5] == 't' && command[6] == 'e' && 
			        (command[7] == ' ' || command[7] == 0)) {

				// Filename missing
				if (command[8] == 0) {
					status = -1;
				}
				// Try to execute the (hopefully) program.
				else {
					status = executeProgram(command + 8, 0x2000);
				}

				// Error codes
				if (status == -1) {
					print("Error: Program not found\n\r");
				}
				else if (status == -2) {
					print("Error: segment invalid\n\r");
				}
			}
			// Delete command
			else if (command[0] == 'd' && command[1] == 'e' && command[2] == 'l' && command[3] == 'e' 
			      && command[4] == 't' && command[5] == 'e' && 
			        (command[6] == ' ' || command[6] == 0)) {

				// Filename missing.
				if (command[6] == 0) {
					status = -1;
				}
				// Try to delete the file.
				else {
					status = deleteSingleFile(command + 7);
				}

				// Error code
				if (status == -1) {
					print("Error: File not found\n\r");
				}
			}

			// Copy command - for the record, this nested if/else structure is terrible, but I'm trying to
			// keep the code here to simple, efficient constructs for your ease of understanding.
			else if (command[0] == 'c' && command[1] == 'o' && command[2] == 'p' && command[3] == 'y' 
			     && (command[4] == ' ' || command[4] == 0)) {

				// No files specified.
				if (command[5] == 0) {
					print("Error: File not found\n\r");
				}
				else {
					// Move i to where first filename starts, then find where space is,
					// and after that should be where the 2nd filename begins.
					for (i=5; command[i] != ' ' && command[i] != 0 && i < 18; i++) {
					}

					// I said SHOULD be above.
					if (command[i] == 0) {
						print("Error: Destination file not provided\n\r");
					}
					// Once confirmed that both filenames provided, begin copy procedure.
					else {
						command[i] = 0;
						// Try to read in the first file to the buffer.
						status = readSingleFile(command + 5, fileBuffer);

						if (status == -1) {
							print("Error: File not found\n\r");
						}
						// If file read successfully, write the contents to 2nd filename.
						else {
							status = writeSingleFile(command + i + 1, fileBuffer, status);

							if (status == -1) {
								print("Error: Disk directory full\n\r");
							}
							else if (status == -2) {
								print("Error: Disk is full\n\r");
							}
						}
					}
				}
			}
			// dir command
			else if (command[0] == 'd' && command[1] == 'i' && command[2] == 'r' && command[4] == 0) {
				listDirectory(fileBuffer);
				print(fileBuffer);
				print("\n\r");
			}
			// Invalid command
			else {
				print("Unrecognized command\n\r");
			}
		}	
	}
}
