#include <stdio.h>
extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
}
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "main.h"          // Defines (LED_PIN, PH_SENSOR_I2C_ADDRESS, etc.)
#include "freertos_hooks.h"
#include "clicommands.h"
#include "hardware.h"      // New Hardware class
#include "Ezo_i2c.h"       // EZO library for pH sensor

static void vBlinkTask(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
    }
}

static void vPhSensorTask(void *pvParameters) {
    Hardware* hw = static_cast<Hardware*>(pvParameters);
    Ezo_board ezoSensor(PH_SENSOR_I2C_ADDRESS, "pH", hw->getI2c0());  // Updated getter
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("PH Sensor Task: Starting up.\n");
    for (;;) {
        ezoSensor.send_read_cmd();
        vTaskDelay(pdMS_TO_TICKS(PH_READ_DELAY_MS));
        if (ezoSensor.receive_read_cmd() == Ezo_board::SUCCESS) {
            float reading = ezoSensor.get_last_received_reading();
            printf("pH reading: %.2f\n", reading);
        } else {
            printf("Failed to get pH.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(PH_LOOP_DELAY_MS));
    }
}
extern "C" int main(void) {
    Hardware hw;  // Create Hardware instance

    // Initialize active hardware
    hw.initStdio();     // UART0/USB for pH output
    hw.initLedGpio();   // LED on GPIO 25
    hw.initI2c0();      // I2C0 for pH sensor (GPIO 4/5)
    hw.initUart1();     // UART1 for CLI (GPIO 8/9)

    // Create tasks, passing Hardware pointer to vPhSensorTask
    xTaskCreate(vBlinkTask, "Blink", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
    xTaskCreate(vPhSensorTask, "PHSensor", configMINIMAL_STACK_SIZE * 2, &hw, TASK_PRIORITY, NULL);

    printf("Starting FreeRTOS SMP scheduler\n");
    vTaskStartScheduler();
    printf("Scheduler failed to start\n");
    while (1);  // Should never reach here
    return 0;
}