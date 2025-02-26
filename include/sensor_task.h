#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "ph_sensor.h"  // PhSensor class
#include "config.h"     // TASK_PRIORITY, etc.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FreeRTOS task to periodically read pH using PhSensor and print to UART0/USB.
 * @param pvParameters Pointer to PhSensor instance.
 */
void vPhSensorTask(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif // SENSOR_TASK_H