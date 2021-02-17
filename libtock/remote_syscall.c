#include "remote_syscall.h"
#include <spi.h>
#include <console.h>
#include <stdlib.h>
#include <stdio.h>

int subscribe_to_caller(subscribe_cb cb, char* buf, size_t len) {
  int res = getnstr_async(buf, len, cb, NULL);
  return res;
}


int execute_system_call(int* request) {
  //This switch statement uses the first entry of the array to determine what 
  //  system call is being invoked. Note that at this moment, all testing is
  //  being done around the command system call (case 2). The other system calls
  //  have not been robustly tested and should not be used at this time 
  //  but will be ready for use soon.
  switch(request[0]) {
    //Subscribe
    case 1: syscallOne(request[1], request[2], (void*)request[3], (void*)request[4]);
            break;
    //Command
    case 2: {
	      int ret = syscallTwo(request[1], request[2], request[3], request[4]);
              printf("Command returns: %d\n", ret);
              return ret;
            }
    //Allow
    case 3: syscallThree(request[1], request[2], request[3], (void*)request[4]);
            break;
    //Read-only allow
    case 4: syscallFour(request[1], request[2], request[3], (void*)request[4]);
            break;
    //Memop
    case 5: syscallFive(request[1], request[2]);
            break;
    default: printf("Not supported!\n");
             return -1;
  }
  return -1;
}

//Yield
void syscallZero (void) {
  yield();
}

//Subscribe
int syscallOne (size_t driverNum, size_t subscribeNum, void* arg0, subscribe_cb cb) {
  return subscribe(driverNum, subscribeNum, cb, arg0);
}

//Command
int syscallTwo (size_t driverNum, size_t subdriverNum, size_t arg0, size_t arg1) {
  return command(driverNum, subdriverNum, arg0, arg1);
}

//Allow
int syscallThree (size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer) {
  return allow(driverNum, subdriverNum, buffer, arg0);
}

//Read-Only Allow
int syscallFour(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer) {
  return allow(driverNum, subdriverNum, buffer, arg0);
}

//Memop
void* syscallFive(size_t operand, size_t arg0) {
  return memop(operand, arg0);
}
