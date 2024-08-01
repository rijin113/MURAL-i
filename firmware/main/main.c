#include "servo_setup.h"
#include "rc_setup.h"

#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static ServoControl servo_rudder;
static ServoControl servo_elevator;
static ServoControl servo_aileron_one;
static ServoControl servo_aileron_two;

static ServoControl tests[2];

void app_main()
{
    /* Plane Setup */
    ServoSetup(&servo_aileron_one, AILERON1);
    ServoSetup(&servo_aileron_two, AILERON2);
    // ServoSetup(&servo_rudder, RUDDER);
    // ServoSetup(&servo_elevator, ELEVATOR);

    ServoSetup(&tests[0], ELEVATOR);
    ServoSetup(&tests[1], RUDDER);

    // ISSUE HAPPENS WHEN I UNCOMMENT ONE OF THE ABOVE. NOT A CHANNEL ISSUE. NOT A PIN ISSUE. TRIED CHANGING BOTH.
    // NOT A FREERTOS TASK ISSUE SINCE ONLY THREE TASKS ARE RUNNING AND ITS USING ONLY ONE OF THE TEST INSTANCE EITHER WAY.
    // IT IS FAILING FOR SURE AT THE SETUP OF THEM TOGETHER

    RCSetup();

    /* Tasks for RC Reciever and Transmitter */
    xTaskCreate(rx_rc_task, "UART_RX_Task", 4096, NULL, configMAX_PRIORITIES, NULL);

    /* Tasks for Servos (flying controls) */
    xTaskCreate(task_aileron_one, "StartAileronOne", 4096, &(servo_aileron_one.comparator), 3, NULL);
    xTaskCreate(task_aileron_two, "StartAileronTwo", 4096, &(servo_aileron_two.comparator), 3, NULL);
    // xTaskCreate(task_elevator, "StartElevator", 4096, &(servo_elevator.comparator), 3, NULL);
    xTaskCreate(task_rudder, "StartRudder", 4096, tests, 3, NULL);
}