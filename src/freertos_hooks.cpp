#include "freertos_hooks.h"
#include <stdio.h>

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
    (void)pxTask;
    (void)pcTaskName;
    printf("Stack overflow in task: %s\n", pcTaskName);
    while (1);  // Trap for debugging
}

void vApplicationTickHook(void) {
    // Empty; required by configUSE_TICK_HOOK = 1
}

void vApplicationMallocFailedHook(void) {
    printf("Malloc failed\n");
    while (1);  // Trap for debugging
}

void vApplicationPassiveIdleHook(void) {
    // Empty; required by SMP port despite configUSE_PASSIVE_IDLE_HOOK = 0
}