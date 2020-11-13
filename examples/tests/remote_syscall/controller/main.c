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

#define BUF_SIZE 5 // 1 + 1 + 1 + 1 + 1
char rbuf[BUF_SIZE];
char wbuf[BUF_SIZE];

void zero_wbuf(void) {
  for (int i = 0; i < BUF_SIZE; i++) {
    wbuf[i] = 0;
  }
}

int* process_input(int* spi_buf) {
  char* tok;
  tok = strtok(wbuf, ":");
  int i = 0;
  while (tok && i < 5) {
    spi_buf[i] = (int)strtol(tok, &end, 10);
    i++;
    tok = strtok(NULL, ":");
  }
}

static void wsyscall_cb(__attribute__ ((unused)) int arg0,
                     __attribute__ ((unused)) int arg2,
                     __attribute__ ((unused)) int arg3,
                     __attribute__ ((unused)) void* userdata) {
  int spi_buf[5];
  process_input(spi_buf);
  spi_read_write(wbuf, rbuf, BUF_SIZE, NULL, NULL);
  printf("\nWrite callback complete!\n");
  zero_wbuf();
  int ret = subscribe_to_caller(wsyscall_cb, wbuf, BUF_SIZE);
  if (ret != TOCK_SUCCESS) {
    printf("[SHORT] Error doing UART receive: %i\n", ret);
  }
}

int main(void) {
  spi_init();
  spi_set_chip_select(0);
  spi_set_rate(400000);
  spi_set_polarity(false);
  spi_set_phase(false);
  printf("Spi Controller is initialized!\n");
  
  zero_wbuf();
  int ret = subscribe_to_caller(wsyscall_cb, wbuf, BUF_SIZE);
  if (ret != TOCK_SUCCESS) {
    printf("[SHORT] Error doing UART receive: %i\n", ret);
  }
  printf("Prepped!\n");
}
