/* Peripheral */
#include "led.h"
#include "gpio.h"
#include <button.h>
#include <unistd.h>
#include <timer.h>
#include <spi_slave.h>
#include "remote_syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ARGS 5
#define BASE_SIZE 5 // 5 drivers' allows are supported initially
#define BUF_SIZE 21 // 5 int arguments + 1 char checksum
#define PIN 0 
char rbuf[BUF_SIZE];
char wbuf[BUF_SIZE];
char zbuf[BUF_SIZE];
AllowArray allow_array;
bool toggle = true;

void debug_buffer(void);
void convert_chars_to_int(unsigned int* spi_info, unsigned int start);
void get_int_array(unsigned int* spi_info);
void put_int_in_wbuf(int ret);
bool verify_checksum(void);
void send_response(int ret);

void debug_buffer(void) {
  if (sizeof(wbuf) == 0) {
    printf("Buffer is empty!\n");
    return;
  }
  for (unsigned int i = 0; i < BUF_SIZE; i++) {
    printf("Buffer element %d: %d\n", i, wbuf[i]);
  }
}

void convert_chars_to_int(unsigned int* spi_info, unsigned int start) {
  unsigned int elt = 0;
  for (unsigned int i = start; i < start + 4; i++) {
    int part = (int)rbuf[i];//strtol(str, &end, 10);
    part <<= (3 - i%4);
    elt += part;
  }
  spi_info[start/4] = elt;  
}

void get_int_array(unsigned int* spi_info) {
  for (unsigned int i = 0; i < sizeof(rbuf); i += 4) {
    convert_chars_to_int(spi_info, i);
  }
}

bool verify_checksum(void) {
  unsigned int y = 0;
  for (int i = 0; i < BUF_SIZE; i++) {
    y |= rbuf[i];
  }
  printf("Our checksum: %d vs. their checksum %d\n", y, rbuf[BUF_SIZE - 1]);
  return y == rbuf[BUF_SIZE - 1];
}

void put_int_in_wbuf(int ret) {
  if (sizeof(wbuf) < 4) {
    printf("wbuf is not long enough!\n");
    return;
  }
  for(int i = 3; i >= 0; i--) {
    wbuf[i] = ret%10;
    ret /= 10;
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
  int ret = 0;
  if (!toggle) {
    send_response(ret);
    return;
  }
  if (!verify_checksum()) {
    printf("Unable to verify the checksum!\n");
    /*TODO Need to do something special*/
    return;
  }

  unsigned int spi_info[NUM_ARGS];
  get_int_array(spi_info);
  for (int i = 0; i < 5; i++) {
    printf("spi_info: %d\n", spi_info[i]);
  }

  ret = execute_system_call(spi_info, &allow_array);
  put_int_in_wbuf(ret);
  send_response(ret);
}

void send_response(int ret) {
  if (ret == 0) {
    spi_slave_read_write(zbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
  } else {
    spi_slave_read_write(wbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
  }
  gpio_toggle(PIN);
  gpio_toggle(PIN);
  toggle = !toggle;
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
  int err = spi_slave_read_write(zbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
  if (err < 0) {
    printf("Error: spi_slave_read_write: %d\n", err);
  }
  spi_slave_chip_selected(selected_cb, NULL);
  printf("Spi peripheral set up\n");
  
  if (gpio_count() >= 1) {
    gpio_enable_output(PIN);
  }

  initAllowArray(&allow_array, BASE_SIZE);
}
