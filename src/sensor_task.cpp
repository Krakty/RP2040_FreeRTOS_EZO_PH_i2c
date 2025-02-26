#include "sensor_task.h"
#include <stdio.h>

void vPhSensorTask(void *pvParameters) {
    PhSensor* sensor = static_cast<PhSensor*>(pvParameters);
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("PH Sensor Task: Starting up.\n");

    for (;;) {
        float ph = sensor->readPh();
        if (ph >= 0.0f) {
            printf("pH reading: %.2f\n", ph);
        } else {
            printf("Failed to get pH.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(PH_LOOP_DELAY_MS));
    }
}