#include <stdio.h>
extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
}
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "main.h"

// EZO library headers
#include "Ezo_i2c.h"

// Define constants
#define LED_PIN 25
#define BLINK_DELAY_MS 500
#define PH_SENSOR_I2C_ADDRESS 0x63  // Example address for pH sensor (EZO pH is typically 99 decimal = 0x63 hex)
#define PH_READ_DELAY_MS 800       // Delay for pH sensor reading (per EZO spec)
#define PH_LOOP_DELAY_MS 1000      // Loop delay between readings

static void prvSetupHardware(void) {
    stdio_init_all();  // Initialize UART and USB stdio
    
    // Initialize GPIO for LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);  // Start LED off
    
    // Initialize I2C0 at 100kHz on GPIO 4 (SDA) & 5 (SCL)
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
}

static void vBlinkTask(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        gpio_put(LED_PIN, 1);
        printf("LED ON\n");
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
        gpio_put(LED_PIN, 0);
        printf("LED OFF\n");
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
    }
}

static void vPhSensorTask(void *pvParameters) {
    (void)pvParameters;
    
    // Create Ezo_board object for pH sensor
    Ezo_board ezoSensor(PH_SENSOR_I2C_ADDRESS);
    
    // Wait for sensor to power on
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    printf("PH Sensor Task: Starting up.\n");
    
    for (;;) {
        // Send read command to pH sensor
        ezoSensor.send_read_cmd();
        vTaskDelay(pdMS_TO_TICKS(PH_READ_DELAY_MS));  // Wait for reading
        
        // Retrieve and print the reading
        if (ezoSensor.receive_read_cmd() == Ezo_board::SUCCESS) {
            float reading = ezoSensor.get_last_received_reading();
            printf("pH reading: %.2f\n", reading);
        } else {
            printf("Failed to get pH.\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(PH_LOOP_DELAY_MS));  // Loop delay
    }
}

extern "C" int main(void) {
    prvSetupHardware();
    
    // Create the LED blink task
    xTaskCreate(vBlinkTask, "Blink", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
    
    // Create the pH sensor task
    xTaskCreate(vPhSensorTask, "PHSensor", configMINIMAL_STACK_SIZE * 2, NULL, TASK_PRIORITY, NULL);
    
    printf("Starting FreeRTOS SMP scheduler\n");
    vTaskStartScheduler();
    printf("Scheduler failed to start\n");
    while (1);  // Should never reach here
    return 0;
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
    (void)pxTask;
    (void)pcTaskName;
    printf("Stack overflow in task: %s\n", pcTaskName);
    while (1);  // Trap for debugging
}

extern "C" void vApplicationTickHook(void) {
    // Empty; required by configUSE_TICK_HOOK = 1
}

extern "C" void vApplicationMallocFailedHook(void) {
    printf("Malloc failed\n");
    while (1);  // Trap for debugging
}

extern "C" void vApplicationPassiveIdleHook(void) {
    // Empty; required by SMP port despite configUSE_PASSIVE_IDLE_HOOK = 0
}