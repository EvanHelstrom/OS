// Evan Quist
int checkMatch(char *filename, char *buffer, int subarr);
void split(char *buffer, char *command, char *arg);

void main() {
	char buffer[512];
	char command[512];
	char arg[512];
	char file[13312];
	int ret = 9;
	int x = 0;
	while (1) {
		print_String("\r\nShell> ");
		read_String(buffer);
		print_String("\n\r");
		split(buffer, command, arg); 			// Split the command and arg
		if(checkMatch(command,"type") == 0){ 		// If its the type command
			ret = read_File(arg, file);		// Read the file!

			if(ret == -1){ 				// Returns error if no file!
				print_String("Incorrect File Name.");
			} else { 				// Prints out file!
				print_String(file);
			}
		} else if(checkMatch(command, "execute") == 0){ // If the command is execute
			ret = execute_Program(arg, 0x2000);	// Runs at 0x2000
		} else {					// Command not found
			print_String("Unrecognized command.");
		}
	}
}
int checkMatch(char *reference, char *command) {
	int i = 0;
	while(reference[i] != 0x00){ 				// If there is more to the string
		if(command[i] == 0x00 && reference[i] == 0x00){ // If they end at the same time its a match
			return 0;
		}
		if(command[i] != reference[i]){ 		// If they mistmatch then deny
			return -1;
		} else {
			i++;
		}
	}
	return 0; 						// If we limit out, which wont happen here, its a match. Just a catch all really
}
void split(char *buffer, char *command, char *arg) {
	int x = 0;
	int y = 0;
	while(buffer[x] != ' '){ 				// Read until the space
		command[x] = buffer[x]; 			// Save everything before space to command
		x++;
	}
	x++; 							// Skip the space
	while(buffer[x] != 0x00){ 				// Read after the space
		arg[y] = buffer[x]; 				// Save it all to arg
		x++;
		y++;
	}
}
