/* Peripheral */
#include "led.h"
#include "gpio.h"
#include <button.h>
#include <unistd.h>
#include <spi_slave.h>
#include "remote_syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 20 // 5 int arguments
#define PIN 0 
char rbuf[BUF_SIZE];
char wbuf[BUF_SIZE];
void debug_buffer(char* buf);

void debug_buffer(char* buf) {
  if (sizeof(buf) == 0) {
    printf("Buffer is empty!\n");
    return;
  }
  for (unsigned int i = 0; i < (sizeof(buf)/sizeof(buf[0])); i++) {
    printf("Buffer element %d: %d\n", i, buf[i]);
  }
}

// Note that this assumes the behavior of the controller is as follows: 
// The controller passes us a buffer with increasing i values, and on the next operation, will
// pass us back the buffer we sent it. This is implemented in the
// spi_master_transfer example.
static void rsyscall_cb(__attribute__ ((unused)) int arg0,
                        __attribute__ ((unused)) int arg2,
                        __attribute__ ((unused)) int arg3,
                        __attribute__ ((unused)) void* userdata) {
  printf("Read System Call!\n");
  debug_buffer(rbuf);

  int spi_info[5];
  memcpy(spi_info, rbuf, sizeof(rbuf));
  execute_system_call(spi_info);


  // TODO Currently sending dummy values back to peripheral
  wbuf[0] = 1;
  spi_slave_read_write(wbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
  
  //TODO Not sure if GPIO is actually needed here
  printf("Toggle the GPIO!\n");
  gpio_toggle(PIN);
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
  if (gpio_count() >= 1) {
    gpio_enable_output(PIN);
  }
}
