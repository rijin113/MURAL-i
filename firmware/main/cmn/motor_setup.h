#include "driver/mcpwm_prelude.h"

typedef struct {
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t generator;
} MotorControl;

mcpwm_timer_handle_t *get_motor_timer(MotorControl *motor);
mcpwm_oper_handle_t *get_motor_oper(MotorControl *motor);
mcpwm_cmpr_handle_t *get_motor_comparator(MotorControl *motor);
mcpwm_gen_handle_t *get_motor_generator(MotorControl *motor);

void MotorSetup(MotorControl *motor);

// Motor Task
void task_motor(void *pvParameters);
