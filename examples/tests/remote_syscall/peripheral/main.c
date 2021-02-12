/* Peripheral */
#include "led.h"
#include <button.h>
#include <unistd.h>
#include <spi_slave.h>
#include "remote_syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 20 // 5 int arguments
char rbuf[BUF_SIZE];
char wbuf[BUF_SIZE];

// Note that this assumes the behavior of the controller is as follows: 
// The controller passes us a buffer with increasing i values, and on the next operation, will
// pass us back the buffer we sent it. This is implemented in the
// spi_master_transfer example.
static void rsyscall_cb(__attribute__ ((unused)) int arg0,
                        __attribute__ ((unused)) int arg2,
                        __attribute__ ((unused)) int arg3,
                        __attribute__ ((unused)) void* userdata) {
  printf("IN THE READ SYSTEM CALL INTERRUPT!\n");
  int spi_info[5];
  memcpy(spi_info, rbuf, sizeof(rbuf));
  for (int i = 0; i < BUF_SIZE; i++) {
    printf("rbuf: %d\n", rbuf[i]);
  }
  
  //This switch statement uses the first entry of the array to determine what 
  //  system call is being invoked. Note that at this moment, all testing is
  //  being done around the command system call (case 2). The other system calls
  //  have not been robustly tested and should not be used at this time 
  //  but will be ready for use soon.
  /*switch(spi_info[0]) {
    //Subscribe
    case 1: syscallOne(spi_info[1], spi_info[2], (void*)spi_info[3], (void*)spi_info[4]);
            break;
    //Command
    case 2: {
		int ret = syscallTwo(spi_info[1], spi_info[2], spi_info[3], spi_info[4]);
		printf("Command returns: %d\n", ret);
                break;
	      }
    //Allow
    case 3: syscallThree(spi_info[1], spi_info[2], spi_info[3], (void*)spi_info[4]);
              break;
    //Read-only allow
    case 4: syscallFour(spi_info[1], spi_info[2], spi_info[3], (void*)spi_info[4]);
              break;
    //Memop
    case 5: syscallFive(spi_info[1], spi_info[2]);
              break;
    default: printf("Not supported!\n");
             break;
  }*/
  spi_slave_read_write(wbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
}

//Callback to indicate the peripheral has been selected
static void selected_cb(__attribute__ ((unused)) int arg0,
                        __attribute__ ((unused)) int arg2,
                        __attribute__ ((unused)) int arg3,
                        __attribute__ ((unused)) void* userdata) {
  printf("In subscribe callback\n");
}

// This function first initializes the write buffer to all zeroes. We
// then wait until the master begins the transfer, and we then switch
// buffers, so that the data the master sends is passed between the
// master and the slave. Further, after we receive the buffer with data, we
// check to make sure we received the correct values. If not, we enable the LED
// and never disable it.
int main(void) {
  printf("Start of the Spi peripheral!\n");
  spi_slave_set_polarity(false);
  spi_slave_set_phase(false);
  int err = spi_slave_read_write(wbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
  if (err < 0) {
    printf("Error: spi_slave_read_write: %d\n", err);
  }
  spi_slave_chip_selected(selected_cb, NULL);
  printf("Spi peripheral set up\n");
}
