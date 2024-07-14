#include "driver/gpio.h"

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

void RCSetup(void);
void parse_ibus_data(uint8_t* data, int len);
void rx_rc_task(void *arg);