/*
 * Remote Syscalls
 */

#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

  /*Listening: this function let's */
  int subscribe_to_caller(subscribe_cb cb, char* buf, size_t len);
  void syscallZero();
  void syscallOne(size_t driverNum, size_t subscribeNum, size_t arg0, subscribe_cb cb);
  void syscallTwo(size_t driverNum, size_t subdriverNum, size_t arg0, size_t arg1);
  void syscallThree(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer);
  void syscallFour(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer);
  void syscallFive(size_t operand, size_t arg0);

#ifdef __cplusplus
}
#endif
