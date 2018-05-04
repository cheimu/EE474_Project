#include "structures.h" 

void put(char val, int size) {
  inpulseBuffer[bufHead] = val;

  if (lead==1) {
    if (bufHead + 1 >= size) {
      lead = 0;
      bufHead = 0;
      if (bufTail == 0) {
	bufTail = 1;
	lead = 0;
      }
    } else {
      bufHead += 1;
    }
  } else {
    if (bufHead + 1 == bufTail) {
      if ((bufTail+1)== size) { 
        bufTail=0;
      } else {
        bufTail+=1;
      }
      bufHead+=1;
    } else {
      bufHead+=1;
    }
  }
}

char get(int size){
  if (lead==1) {
    if (bufTail==bufHead) { // empty
      return -1;
    } else {
      return inpulseBuffer[bufTail];
    }
  } else {
    if ((bufTail)>=size) {
      if (bufHead == 0){
	return -1; //empty
      } else {
	bufTail = 0;
	lead=1;
	return inpulseBuffer[bufTail];
      }
    } else {
      return inpulseBuffer[bufTail];
    }
  }
  bufTail+=1;
}



