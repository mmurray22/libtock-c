#include "remote_syscall.h"

static void remotesys_callback(__attribute__ ((unused)) int num,
                            int activateNumber,
                            __attribute__ ((unused)) int arg2,
                            __attribute__ ((unused)) void *ud) {
    printf("Callback activated!");
    ack(activateNumber);

    /*Carry out Caller instructions*/
    match activateNumber {
      1 => activate_led(),
      2 => activate_temperature(),
      3 => activate_qdec(),
      _ => printf("Currently not supported!"),
    }
    /*ack("Complete!");*/
}

/*
 * First iteration of main:
 * -One large infinite while loop for the connection
 */
int main() {
  subscribe_to_caller(remotesys_callback);
  return 0;
}
