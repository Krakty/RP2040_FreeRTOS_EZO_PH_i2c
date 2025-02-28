#include "sensor_task.h"
#include <stdio.h>

void vPhSensorTask(void *pvParameters) {
    struct Params {
        PhSensor* sensor;
        volatile bool* suspendPh;
    } *params = static_cast<struct Params*>(pvParameters);
    PhSensor* sensor = params->sensor;
    volatile bool* suspendPh = params->suspendPh;

    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("PH Sensor Task: Starting up.\n");

    for (;;) {
        if (!*suspendPh) {
            float ph = sensor->readPh();
            if (ph >= 0.0f) {
                printf("pH reading: %.2f\n", ph);
            } else {
                printf("Failed to get pH.\n");
            }
            vTaskDelay(pdMS_TO_TICKS(PH_LOOP_DELAY_MS));
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));  // Wait while suspended
        }
    }
}