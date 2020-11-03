/* Controller */
#include "remote_syscall.h"
#include <console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <spi.h>
#include <button.h>

#define BUF_SIZE 16
char rbuf[BUF_SIZE];
char wbuf[BUF_SIZE];
char ibuf[BUF_SIZE];
char zbuf[BUF_SIZE];
bool toggle = true;

// This callback occurs when a read_write call completed.
// Note that we do not start another transfer here, and
// we simply check for buffer equality.
static void write_cb(__attribute__ ((unused)) int arg0,
                     __attribute__ ((unused)) int arg2,
                     __attribute__ ((unused)) int arg3,
                     __attribute__ ((unused)) void* userdata) {
  printf("Write callback complete!\n");
}

// This is the callback for the button press.
// Each button press represents a transfer, and
// we do not start transfering until the button
// has been pressed.
static void button_cb(__attribute__((unused)) int btn_num,
                      __attribute__ ((unused)) int val,
                      __attribute__ ((unused)) int arg2,
                      __attribute__ ((unused)) void* userdata) {
  if (val == 1) {
    if (toggle) {
      // This is the first read write; note that this is
      // inverted from the spi_slave_transfer, as we do
      // not call spi_read_write until after the button
      // is pressed.
      spi_read_write(wbuf, rbuf, BUF_SIZE, write_cb, NULL);
    } else {
      spi_read_write(rbuf, wbuf, BUF_SIZE, write_cb, NULL);
    }
    // Note that the toggle is reset inside the button callback,
    // not the write completed callback. This decision was arbitrary.
    toggle = !toggle;
  }
}

int main(void) {
  int i;
  for (i = 0; i < BUF_SIZE; i++) {
    wbuf[i] = i;
    ibuf[i] = i;
    zbuf[i] = 0;
  }
  printf("Buf setup!\n");
  spi_init();
  spi_set_chip_select(0);
  spi_set_rate(400000);
  spi_set_polarity(false);
  spi_set_phase(false);
  printf("Spi Controller is initialized!\n");

  button_subscribe(button_cb, NULL);

  int nbuttons = button_count();
  int j;
  for (j = 0; j < nbuttons; j++) {
    button_enable_interrupt(j);
  }
}
