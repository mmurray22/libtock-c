#include "remote_syscall.h"
#include <console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char buf[100];
const size_t len = 11;

static void remotesys_callback(int syscallNumber __attribute__ ((unused)),
                               int argsOne,
                               int argsTwo __attribute__ ((unused)),
                               void *argsLeft __attribute__ ((unused))) {
  printf("Callback activated!");  
  /*Carry out Caller instructions*/
  switch(syscallNumber) {
    /*Yield*/
    case 0: syscallZero();
	    break;
    /*Subscribe*/
    case 1: syscallOne(argsOne, argsTwo, *(size_t*)(argsLeft), *((subscribe_cb*)((size_t*)argsLeft + 1)));
	    break;
    /*Command*/
    case 2: if (argsLeft != NULL) {
	      syscallTwo(argsOne, argsTwo, *(size_t*)(argsLeft), *((size_t*)((size_t*)argsLeft + 1)));
            }
	    break;
    /*Allow*/
    case 3: if (argsLeft != NULL) {
	      syscallThree(argsOne, argsTwo, *(size_t*)(argsLeft), *((char**)((size_t*)argsLeft + 1)));
            }
	    break;
    /*Read-only allow*/
    case 4: if (argsLeft != NULL) {
	      syscallFour(argsOne, argsTwo, *(size_t*)(argsLeft), *((char**)((size_t*)argsLeft + 1)));
            }
	    break;
    /*Memop*/
    case 5: syscallFive(argsOne, argsTwo); 
	    break;
    default:  printf("This syscall is currently not supported!");
	      break;
  }
  printf("Completed callback!");
}

/*
 * First iteration of main:
 * -One large infinite while loop for the connection
 */
int main(void) {
  /*Read console_recv_small*/
  int ret = getnstr_async(buf, 11, remotesys_callback, NULL);
  if (ret != TOCK_SUCCESS) {
    printf("[SHORT] Error doing UART receive: %i\n", ret);
  }
  printf("Prepped!\n");
  return 0;
}
