#include <stdlib.h>
#include <stdio.h>

int lead = 1;
int bufHead = 0;
int bufTail = 0;
char inpulseBuffer[8];

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

int main(){
  put('A',8);
  put('B',8);
  put('C',8);
  put('D',8);
  put('E',8);
  put('F',8);
  put('G',8);
  put('H',8);
  printf("Result \n");
  printf("Result:  %s  \n", inpulseBuffer);
  char r[]={get(8),get(8),get(8),get(8), get(8), get(8), get(8), get(8)};
  printf("Result:  %s", r);

}
