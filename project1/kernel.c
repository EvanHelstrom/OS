/* Evan Quist, OS, 9/6/2021 */
// Writes a single Char to the screen
int putChar (char symbol, char color, int row, int col);
// Writes a full string, builds on putChar
int putStr (char *str, char color, int row, int col);

int main () {
	char disp; // Color to display
	char* helloWorld;
	char* myName;
	char* demo;
	helloWorld = "Hello World"; // Hello World in middle
	myName = "Evan Quist"; // Write name to top
	demo = "this is a demo of what \nsuccess is"; 
	// Should wrap and test the newline arg
	disp = 53; 
	// Assign color for general display
	putStr(myName, 7, 3, 0);
	putStr(helloWorld, 53, 18, 34);
	putStr(demo, disp, 20, 78);
	while(disp == disp){
	// Loop forever!
	};
	return 0;
}
int putChar (char symbol, char color, int row, int col){
	int line, offset, mult, result, temp;
	line = 80; // How many chars are in one line
	offset = 1; // We start counting at 0 :)
	mult = 2; // 2 data points for each car
	temp = (row-offset)*line; // Calculate real vert offset
	result = (temp*mult) + (col*mult) + 0x8000; 
	// Find actual position by adding base address and column
	putInMemory(0xB000, result, symbol);
	// Write the char
	putInMemory(0xB000, result+1, color);
	// Write its color
	return 0;
}
int putStr (char *str, char color, int row, int col){
	int count, temp;
	count = 0; // For iterating the array
	while(0 == 0){ //Forever
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
