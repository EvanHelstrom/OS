/* Evan Quist, OS, 9/20/2021 */
// Writes a single Char to the screen
int putChar (char symbol, char color, int row, int col);
// Writes a full string, builds on putChar
int putStr (char *str, char color, int row, int col);
// Displays a full string using interrupts
int printString(char *str);
// Reads a char from Keyboard via BIOS
int readChar();
// Reads an entire String from Keyboard
int readString(char *buf);
// Read or write sectors from a disk
int readSector(char *buf, int absSector);
// Returns the remainder of a division (modulo)
int mod(int num, int div);
// Process action for interrupt 21
int handleInterrupt21(int ax, int bx, int cx, int dx);

int main () {
	
	char line[80];
	char ch[1];
	char buffer[512];
	readSector(buffer, 30);
	makeInterrupt21();

	interrupt(0x21, 0x00, "Prompt for String: ", 0, 0);
	interrupt(0x21, 0x01, line, 0, 0);
	printString("\n\r");
	interrupt(0x21, 0x00, line, 0, 0);
	interrupt(0x21, 0x00, "\n\rPrompt for Char: ", 0, 0);
	interrupt(0x21, 0x11, ch, 0, 0);
	line[0] = ch[0];
	line[1] = 0x00;
	interrupt(0x21, 0x00, line, 0, 0);
	printString("\n\r");
	printString(buffer);
	while(1){
	// Loop forever!
	};
	return 0;
}

/*
Name: putChar
Params: Symbol to be printed, color to print in, row to print to, col to print to
Desc: prints a single character to the screen in a desired location
*/
int putChar (char symbol, char color, int row, int col){
	int line, offset, mult, result, temp;
	line = 80; // How many chars are in one line
	offset = 1; // We start counting at 0 :)
	mult = 2; // 2 data points for each car
	temp = (row-offset)*line; // Calculate real vert offset
	result = (temp * mult) + (col * mult) + 0x8000; 
	// Find actual position by adding base address and column
	putInMemory(0xB000, result, symbol);
	// Write the char
	putInMemory(0xB000, result + 1, color);
	// Write its color
	return 0;
}

/*
Name: putStr
Params: Char array to print, color to print in, row to print to, col to print to
DescL prints an array of letters onto the screen using putChar
*/
int putStr (char *str, char color, int row, int col){
	int count, temp;
	count = 0; // For iterating the array
	while(1){ //Forever
		if(str[count] == 0){
			break; // When array is done, exit
		}
		temp = col+count; // Catch overflow
		if(temp > 80){ // Will go down a row and return
			col = col - 80;
			row = row + 1;
		}
		if(str[count] == '\n'){ // Catch newline
			col = 0; // Resets cursor
			row = row + 1; // Drops a line
		} else { // If its not a return char then write
			putChar(str[count], color, row, col);
			// Writes to the ui using putChar
			col = col + 1;
			// Advances our column
		}
		count = count + 1; // Advances position in array
	}
	return 0;
}

/*
Name: printString
Params: char array of message to display
Desc: prints a char array to the screen using interrupts
*/
int printString(char *str){
	int count;
	count = 0; // For iterating the array
	while(1){ //Forever
		if(str[count] == 0x00){
			break; // When array is done, exit
		} else { // If there is more to array
			char al = str[count]; // A lower value is the char
			char ah = 0x0E;	// A high is simply set to select current cursor location
			int ax = ah * 256 + al; // Create ax hybrid of al and ah
			interrupt(0x10, ax, 0, 0, 0); // Call the interrupt!

		}
		count = count + 1; // Advances position in array
	}
	return count; // How many letters printed?
}

/*
Name: printChar
Params: char to display
Desc: prints a char to the screen using interrupts
*/
int printChar(char str){
	char al = str; // ASCII of char
	char ah = 0x0E; // Print where cursor is
	int ax = ah * 256 + al; // Create AX
	interrupt(0x10, ax, 0, 0, 0); // Call interrupt
	return 0;
}

/*
Name: readChar
Params: None
Desc: Reads a character inputed from the keyboard
*/
int readChar(){
	int result;
	result = interrupt(0x16, 0, 0, 0, 0) & 0xFF; // Call read interrupt and mask upper bits (scan code)
	return result;
}

/*
Name: readString
Params: Buffer to save array to
Desc: Reads a string from the keyboard input
*/
int readString(char *buf){
	char temp;
	int count;
	count = 0;
	while(1){
		temp = readChar(); // Read input
		if(temp == '\r'){ // If it is a return character
			buf[count] = '\0'; // Terminate array
			return count; // How long do it be?
		} else if(temp == 0x08){ // If it is backspace
			if(count > 0){ // If we are at the start, ignore backspace
				count = count - 1; // Go back!
				printChar(temp); // Move cursor back
				buf[count] = '\0'; //Terminate string to replace old char
				printChar(' '); // Replace on screen
				printChar(temp); // Move cursor back again after replacing with space
			}
		} else { // Otherwise the simple case
			buf[count] = temp; // Save what you typed to the array
			printChar(temp); // Print whatever you typed
			count = count + 1; // Advance the count
		}
	}
	return count;
}

/*
Name: readSector
Params: Buffer to read into, absolute sector number to read
Desc: Reads a sector number into the buffer
*/
int readSector(char *buf, int absSector){
	int ax;
	char ah;
	char al;
	int cx;
	char ch;
	char cl;
	int dx;
	char dh;
	char dl;

	ah = 0x02;
	al = 0x01;

	ch = absSector/36; // This is your formula, I don't know how it works
	cl = mod(absSector, 18) + 1;
	dh = mod(absSector/18, 2);
	dl = 0x00;

	ax = (ah * 256) + al; // Create Xs by combining lower and upper bytes
	cx = (ch * 256) + cl;
	dx = (dh * 256) + dl;
	interrupt(0x13, ax, buf, cx, dx); // Call the read sector interrupt
	return 1; // It was a success!
}

/*
Name: Mod
Params: Number to be divided, number to be divided by
Returns: remainder of division
*/
int mod(int num, int div){
	int result;
	int temp;
	temp = num/div; // How many times does div fit into num
	result = num - (temp * div); // Subtract that many times mult by div to find remainder
	if(result < 0){ // If we rounded up on division
		result = num - (temp - 1) *div;	// Round down instead. We love int division
	}
	return result;
}

/*
Name: handleInterrupt21
Params: 4 ints, including an upper and a lower byte for each
Desc: Allows for users apps to call interrupt 21
*/
int handleInterrupt21(int ax, int bx, int cx, int dx){
	if(ax == 0x00){ // printString
		return printString(bx);
	} else if(ax == 0x11){ // readChar
		*((char*)bx) = readChar(); // Modify what is at address
		// Casts bx to a pointer, then * finds what is at address
		return 1;
	}else if(ax == 0x01){ // readString
		return readString(bx);
	}
	return -1; // If all other calls fail return -1
}

