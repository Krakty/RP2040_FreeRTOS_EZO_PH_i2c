#ifndef MAIN_H
#define MAIN_H

#define LED_PIN 25           // Default Pico LED pin (GPIO 25)
#define BLINK_DELAY_MS 500   // Blink every 500ms
#define TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)  // One above idle priority

#define PH_SENSOR_I2C_ADDRESS 0x63  // Example address for pH sensor (EZO pH is typically 99 decimal = 0x63 hex)
#define PH_READ_DELAY_MS 800       // Delay for pH sensor reading (per EZO spec)
#define PH_LOOP_DELAY_MS 1000      // Loop delay between readings


#endif /* MAIN_H */

