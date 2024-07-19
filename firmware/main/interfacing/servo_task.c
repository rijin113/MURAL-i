#include "servo_setup.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SERVO_MIN_PULSEWIDTH_US 500  // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500  // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE        -90   // Minimum angle
#define SERVO_MAX_DEGREE        90    // Maximum angle

// All the GPIOs below connect to the PWM signal line
#define SERVO_GPIO_RUDDER            2
#define SERVO_GPIO_ELEVATOR          5        
#define SERVO_GPIO_AILERON1          32      
#define SERVO_GPIO_AILERON2          33       
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD        20000    // 20000 ticks, 20ms

static const char *SERVO_TAG = "SERVO TASK";
extern uint16_t channels[6];

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

static inline uint32_t angle_to_compare(int angle)
{
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

void ServoSetup(ServoControl *servo, ServoType servo_type)
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
    switch (servo_type)
    {
        case RUDDER:
            generator_config.gen_gpio_num = SERVO_GPIO_RUDDER;
            break;
        case ELEVATOR:
            generator_config.gen_gpio_num = SERVO_GPIO_ELEVATOR;
            break;
        case AILERON1:
            generator_config.gen_gpio_num = SERVO_GPIO_AILERON1;
            break;
        case AILERON2:
            generator_config.gen_gpio_num = SERVO_GPIO_AILERON2;
            break;
        default:
            break;
    }

    mcpwm_new_generator(servo->oper, &generator_config, get_servo_generator(servo));

    // set the initial compare value, so that the servo will spin to the center position
    mcpwm_comparator_set_compare_value(servo->comparator, angle_to_compare(0));

    // go high on counter empty
    mcpwm_generator_set_action_on_timer_event(servo->generator,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    // go low on compare threshold
    mcpwm_generator_set_action_on_compare_event(servo->generator,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, servo->comparator, MCPWM_GEN_ACTION_LOW));

    mcpwm_timer_enable(servo->timer);
    mcpwm_timer_start_stop(servo->timer, MCPWM_TIMER_START_NO_STOP);
}

void task_rudder(void *pvParameters)
{
    mcpwm_cmpr_handle_t *task_comparator = (mcpwm_cmpr_handle_t *)pvParameters;

    while (1) {
        ESP_LOGE(SERVO_TAG, "MOVING RUDDER");
        
        // TODO: Change all channel indexes below to match actual flight controls.
        // For example, channel 2 might not actually be used for rudder. 

        // Linear Interpolation ([1000, 2000] to [-90, 90])
        int rc_value = -270 + ((channels[0]) * 180 / 1000); // Linear interpolation

        mcpwm_comparator_set_compare_value(*task_comparator, angle_to_compare(rc_value));

        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}

void task_elevator(void *pvParameters)
{
    mcpwm_cmpr_handle_t *task_comparator = (mcpwm_cmpr_handle_t *)pvParameters;

    while (1) {
        ESP_LOGE(SERVO_TAG, "MOVING ELEVATOR");

        // Linear Interpolation ([1000, 2000] to [-90, 90])
        int rc_value = -270 + ((channels[1]) * 180 / 1000); // Linear interpolation

        mcpwm_comparator_set_compare_value(*task_comparator, angle_to_compare(rc_value));

        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}

void task_aileron_one(void *pvParameters)
{
    mcpwm_cmpr_handle_t *task_comparator = (mcpwm_cmpr_handle_t *)pvParameters;

    while (1) {
        ESP_LOGE(SERVO_TAG, "MOVING AILERON ONE");

        // Linear Interpolation ([1000, 2000] to [-90, 90])
        int rc_value = -270 + ((channels[2]) * 180 / 1000); // Linear interpolation

        mcpwm_comparator_set_compare_value(*task_comparator, angle_to_compare(rc_value));

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}

void task_aileron_two(void *pvParameters)
{
    mcpwm_cmpr_handle_t *task_comparator = (mcpwm_cmpr_handle_t *)pvParameters;

    while (1) {
        ESP_LOGE(SERVO_TAG, "MOVING AILERON TWO");

        // Linear Interpolation ([1000, 2000] to [-90, 90])
        int rc_value = -270 + ((channels[3]) * 180 / 1000); // Linear interpolation
        ESP_LOGE(SERVO_TAG, "AILERON TWO DATA: %d", rc_value);

        mcpwm_comparator_set_compare_value(*task_comparator, angle_to_compare(rc_value));

        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}