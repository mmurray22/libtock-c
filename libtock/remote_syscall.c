#include "remote_syscall.h"
#include <spi.h>
#include <console.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
/* DriverArray Functions */

void initDriverArray(DriverArray* da, size_t initialSize) {
    da->arr = malloc(initialSize * sizeof(char));
    assert(da->arr);
    da->used = 0;
    da->size = initialSize;
}

void insertBuffer(DriverArray* da, char* buf, size_t buf_len) {
    if ((da->used + buf_len) >= da->size) {
      da->size *= 2;
      da->arr = realloc(da->arr, da->size * sizeof(char));
      assert(da->arr);
    }
    da->arr[da->used] = buf;
    da->used += buf_len;
}

void removeBuffer(DriverArray* da, int index) {
    /* TODO: Don't know if I need this */
    (void) da;
    (void) index;
}

void freeDriverArray(DriverArray* da) {
    free(da->arr);
    da->arr = NULL;
    da->used = 0;
    da->size = 0;
}
  
/* AllowArray Functions */

void initAllowArray(AllowArray* aa, size_t initialSize) {
    aa->arr = malloc(initialSize * sizeof(DriverArray));
    assert(aa->arr);
    aa->used = 0;
    aa->size = initialSize;
}

void insertDriverArray(AllowArray* aa, DriverArray da) {
    if (aa->used == aa->size) {
      aa->size *= 2;
      aa->arr = realloc(aa->arr, aa->size * sizeof(DriverArray));
      assert(aa->arr);
    }       
    aa->arr[aa->used] = da;
    aa->used++;

}

void removeDriverArray(AllowArray* aa, DriverArray da) {
    /* TODO: Don't know if I need this */
    (void) aa;
    (void) da;
}

void freeAllowArray(AllowArray* aa) {
    free(aa->arr);
    aa->arr = NULL;
    aa->used = 0;
    aa->size = 0;
}

/* Peripheral Functions */

int execute_system_call(unsigned int* request,
		        AllowArray* allow_array) {
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
    case 3: syscallThree(request[1], request[2], request[3], 2 /*request[4]*/, allow_array);
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
int syscallThree (size_t driverNum, 
		  size_t subdriverNum, 
		  size_t arg0, 
		  size_t buf_len, 
		  AllowArray* aa) {
  /*Step 1: Malloc a buffer of the appropriate length*/
  char* buffer = malloc(buf_len); //TODO: Get actual length from controller
  assert(buffer);

  /*Step 2: Determine what DriverArray the buffer belongs to*/
  int selected_row = -1;
  for (size_t i = 0; i < aa->used; i++) {
    if (aa->arr[i].driver_num == driverNum) {
      selected_row = i;
      break;
    }
  }

  if (selected_row < 0) {
    DriverArray da;
    initDriverArray(&da, 5); //TODO: MAKE CONSTANT
    insertDriverArray(aa, da);
    selected_row = aa->used - 1;
  }

  /*Step 3: Add buffer to the DriveArray*/
  insertBuffer(&aa->arr[selected_row], buffer, buf_len);

  /*Step 4: Allow the buffer!*/
  int ret = allow(driverNum, subdriverNum, (void*)buffer, arg0);
  return ret;
}

//Read-Only Allow
int syscallFour(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer) {
   /*TODO Merge function with normal allow*/
  (void) driverNum;
  (void) subdriverNum;
  (void) arg0;
  (void) buffer;
  return -1;
}

//Memop
void* syscallFive(size_t operand, size_t arg0) {
  /*TODO Write function */
  (void) operand;
  (void) arg0;
  return NULL;
}
