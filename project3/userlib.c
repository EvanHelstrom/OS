// Evan Quist
int print_String(char *str) {
	int returnValue;
	returnValue = interrupt(0x21,0x00,str,0,0);
	return returnValue;
}
int read_Char(){
	int returnValue;
	returnValue = interrupt(0x21,0x011,0,0,0);
	return returnValue;
}
int read_String(char *buf){
	int returnValue;
	returnValue = interrupt(0x21,0x01,buf,0,0);
	return returnValue;
}
int read_File(char *filename, char *buf){
	int returnValue;
	returnValue = interrupt(0x21,0x03,filename,buf);
	return returnValue;
}
int execute_Program(char* name, int segment){
	int returnValue;
	returnValue = interrupt(0x21,0x04,name,segment);
	return returnValue;
}
void terminate_Program(){
	interrupt(0x21,0x05,0,0,0);
}
