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

#define BUF_SIZE 10
#define BASE_TEN 10 
#define DELIMITER ":"
#define NUM_ARGS 5

char spi_pass_buf[NUM_ARGS*4];
char rbuf[NUM_ARGS*4];
char wbuf[BUF_SIZE];

void process_input(int* spi_buf); 
void print_buf(int* buf);

void print_buf(int* buf) {
  for (int i = 0; i < NUM_ARGS; i++) {
    printf("Buf elt: %d\n", buf[i]);
  }
}

void process_input(int* spi_buf) {
  char* tok;
  tok = strtok(wbuf, DELIMITER);
  int i = 0;
  char* end;
  while (tok && i < NUM_ARGS) {
    spi_buf[i] = (int)strtol(tok, &end, BASE_TEN);
    i++;
    tok = strtok(NULL, DELIMITER);
  }
  if ( i < 5 ) {
    spi_buf[i] = 0;
    i++;
  }
}

static void wsyscall_cb(__attribute__ ((unused)) int arg0,
                        __attribute__ ((unused)) int arg2,
                        __attribute__ ((unused)) int arg3,
                        __attribute__ ((unused)) void* userdata) {
  int spi_buf[NUM_ARGS];
  process_input(spi_buf);
  print_buf(spi_buf);
  memcpy(spi_pass_buf, spi_buf, sizeof(spi_buf));
  spi_read_write(spi_pass_buf, rbuf, NUM_ARGS*4, NULL, NULL);
  int test_buf[NUM_ARGS];
  printf("Sizeof: %d\n", sizeof(spi_pass_buf));
  memcpy(test_buf, spi_pass_buf, sizeof(spi_pass_buf));
  print_buf(test_buf);
  printf("\n Write callback complete! \n");
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
  
  int ret = subscribe_to_caller(wsyscall_cb, wbuf, BUF_SIZE);
  if (ret != TOCK_SUCCESS) {
    printf("[SHORT] Error doing UART receive: %i\n", ret);
  }
  printf("Prepped!\n");
}
