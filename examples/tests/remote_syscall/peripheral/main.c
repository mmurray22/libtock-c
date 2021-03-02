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

#define NUM_ARGS 5
#define MAX_UNIQUE_ALLOW 5
#define MAX_NUM_BUFFERS 5
#define BUF_SIZE 20 // 5 int arguments
#define PIN 0 
char rbuf[BUF_SIZE + 4];
char wbuf[BUF_SIZE + 1];
uint8_t* allow_buf[MAX_UNIQUE_ALLOW][MAX_NUM_BUFFERS];
driver_info drivers_with_buffers[MAX_UNIQUE_ALLOW];

void debug_buffer(void);
void convert_chars_to_int(unsigned int* spi_info, unsigned int start);
void get_int_array(unsigned int* spi_info);
void put_int_in_wbuf(int ret);
bool verify_checksum(unsigned int* spi_info);
void send_response(int ret);

void debug_buffer(void) {
  if (sizeof(rbuf) == 0) {
    printf("Buffer is empty!\n");
    return;
  }
  for (unsigned int i = 0; i < BUF_SIZE; i++) {
    printf("Buffer element %d: %d\n", i, rbuf[i]);
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

bool verify_checksum(unsigned int* spi_info) {
  unsigned int y = 0;
  int len = sizeof(spi_info)/sizeof(spi_info[0]);
  for (int i = 0; i < (len - 1); i++) {
    y &= spi_info[i];
  }
  return y == spi_info[len - 1];
}

void put_int_in_wbuf(int ret) {
  if (sizeof(wbuf) < 4) {
    printf("wbuf is not long enough!\n");
    return;
  }
  wbuf[0] = 1;
  for(int i = 4; i >= 1; i--) {
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
  debug_buffer();

  unsigned int spi_info[NUM_ARGS];
  get_int_array(spi_info);
  if (!verify_checksum(spi_info)) {
    printf("Unable to verify the checksum!\n");
    //return; //TODO: Change 
  }


  /*Print check for spi_info: for (int i = 0; i < 5; i++) {
    printf("spi_info: %d\n", spi_info[i]);
  }*/


  int ret = execute_system_call(spi_info, MAX_UNIQUE_ALLOW, MAX_NUM_BUFFERS, allow_buf, drivers_with_buffers);
  send_response(ret);
}

void send_response(int ret) {
  put_int_in_wbuf(ret);
  spi_slave_read_write(wbuf, rbuf, BUF_SIZE, rsyscall_cb, NULL);
  printf("Toggle the GPIO!\n");
  gpio_toggle(PIN);
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
