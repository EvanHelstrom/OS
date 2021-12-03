/*
 *  proc.c
 *
 *
 *  Created by Grant Braught on 3/31/10.
 *  Copyright 2010 Dickinson College. All rights reserved.
 *
 */

#include "proc.h"

/*
 * Initialize the data structures used by the proc system.  Set
 * the running process to be the idle process.
 */
void initializeProcStructures() {
	int i=0;

	idleProc.name[0] = 'I';
	idleProc.name[1] = 'D';
	idleProc.name[2] = 'L';
	idleProc.name[3] = 'E';
	idleProc.name[4] = 0;

	idleProc.state = READY;
	idleProc.segment = 0x0000;
	idleProc.stackPointer = 0x0000;

	running = &idleProc;

	readyHead = NULL;
	readyTail = NULL;
	for(i=0; i<8; i++) {
		pcbPool[i].state = DEFUNCT;
		pcbPool[i].name[0] = 0x00;
		pcbPool[i].segment = 0x00;
		pcbPool[i].stackPointer = 0x00;
		memoryMap[i] = FREE;
	}
}

/*
 * Find a free segment in the memoryMap, mark it as used, and return its index.
 * The segment indexes are 0, 1, ... 7 corresponding to 0x2000, 0x3000...0x9000
 * If memory is full, return -1.
 */
int getFreeMemorySegment() {
	int i;
	for (i=0; i<8; i++) {
		if (memoryMap[i] == FREE) {
			memoryMap[i] = USED;	// mark it as used.
			return i;
		}
	}
	return NO_FREE_SEGMENTS;		// No more segments left!
}

/*
 * Mark the memory segement at the given idex as unused.
 */
void releaseMemorySegment(int segIndex) {
	memoryMap[segIndex] = FREE;
}

/*
 * Find a free PCB to use, set its state to STARTING and return
 * a pointer to it.  Returns NULL if there are no PCBs available.
 */
struct PCB *getFreePCB() {
	int i;
	for (i=0; i<8; i++) {
		if (pcbPool[i].state == DEFUNCT) {
			pcbPool[i].state = STARTING;
			return &pcbPool[i];
		}
	}
	return NULL;		// No more PCBs left!
}

/*
 * Set the PCB as DEFUNCT and set its next and prev pointers
 * to null (defensive programming!).
 */
void releasePCB(struct PCB *pcb) {
	pcb->state = DEFUNCT;
	pcb->next = NULL;
	pcb->prev = NULL;
	pcb->name[0] = 0x00;
}

/*
 * Add the given PCB to the tail of the ready queue.
 */
void addToReady(struct PCB *pcb) {
	if (readyHead == NULL) {	// Empty queue
		readyHead = pcb;
		readyTail = pcb;
		pcb->next = NULL;
		pcb->prev = NULL;
	}
	else {
		readyTail->next = pcb;
		pcb->prev = readyTail;
		pcb->next = NULL;
		readyTail = pcb;
	}
}

/*
 * Remove the PCB at the head of the ready queue and return
 * a pointer to it. Return NULL if there are no PCBs in the
 * ready queue.
 */
struct PCB *removeFromReady() {
	if (readyHead == NULL) {		// empty queue
		return NULL;
	}
	else if (readyHead == readyTail) {		// one entry in queue
		struct PCB *tmp = readyHead;
		readyHead = NULL;
		readyTail = NULL;
		return tmp;
	}
	else {
		struct PCB *tmp = readyHead;
		readyHead = readyHead->next;
		readyHead->prev = NULL;
		return tmp;
	}
}