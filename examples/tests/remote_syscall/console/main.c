#include "remote_syscall.h"
#include <console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct SpiPeripheralReceive {
  char syscallNumber;
  int argsOne;
  int argsTwo;
  size_t argsThree;
  size_t argsFour;
};
int buf_len = sizeof(char) + sizeof(int) + sizeof(int) + sizeof(int);
char buf[sizeof(struct SpiPeripheralReceive) + 1];

static void rsyscall_cb(int result __attribute__ ((unused)),
                       int len __attribute__ ((unused)),
                       int _z __attribute__ ((unused)),
                       void* ud __attribute__ ((unused))) {
  printf("\n\nRemote Syscall: \n");
  char syscallNumber = buf[0];
  printf("Syscall Type: %c\n", syscallNumber);

  char argOne[sizeof(int) + 1];
  char argTwo[sizeof(int) + 1];
  char argThree[sizeof(int) + 1];

  for (unsigned int i = 0; i < sizeof(int); i++) {
    argOne[i] = buf[sizeof(char) + i];
  }
  argOne[sizeof(argOne) - 1] = '\0';
  printf("ArgOne: %d\n", atoi(argOne));

  for (unsigned int i = 0; i < sizeof(int); i++) {
    argTwo[i] = buf[sizeof(char) + sizeof(int) + i];
  }
  argTwo[sizeof(argTwo) - 1] = '\0';
  printf("ArgTwo: %d\n", atoi(argTwo));

  for (unsigned int i = 0; i < sizeof(int); i++) {
    argThree[i] = buf[sizeof(char) + sizeof(int) + sizeof(int) + i];
  }
  argThree[sizeof(argThree) - 1] = '\0';
  printf("ArgThree: %d\n", atoi(argThree));
  
  printf("End of the callback!");
  /*size_t argThree = *(size_t*)(buf + sizeof(char) + sizeof(int) + sizeof(int));
  printf("ArgThree: %d\n", argThree);
  size_t argFour = *(size_t*)(buf + sizeof(char) + sizeof(int) + sizeof(int) + sizeof(size_t));
  printf("ArgFour: %d\n", argFour);
  switch(syscallNumber) {
    //Yield
    case '0': syscallZero();
	    break;*/
    //Subscribe
    /*case '1': syscallOne(argOne, argTwo, argThree, argFour);
	    break;
    //Command
    case '2': syscallTwo(argOne, argTwo, argThree, argFour);
	      break;
    //Allow
    case '3': syscallThree(argOne, argTwo, argThree, argFour);
	      break;
    //Read-only allow
    case '4': syscallFour(argOne, argTwo, argThree, argFour);
	      break;
    //Memop
    case '5': syscallFive(argOne, argTwo);
	      break;
    default: printf("This syscall is currently not supported!\n");
	     break;
  }*/
}

int main(void) {
  int ret = subscribe_to_caller(rsyscall_cb, buf, buf_len);
  if (ret != TOCK_SUCCESS) {
    printf("[SHORT] Error doing UART receive: %i\n", ret);
  }
  printf("Prepped!\n");
  return 0;
}

