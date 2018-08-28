#include "structures.h"

void put_data(char val, int size_i, ringBuffer* buf) {
  buf->inpulseBuffer[buf->bufHead] = val;
  if (buf->lead==1) {
    if (buf->bufHead + 1 >= size_i) {
      buf->lead = 0;
      buf->bufHead = 0;
      if (buf->bufTail == 0) {
	      buf->bufTail = 1;
	      buf->lead = 0;
      }
    } else {
      buf->bufHead += 1;
    }
  } else {
    if (buf->bufHead + 1 == buf->bufTail) {
      if ((buf->bufTail+1)== size_i) {
        buf->bufTail=0;
      } else {
        buf->bufTail+=1;
      }
      buf->bufHead+=1;
     } else {
      buf->bufHead+=1;
     }
  }
}

char get_data(int size_i, ringBuffer* buf){
  if (buf->lead==1) {
    if (buf->bufTail==buf->bufHead) { // empty
      return -1;
    } else {
      return buf->inpulseBuffer[buf->bufTail];
    }
  } else {
    if ((buf->bufTail)>=size_i) {
      if (buf->bufHead == 0){
	       return -1; //empty
      } else {
	buf->bufTail = 0;
	buf->lead=1;
	return buf->inpulseBuffer[buf->bufTail];
      }
    } else {
      return buf->inpulseBuffer[buf->bufTail];
    }
  }
  buf->bufTail+=1;
}
