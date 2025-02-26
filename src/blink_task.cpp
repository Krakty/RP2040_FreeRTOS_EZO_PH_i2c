#include "blink_task.h"

void vBlinkTask(void *pvParameters) {
    (void)pvParameters;  // Cast unused parameter to void
    for (;;) {
        gpio_put(LED_PIN, 1);                    // LED on
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));  // Delay 500ms
        gpio_put(LED_PIN, 0);                    // LED off
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));  // Delay 500ms
    }
}