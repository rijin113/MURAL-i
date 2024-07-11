#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"

#define SERVO_MIN_PULSEWIDTH_US 500  // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500  // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE        -90   // Minimum angle
#define SERVO_MAX_DEGREE        90    // Maximum angle

#define SERVO_PULSE_GPIO             2        // GPIO connects to the PWM signal line
#define SERVO_PULSE_GPIO2             4        // GPIO connects to the PWM signal line
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD        20000    // 20000 ticks, 20ms

#define TAG "Task"

typedef struct {
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t generator;
} ServoControl;

mcpwm_timer_handle_t *get_servo_timer(ServoControl *servo) {
    return &(servo->timer);
}

mcpwm_oper_handle_t *get_servo_oper(ServoControl *servo) {
    return &(servo->oper);
}

mcpwm_cmpr_handle_t *get_servo_comparator(ServoControl *servo) {
    return &(servo->comparator);
}

mcpwm_gen_handle_t *get_servo_generator(ServoControl *servo) {
    return &(servo->generator);
}

static ServoControl servo1;
static ServoControl servo2;

static inline uint32_t example_angle_to_compare(int angle)
{
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

void ServoSetup(ServoControl *servo, uint8_t task_diff)
{
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
        .period_ticks = SERVO_TIMEBASE_PERIOD,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };

    mcpwm_new_timer(&timer_config, get_servo_timer(servo));

    mcpwm_operator_config_t operator_config = {
        .group_id = 0, // operator must be in the same group to the timer
    };
    mcpwm_new_operator(&operator_config, get_servo_oper(servo));
    mcpwm_operator_connect_timer(servo->oper, servo->timer);

    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };

    mcpwm_new_comparator(servo->oper, &comparator_config, get_servo_comparator(servo));

    mcpwm_generator_config_t generator_config;
    if (task_diff == 1)
    {
        generator_config.gen_gpio_num = SERVO_PULSE_GPIO;
    }
    else
    {
        generator_config.gen_gpio_num = SERVO_PULSE_GPIO2;
    }

    mcpwm_new_generator(servo->oper, &generator_config, get_servo_generator(servo));

    // set the initial compare value, so that the servo will spin to the center position
    mcpwm_comparator_set_compare_value(servo->comparator, example_angle_to_compare(0));

    // go high on counter empty
    mcpwm_generator_set_action_on_timer_event(servo->generator,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    // go low on compare threshold
    mcpwm_generator_set_action_on_compare_event(servo->generator,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, servo->comparator, MCPWM_GEN_ACTION_LOW));

    mcpwm_timer_enable(servo->timer);
    mcpwm_timer_start_stop(servo->timer, MCPWM_TIMER_START_NO_STOP);
}

void task_servo1(void *pvParameters)
{
    mcpwm_cmpr_handle_t *task_comparator = (mcpwm_cmpr_handle_t *)pvParameters;

    int angle = 0;
    int step = 5;
    while (1) {
        ESP_LOGE(TAG, "INSIDE TASK1");
        mcpwm_comparator_set_compare_value(*task_comparator, example_angle_to_compare(angle));

        //Add delay, since it takes time for servo to rotate, usually 200ms/60degree rotation under 5V power supply
        vTaskDelay(pdMS_TO_TICKS(1000));
        if ((angle + step) > 90 || (angle + step) < -90) {
            step *= -1;
        }
        angle += step;
    }
    vTaskDelete(NULL);
}

void task_servo2(void *pvParameters)
{
    mcpwm_cmpr_handle_t *task_comparator = (mcpwm_cmpr_handle_t *)pvParameters;

    int angle = 0;
    int step = 5;
    while (1) {
        ESP_LOGE(TAG, "INSIDE TASK2");
        mcpwm_comparator_set_compare_value(*task_comparator, example_angle_to_compare(angle));

        //Add delay, since it takes time for servo to rotate, usually 200ms/60degree rotation under 5V power supply
        vTaskDelay(pdMS_TO_TICKS(500));
        if ((angle + step) > 90 || (angle + step) < -90) {
            step *= -1;
        }
        angle += step;
    }
    vTaskDelete(NULL);
}

void app_main()
{
    ServoSetup(&servo1, 1);
    ServoSetup(&servo2, 2);
    xTaskCreate(task_servo1, "RunServo1", 4096, &(servo1.comparator), 3, NULL);
    xTaskCreate(task_servo2, "RunServo2", 4096, &(servo2.comparator), 3, NULL);
}