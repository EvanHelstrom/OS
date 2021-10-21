// Evan Quist
void main() {
interrupt(0x21,0,"WooHoo! I'm not a user program!\r\n",0,0);
interrupt(0x21, 0x05, 0, 0, 0);
}

