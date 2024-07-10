#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

void task_LED1(void *param)
{
    while (1)
    {
        printf("this is task 1\n");
        vTaskDelay(500); // Yield CPU for 500 ms
    }

    vTaskDelete(NULL);
}

void task_LED2(void *param)
{
    while (1)
    {
        printf("this is task 2\n");
        vTaskDelay(1000); // Yield CPU for 1000 ms
    }

    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(task_LED1, "task1", 4096, NULL, 3, NULL);
    xTaskCreate(task_LED2, "task2", 4096, NULL, 2, NULL);
}