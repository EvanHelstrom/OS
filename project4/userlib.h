/* Header file for user library functions.
Author: Brian Law
*/

int print(char* s);
int readSingleChar(char* buf);
int readSingleString(char* buf);
int readSingleFile(char *filename, char *fileBuffer);
int executeProgram(char* filename, int segment);
void terminateProgram();
