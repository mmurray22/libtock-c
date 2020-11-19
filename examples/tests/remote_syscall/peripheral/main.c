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

// Note that this assumes the behavior of the master; that it passes us
// a buffer with increasing i values, and on the next operation, will
// pass us back the buffer we sent it. This is implemented in the
// spi_master_transfer example.
static void rsyscall_cb(__attribute__ ((unused)) int arg0,
                        __attribute__ ((unused)) int arg2,
                        __attribute__ ((unused)) int arg3,
                        __attribute__ ((unused)) void* userdata) {
  printf("RSYSCALL ACTIVATED!");
  int spi_info[5];
  memcpy(spi_info, rbuf, sizeof(rbuf));
  printf("Syscall number: %d\n", spi_info[0]);
  printf("Arg0: %d\n", spi_info[1]);
  printf("Arg1: %d\n", spi_info[2]);
  printf("Arg2: %d\n", spi_info[3]);
  printf("Arg3: %d\n", spi_info[4]);
  switch(spi_info[0]) {
    //Yield
    case 0: syscallZero();
            break;
    //Subscribe
    /*case 1: syscallOne(spi_info[1], spi_info[2], spi_info[3], spi_info[4]);
            break;*/
    //Command
    case 2: {
		int ret = syscallTwo(spi_info[1], spi_info[2], spi_info[3], spi_info[4]);
		printf("Command returns: %d\n", ret);
                break;
	      }
    //Allow
    /*case 3: syscallThree(spi_info[1], spi_info[2], spi_info[3], spi_info[4]);
              break;
    //Read-only allow
    case 4: syscallFour(spi_info[1], spi_info[2], spi_info[3], spi_info[4]);
              break;*/
    //Memop
    case 5: syscallFive(spi_info[1], spi_info[2]);
              break;
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
  spi_slave_set_polarity(false);
  spi_slave_set_phase(false);
  int err = spi_slave_read_write(wbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
  if (err < 0) {
    printf("Error: spi_slave_read_write: %d\n", err);
  }
  spi_slave_chip_selected(selected_cb, NULL);
  printf("Spi peripheral set up\n");
}
