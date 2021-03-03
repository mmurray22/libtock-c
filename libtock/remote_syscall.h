/*
 * Remote Syscalls
 */
#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    char** arr;
    size_t driver_num;
    size_t used;
    size_t size;
  } DriverArray;

  typedef struct {
    DriverArray* arr;
    size_t used;
    size_t size;
  } AllowArray;

  /* DriverArray Functions */
  void initDriverArray(DriverArray* da, size_t initialSize);
  void insertBuffer(DriverArray* da, char* buf, size_t buf_len); 
  void removeBuffer(DriverArray* da, int index);
  void freeDriverArray(DriverArray* da);

  /* AllowArray Functions */
  void initAllowArray(AllowArray* aa, size_t initialSize);
  void insertDriverArray(AllowArray* aa, DriverArray da);
  void removeDriverArray(AllowArray* aa, DriverArray da);
  void freeAllowArray(AllowArray* aa);

  /* Peripheral Functions */
  int execute_system_call(unsigned int* request,
                          AllowArray* allow_array);
  void syscallZero(void);
  int syscallOne(size_t driverNum, size_t subscribeNum, void* arg0, subscribe_cb cb);
  int syscallTwo(size_t driverNum, size_t subdriverNum, size_t arg0, size_t arg1);
  int syscallThree (size_t driverNum, size_t subdriverNum, size_t arg0, size_t buf_len, AllowArray* aa);
  int syscallFour(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer);
  void* syscallFive(size_t operand, size_t arg0);

 #ifdef __cplusplus
}
#endif
