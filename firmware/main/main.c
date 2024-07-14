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

void app_main()
{
    /* Plane Setup */
    ServoSetup(&servo_rudder, RUDDER);
    ServoSetup(&servo_elevator, ELEVATOR);
    RCSetup();

    /* Tasks for RC Reciever and Transmitter */
    xTaskCreate(rx_rc_task, "UART_RX_Task", 4096, NULL, configMAX_PRIORITIES, NULL);

    /* Tasks for Servos (flying controls) */
    xTaskCreate(task_rudder, "StartRudder", 4096, &(servo_rudder.comparator), 3, NULL);
    xTaskCreate(task_elevator, "StartElevator", 4096, &(servo_elevator.comparator), 3, NULL);
}