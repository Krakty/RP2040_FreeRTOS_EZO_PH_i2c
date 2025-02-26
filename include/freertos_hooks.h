#ifndef FREERTOS_HOOKS_H
#define FREERTOS_HOOKS_H

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

// FreeRTOS Hook Function Declarations
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);
void vApplicationMallocFailedHook(void);
void vApplicationPassiveIdleHook(void);

#ifdef __cplusplus
}
#endif

#endif // FREERTOS_HOOKS_H