#include "driver/mcpwm_prelude.h"

typedef enum
{
    RUDDER = 0,
    ELEVATOR,
    AILERON1,
    AILERON2
} ServoType;

typedef struct {
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t generator;
} ServoControl;

mcpwm_timer_handle_t *get_servo_timer(ServoControl *servo);
mcpwm_oper_handle_t *get_servo_oper(ServoControl *servo);
mcpwm_cmpr_handle_t *get_servo_comparator(ServoControl *servo);
mcpwm_gen_handle_t *get_servo_generator(ServoControl *servo);

void ServoSetup(ServoControl *servo, ServoType task_diff);

// Servo Tasks
void task_rudder(void *pvParameters);
void task_elevator(void *pvParameters);
