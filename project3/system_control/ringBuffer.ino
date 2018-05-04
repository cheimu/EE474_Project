#include "structures.h"

void put(char val, int size){
  if ((bufHead + 1) < size) {
    bufHead += 1;   
    pulseBuffer[bufHead] = val; 
  } else {
    int i;
    for (i = 0; i < size - 1; i++) {
      pulseBuffer[i] = pulseBuffer[i+1];
    }
    pulseBuffer[size-1] = val;
  }
}
