#include "remote_syscall.h"
#include <spi.h>
#include <console.h>
#include <stdlib.h>
#include <stdio.h>

int subscribe_to_caller(subscribe_cb cb, char* buf, size_t len) {
  int res = getnstr_async(buf, len, cb, NULL);
  return res;
}

int execute_system_call(unsigned int* request,
	       		unsigned int driver_rows,
                        unsigned int buf_columns,	
		        uint8_t* allow_buf[driver_rows][buf_columns],
			driver_info* drivers_with_buffers) {
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
    case 3: syscallThree(request[1], request[2], request[3], driver_rows, buf_columns, allow_buf[driver_rows][buf_columns], drivers_with_buffers);
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
		  unsigned int driver_rows, 
		  unsigned int buf_columns, 
		  uint8_t* buffer[driver_rows][buf_columns], 
		  driver_info* drivers_with_buffers) {
  /*Step 1: Determine whether there is room to allow the buffer*/
  int ret = -1;
  /*Step 2: Determine what row and buffer number the buffer will be assigned*/
  int selected_row = -1;
  for (unsigned int i = 0; i < driver_rows; i++) {
    if (drivers_with_buffers[i].driver_num < 0) {
      drivers_with_buffers[i].driver_num = driverNum;
      drivers_with_buffers[i].num_bufs_left = buf_columns;
      selected_row = i;
      break;
    } else if ((unsigned int)drivers_with_buffers[i].driver_num == driverNum) {
      selected_row = i;
      break;
    }
  }
  if (selected_row < 0 || drivers_with_buffers[selected_row].num_bufs_left == 0) {
    return ret; //no more rows or buf slots available
  }
  int selected_buffer_slot = buf_columns - drivers_with_buffers[selected_row].num_bufs_left;
  drivers_with_buffers[selected_row].num_bufs_left -= 1;

  /*Step 3: Allow the buffer!*/
  ret = allow(driverNum, subdriverNum, (void*)buffer[selected_row][selected_buffer_slot], arg0);
  return ret;
}

//Read-Only Allow
int syscallFour(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer) {
  return allow(driverNum, subdriverNum, buffer, arg0);
}

//Memop
void* syscallFive(size_t operand, size_t arg0) {
  return memop(operand, arg0);
}
