#include <led.h>
#include <qdec.h>
#include <tock.h>
#include <timer.h>
static void timer_fired ( __attribute__ ((unused)) int arg0,
                          __attribute__ ((unused)) int arg1,
                          __attribute__ ((unused)) int arg2,
                          __attribute__ ((unused)) void *ud) {
	printf("Displacement val: %d\n", qdec_displacement());
}

static void qdec_callback(int qdec_num,
                          __attribute__ ((unused)) int val,
                          __attribute__ ((unused)) int arg2,
                          __attribute__ ((unused)) void *ud) {
  printf("QDEC VALUE: %d\n", qdec_num);

  // Set LEDs based on acc value
  if (qdec_num > 0) led_on(0); else led_off(0);
  if (qdec_num == 0) led_on(1); else led_off(1);
  if (qdec_num < 0) led_on(2); else led_off(2);
}


int main(void) {
  printf("THE [QDEC] Test App\n");
  
  if (!qdec_exists()) {
    printf("No QDEC driver\n");
    return -1;
  }
  
  printf("QDEC exists!\n");

  qdec_enable();
  int ret = qdec_subscribe(qdec_callback, NULL);
  printf("Qdec subscribe result: %d\n", ret);
  /*if (!qdec_interrupt_enable()) {
     printf("Can't enable interrupts!\n");
     return -1;
  }*/

  printf("QDEC has been fully initiated!\n");
  
  printf("Setting timer\n");
  static tock_timer_t timer;
  timer_every(5000, timer_fired, NULL, &timer);
  printf("Timer set!\n");

  return 0;
}
 
