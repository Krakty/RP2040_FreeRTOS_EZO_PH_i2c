#include "sensor_task.h"
#include <stdio.h>

void vPhSensorTask(void *pvParameters) {
    PhSensor* sensor = static_cast<PhSensor*>(pvParameters);
    vTaskDelay(pdMS_TO_TICKS(1000));  // Initial delay - 1s startup
    printf("PH Sensor Task: Starting up.\n");

    for (;;) {
        float ph = sensor->readPh();
        if (ph >= 0.0f) {  // Success - PhSensor returns -1.0f on error
            printf("pH reading: %.2f\n", ph);
        } else {
            printf("Failed to get pH.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(PH_LOOP_DELAY_MS));  // ~1s between reads - total ~1.8s with readPh delay
    }
}