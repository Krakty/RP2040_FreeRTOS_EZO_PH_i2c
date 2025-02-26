#ifndef BLINK_TASK_H
#define BLINK_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "config.h"  // For LED_PIN, BLINK_DELAY_MS, TASK_PRIORITY

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FreeRTOS task to blink the LED on GPIO pin defined by LED_PIN.
 * @param pvParameters Parameters passed to the task (unused).
 */
void vBlinkTask(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif // BLINK_TASK_H