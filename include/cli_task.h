#ifndef CLI_TASK_H
#define CLI_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "ph_sensor.h"
#include "hardware.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

void vCliTask(void *pvParameters);

typedef struct {
    PhSensor* sensor;
    Hardware* hardware;
    volatile bool* suspendPh;  // Signal to suspend/resume pH
} CliTaskParams;

#ifdef __cplusplus
}
#endif

#endif // CLI_TASK_H