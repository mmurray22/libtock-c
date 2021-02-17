/*
 * Remote Syscalls
 */

#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

  /* Peripheral Functions */
  int subscribe_to_caller(subscribe_cb cb, char* buf, size_t len);
  int execute_system_call(unsigned int* request);
  void syscallZero(void);
  int syscallOne(size_t driverNum, size_t subscribeNum, void* arg0, subscribe_cb cb);
  int syscallTwo(size_t driverNum, size_t subdriverNum, size_t arg0, size_t arg1);
  int syscallThree(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer);
  int syscallFour(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer);
  void* syscallFive(size_t operand, size_t arg0);
 
  /* Controller Functions */

#ifdef __cplusplus
}
#endif
