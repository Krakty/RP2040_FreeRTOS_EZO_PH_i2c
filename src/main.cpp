#include <stdio.h>
extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
}
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "config.h"          // PH_SENSOR_I2C_ADDRESS, TASK_PRIORITY, etc.
#include "freertos_hooks.h"
#include "clicommands.h"     // Placeholder for CLI - refine later
#include "hardware.h"
#include "ph_sensor.h"       // New PhSensor class
#include "blink_task.h"      // vBlinkTask
#include "sensor_task.h"     // New vPhSensorTask - add this

extern "C" int main(void) {
    Hardware hw;
    hw.initStdio();    // UART0/USB for printf
    hw.initLedGpio();  // LED on GPIO 25
    hw.initI2c0();     // I2C0 for pH sensor (GPIO 4/5)
    hw.initUart1();    // UART1 for future CLI (GPIO 8/9)

    // Create PhSensor instance with I2C0 and address from config
    PhSensor phSensor(hw.getI2c0(), PH_SENSOR_I2C_ADDRESS);

    // Create tasks - pass PhSensor pointer to vPhSensorTask
    xTaskCreate(vBlinkTask, "Blink", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
    xTaskCreate(vPhSensorTask, "PHSensor", configMINIMAL_STACK_SIZE * 2, &phSensor, TASK_PRIORITY, NULL);

    printf("Starting FreeRTOS SMP scheduler\n");
    vTaskStartScheduler();
    printf("Scheduler failed to start\n");
    while (1);
    return 0;
}