/*
 * Remote Syscalls
 */

#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

  /*Listening: this function let's */
  void subscribe_to_caller(subscribe_cb cb);
  bool ack(int activateNumber);
  void activate_leds();
  void activate_temperature();
  void activate_qdec();
  bool continue_connection();
}
