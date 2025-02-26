#ifndef CLI_TASK_H
#define CLI_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "ph_sensor.h"  // PhSensor class
#include "hardware.h"   // Hardware for UART1
#include "config.h"     // Displays, prompts, TASK_PRIORITY

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FreeRTOS task for CLI on UART1 - mode-switching with safe/danger tables.
 * @param pvParameters Pointer to CliTaskParams struct.
 */
void vCliTask(void *pvParameters);

// Struct to pass PhSensor and Hardware to vCliTask
typedef struct {
    PhSensor* sensor;
    Hardware* hardware;
} CliTaskParams;

#ifdef __cplusplus
}
#endif

#endif // CLI_TASK_H