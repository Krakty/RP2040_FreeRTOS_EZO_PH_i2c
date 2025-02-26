#include "cli_task.h"
#include <stdio.h>
#include <string.h>

// Forward declaration - process CLI commands using PhSensor
static void process_cli_command(PhSensor* sensor, uart_inst_t* cliUart, char* cmd_str);

void vCliTask(void *pvParameters) {
    CliTaskParams* params = static_cast<CliTaskParams*>(pvParameters);
    PhSensor* sensor = params->sensor;
    uart_inst_t* cliUart = params->hardware->getUart1();  // Renamed from uart1

    char buffer[32];  // Buffer for CLI input - fits "PH:CAL,MID,7.00"
    uint8_t pos = 0;
    bool dangerMode = false;  // Start in safe mode

    printf("CLI Task: Starting up on UART1\n");  // UART0/USB
    uart_puts(cliUart, PH_CLI_SCROLL_PADDING);   // Initial scroll
    uart_puts(cliUart, PH_CLI_SAFE_DISPLAY);     // Safe table
    uart_puts(cliUart, PH_CLI_SAFE_PROMPT);      // Safe prompt

    for (;;) {
        if (uart_is_readable(cliUart)) {
            char c = uart_getc(cliUart);
            uart_putc(cliUart, c);  // Echo to UART1

            if (c == '\r' || c == '\n') {  // Command entered
                buffer[pos] = '\0';        // Null-terminate
                if (pos > 0) {             // Non-empty command
                    if (!dangerMode && strncmp(buffer, "PH:DANGER", 9) == 0) {
                        uart_puts(cliUart, "\r\nPH:DANGER -> Entering Danger Mode\r\n");
                        uart_puts(cliUart, "PH:DANGER -> Mode Switched\r\n");
                        dangerMode = true;
                    } else if (dangerMode && strncmp(buffer, "EXIT", 4) == 0) {
                        uart_puts(cliUart, "\r\nEXIT -> Exiting Danger Mode\r\n");
                        uart_puts(cliUart, "EXIT -> Mode Switched\r\n");
                        dangerMode = false;
                    } else if (strncmp(buffer, "PH:HELP", 7) == 0) {
                        // Do nothing - reprint handled below
                    } else {
                        process_cli_command(sensor, cliUart, buffer);  // Pass renamed cliUart
                    }
                    // Reprint table based on mode
                    uart_puts(cliUart, PH_CLI_SCROLL_PADDING);
                    uart_puts(cliUart, dangerMode ? PH_CLI_DANGER_DISPLAY : PH_CLI_SAFE_DISPLAY);
                    uart_puts(cliUart, dangerMode ? PH_CLI_DANGER_PROMPT : PH_CLI_SAFE_PROMPT);
                }
                pos = 0;  // Reset buffer
            } else if (pos < sizeof(buffer) - 1) {  // Room in buffer
                buffer[pos++] = c;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Avoid busy-wait
    }
}

static void process_cli_command(PhSensor* sensor, uart_inst_t* cliUart, char* cmd_str) {
    char result[32];  // Buffer for UART1 echo
    if (strncmp(cmd_str, "PH:READ", 7) == 0) {
        float ph = sensor->readPh();
        if (ph >= 0.0f) {
            snprintf(result, sizeof(result), "PH:READ -> %.2f", ph);
            printf("pH: %.2f\n", ph);  // UART0
        } else {
            snprintf(result, sizeof(result), "PH:READ -> failed");
            printf("pH read failed\n");
        }
    } else if (strncmp(cmd_str, "PH:CAL", 6) == 0) {
        char mode[10];
        float value = 0.0f;
        if (sscanf(cmd_str, "PH:CAL,%9[^,],%f", mode, &value) >= 1) {
            sensor->calibrate(mode, value);
            snprintf(result, sizeof(result), "PH:CAL,%s -> Done", mode);
            printf("Calibration %s done\n", mode);
        } else {
            snprintf(result, sizeof(result), "PH:CAL -> Invalid");
        }
    } else if (strncmp(cmd_str, "PH:LED", 6) == 0) {
        if (strncmp(cmd_str + 7, "ON", 2) == 0) {
            sensor->setLed(true);
            snprintf(result, sizeof(result), "PH:LED,ON -> Done");
            printf("LED on\n");
        } else if (strncmp(cmd_str + 7, "OFF", 3) == 0) {
            sensor->setLed(false);
            snprintf(result, sizeof(result), "PH:LED,OFF -> Done");
            printf("LED off\n");
        } else if (strncmp(cmd_str + 7, "?", 1) == 0) {
            snprintf(result, sizeof(result), "PH:LED,? -> %s", sensor->getLedState() ? "ON" : "OFF");
            printf("LED state: %s\n", sensor->getLedState() ? "ON" : "OFF");
        } else {
            snprintf(result, sizeof(result), "PH:LED -> Invalid");
        }
    } else if (strncmp(cmd_str, "PH:FIND", 7) == 0) {
        sensor->find();
        snprintf(result, sizeof(result), "PH:FIND -> Blinking");
        printf("Finding device - LED blinking\n");
    } else if (strncmp(cmd_str, "PH:SLOPE", 8) == 0) {
        float acid, base, offset;
        sensor->getSlope(acid, base, offset);
        snprintf(result, sizeof(result), "PH:SLOPE -> %.1f,%.1f,%.2f", acid, base, offset);
        printf("Slope: Acid %.1f%%, Base %.1f%%, Offset %.2fmV\n", acid, base, offset);
    } else if (strncmp(cmd_str, "PH:TEMP", 7) == 0) {
        float temp;
        if (sscanf(cmd_str, "PH:TEMP,%f", &temp) == 1) {
            sensor->setTemperatureCompensation(temp);
            snprintf(result, sizeof(result), "PH:TEMP,%.1f -> Done", temp);
            printf("Temp comp set to %.1fC\n", temp);
        } else {
            snprintf(result, sizeof(result), "PH:TEMP -> Invalid");
        }
    } else if (strncmp(cmd_str, "PH:STATUS", 9) == 0) {
        char status[32];
        sensor->getStatus(status, sizeof(status));
        snprintf(result, sizeof(result), "PH:STATUS -> %s", status);
        printf("Status: %s\n", status);
    } else if (strncmp(cmd_str, "PH:INFO", 7) == 0) {
        char info[32];
        sensor->getInfo(info, sizeof(info));
        snprintf(result, sizeof(result), "PH:INFO -> %s", info);
        printf("Info: %s\n", info);
    } else if (strncmp(cmd_str, "PH:BAUD", 7) == 0) {
        uint32_t baud;
        if (sscanf(cmd_str, "PH:BAUD,%lu", &baud) == 1) {
            sensor->setBaudWithConfirm(baud);
            snprintf(result, sizeof(result), "PH:BAUD,%lu -> Done", baud);
            printf("Baud set to %lu - reconnect if needed\n", baud);
        } else {
            snprintf(result, sizeof(result), "PH:BAUD -> Invalid");
        }
    } else if (strncmp(cmd_str, "PH:FACTORY", 10) == 0) {
        sensor->factoryResetWithConfirm();
        snprintf(result, sizeof(result), "PH:FACTORY -> Done");
        printf("Factory reset done - recalibrate\n");
    } else if (strncmp(cmd_str, "PH:PLOCK", 8) == 0) {
        int lock;
        if (sscanf(cmd_str, "PH:PLOCK,%d", &lock) == 1) {
            sensor->setProtocolLockWithConfirm(lock != 0);
            snprintf(result, sizeof(result), "PH:PLOCK,%d -> Done", lock);
            printf("Protocol lock %s\n", lock ? "enabled" : "disabled");
        } else {
            snprintf(result, sizeof(result), "PH:PLOCK -> Invalid");
        }
    } else if (strncmp(cmd_str, "PH:I2C", 6) == 0) {
        uint8_t addr;
        if (sscanf(cmd_str, "PH:I2C,%hhu", &addr) == 1) {
            sensor->setI2cAddressWithConfirm(addr);
            snprintf(result, sizeof(result), "PH:I2C,%u -> Done", addr);
            printf("I2C address set to %u\n", addr);
        } else {
            snprintf(result, sizeof(result), "PH:I2C -> Invalid");
        }
    } else {
        snprintf(result, sizeof(result), "%s -> Unknown", cmd_str);
    }
    uart_puts(cliUart, "\r\n");
    uart_puts(cliUart, result);  // Echo result to UART1
}