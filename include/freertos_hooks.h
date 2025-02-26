#ifndef FREERTOS_HOOKS_H
#define FREERTOS_HOOKS_H

#include "FreeRTOS.h"  // Base FreeRTOS types
#include "task.h"      // TaskHandle_t

#ifdef __cplusplus
extern "C" {
#endif

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);
void vApplicationMallocFailedHook(void);
void vApplicationPassiveIdleHook(void);

#ifdef __cplusplus
}
#endif

#endif // FREERTOS_HOOKS_H