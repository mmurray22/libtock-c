/* Peripheral */
#include "led.h"
#include <button.h>
#include <unistd.h>
#include <spi_slave.h>
#include "remote_syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 40
char rbuf[BUF_SIZE];
char wbuf[BUF_SIZE];
char ibuf[BUF_SIZE];
char zbuf[BUF_SIZE];
bool toggle = true;

int assemble_arg(int rbufIndex, char *argBuf);

int assemble_arg(int rbufIndex, char *argbuf) {
  int argbufIndex = 0;
  while (rbuf[rbufIndex] != 'z') {
    argbuf[argbufIndex] = rbuf[rbufIndex];
    rbufIndex++;
    argbufIndex++; 
  }
  return rbufIndex + 1;
}

// Note that this assumes the behavior of the master; that it passes us
// a buffer with increasing i values, and on the next operation, will
// pass us back the buffer we sent it. This is implemented in the
// spi_master_transfer example.
static void rsyscall_cb(__attribute__ ((unused)) int arg0,
                     __attribute__ ((unused)) int arg2,
                     __attribute__ ((unused)) int arg3,
                     __attribute__ ((unused)) void* userdata) {
  char syscallNumber = rbuf[0];
  printf("Remote Syscall: %c\n", syscallNumber);
  int index = 1;
  char argOne[5];
  char argTwo[10];
  char argThree[10];
  char argFour[10];
  index = assemble_arg(index, argOne);
  index = assemble_arg(index, argTwo);
  index = assemble_arg(index, argThree);
  assemble_arg(index, argFour);
  printf("argOne: 0x%s\n", argOne);
  printf("argTwo: %s\n", argTwo);
  printf("argThree: %s\n", argThree);
  printf("argFour: %s\n", argFour);
  switch(syscallNumber) {
    //Yield
    /*case '0': syscallZero();
            break;
    //Subscribe
    case '1': syscallOne(argOne, argTwo, argThree, argFour);
            break;*/
    //Command
    case '2': {
		int ret = syscallTwo(argOne, argTwo, argThree, argFour);
		printf("Command returns: %d\n", ret);
                break;
	      }
    //Allow
    /*case '3': syscallThree(argOne, argTwo, argThree, argFour);
              break;
    //Read-only allow
    case '4': syscallFour(argOne, argTwo, argThree, argFour);
              break;
    //Memop
    case '5': syscallFive(argOne, argTwo);
              break;*/
    default: printf("This syscall is currently not supported!\n");
             break;
  }
  spi_slave_read_write(wbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
}

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
  int i;
  for (i = 0; i < BUF_SIZE; i++) {
    rbuf[i] = 0;
  }

  spi_slave_set_polarity(false);
  spi_slave_set_phase(false);
  int err = spi_slave_read_write(wbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
  if (err < 0) {
    printf("Error: spi_slave_read_write: %d\n", err);
  }
  printf("Spi peripheral set up\n");
  spi_slave_chip_selected(selected_cb, NULL);
}
