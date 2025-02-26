#include <stdio.h>
extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
}
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "config.h"
#include "freertos_hooks.h"
#include "clicommands.h"
#include "hardware.h"
#include "ph_sensor.h"
#include "blink_task.h"
#include "sensor_task.h"
#include "cli_task.h"

extern "C" int main(void) {
    Hardware hw;
    hw.initStdio();
    hw.initLedGpio();
    hw.initI2c0();
    hw.initUart1();

    PhSensor phSensor(hw.getI2c0(), PH_SENSOR_I2C_ADDRESS);
    CliTaskParams cliParams = { &phSensor, &hw };

    xTaskCreate(vBlinkTask, "Blink", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
    xTaskCreate(vPhSensorTask, "PHSensor", configMINIMAL_STACK_SIZE * 2, &phSensor, TASK_PRIORITY, NULL);
    xTaskCreate(vCliTask, "CLI", configMINIMAL_STACK_SIZE * 2, &cliParams, TASK_PRIORITY, NULL);

    printf("Starting FreeRTOS SMP scheduler\n");
    vTaskStartScheduler();
    printf("Scheduler failed to start\n");
    while (1);
    return 0;
}