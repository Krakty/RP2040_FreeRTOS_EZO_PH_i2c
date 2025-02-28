#include "cli_task.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

static void process_cli_command(PhSensor* sensor, char* cmd_str);

void vCliTask(void *pvParameters) {
    CliTaskParams* params = static_cast<CliTaskParams*>(pvParameters);
    PhSensor* sensor = params->sensor;
    volatile bool* suspendPh = params->suspendPh;

    char buffer[32];
    uint8_t pos = 0;
    bool dangerMode = false;

    printf("CLI Task: Starting up\n");
    for (;;) {
        int c = stdio_getchar_timeout_us(100000);  // 100ms timeout
        if (c != PICO_ERROR_TIMEOUT) {
            if (c == 'P') {
                *suspendPh = true;
                for (int i = 0; i < 50; i++) printf("\n");
                printf(PH_CLI_SAFE_DISPLAY);
                printf(PH_CLI_SAFE_PROMPT);
                while (*suspendPh) {
                    int c = stdio_getchar_timeout_us(10000);  // 10ms timeout
                    if (c != PICO_ERROR_TIMEOUT) {
                        printf("%c", c);
                        if (c == '\r' || c == '\n') {
                            buffer[pos] = '\0';
                            if (pos > 0) {
                                if (!dangerMode && strncmp(buffer, PH_CMD_DANGER, 9) == 0) {
                                    dangerMode = true;
                                    printf("\r\nPH:DANGER -> Entering Danger Mode\r\n\r\n");
                                } else if (strncmp(buffer, PH_CMD_EXIT, 7) == 0) {
                                    if (dangerMode) {
                                        dangerMode = false;
                                        printf("\r\nPH:EXIT -> Leaving Danger Mode\r\n\r\n");
                                    } else {
                                        printf("\r\nPH:EXIT -> Exiting Programming Mode\r\n\r\n");
                                        *suspendPh = false;
                                    }
                                } else if (strncmp(buffer, PH_CMD_HELP, 7) == 0) {
                                    printf("\r\n");
                                    process_cli_command(sensor, buffer);
                                    printf("\r\n\r\n");
                                } else {
                                    printf("\r\n");
                                    process_cli_command(sensor, buffer);
                                    printf("\r\n\r\n");
                                    vTaskDelay(pdMS_TO_TICKS(2000));  // 2s delay
                                }
                            }
                            if (*suspendPh) {
                                for (int i = 0; i < 50; i++) printf("\n");
                                printf(dangerMode ? PH_CLI_DANGER_DISPLAY : PH_CLI_SAFE_DISPLAY);
                                printf(dangerMode ? PH_CLI_DANGER_PROMPT : PH_CLI_SAFE_PROMPT);
                            }
                            pos = 0;
                        } else if (pos < sizeof(buffer) - 1) {
                            buffer[pos++] = (char)c;
                        }
                    }
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void process_cli_command(PhSensor* sensor, char* cmd_str) {
    char result[32];
    if (strncmp(cmd_str, PH_CMD_READ, 7) == 0) {
        float ph = sensor->readPh();
        if (ph >= 0.0f) {
            snprintf(result, sizeof(result), "PH:READ -> %.2f", ph);
            printf("%s", result);
        } else {
            snprintf(result, sizeof(result), "PH:READ -> failed");
            printf("%s", result);
        }
    } else if (strncmp(cmd_str, PH_CMD_LED_ON, 9) == 0) {
        sensor->setLed(true);  // Silent
    } else if (strncmp(cmd_str, PH_CMD_LED_OFF, 10) == 0) {
        sensor->setLed(false);  // Silent
    } else if (strncmp(cmd_str, PH_CMD_LED_STATE, 8) == 0) {
        bool state = sensor->getLedState();
        snprintf(result, sizeof(result), "PH:LED,? -> %s", state ? "ON" : "OFF");
        printf("%s", result);
    } else if (strncmp(cmd_str, PH_CMD_FIND, 7) == 0) {
        sensor->find();  // Silent
    } else if (strncmp(cmd_str, PH_CMD_SLOPE, 8) == 0) {
        float acidPercent, basePercent, mVOffset;
        sensor->getSlope(acidPercent, basePercent, mVOffset);
        if (acidPercent != 0.0f || basePercent != 0.0f || mVOffset != 0.0f) {
            snprintf(result, sizeof(result), "PH:SLOPE -> %.1f,%.1f,%.2f", acidPercent, basePercent, mVOffset);
            printf("%s", result);
        } else {
            snprintf(result, sizeof(result), "PH:SLOPE -> failed");
            printf("%s", result);
        }
    } else if (strncmp(cmd_str, PH_CMD_TEMP, 7) == 0) {
        if (strncmp(cmd_str, PH_CMD_TEMP_STATE, 9) == 0) {
            float temp = sensor->getTemperatureCompensation();
            if (temp != 0.0f) {
                snprintf(result, sizeof(result), "PH:TEMP,? -> %.1f", temp);
                printf("%s", result);
            } else {
                snprintf(result, sizeof(result), "PH:TEMP,? -> failed");
                printf("%s", result);
            }
        } else if (strncmp(cmd_str, PH_CMD_TEMP_RESET, 12) == 0) {
            sensor->resetTemperatureCompensation();
            snprintf(result, sizeof(result), "PH:TEMP,RESET -> Done");
            printf("%s", result);
        } else {
            float temp;
            if (sscanf(cmd_str, "PH:TEMP,%f", &temp) == 1) {
                sensor->setTemperatureCompensation(temp);
                snprintf(result, sizeof(result), "PH:TEMP,%.1f -> Done", temp);
                printf("%s", result);
            } else {
                snprintf(result, sizeof(result), "PH:TEMP -> Invalid");
                printf("%s", result);
            }
        }
    } else if (strncmp(cmd_str, PH_CMD_CAL, 6) == 0) {
        if (strncmp(cmd_str, PH_CMD_CAL_STATE, 8) == 0) {
            int status = sensor->getCalibrationStatus();
            snprintf(result, sizeof(result), "PH:CAL,? -> %d", status);
            printf("%s", result);
        } else {
            char mode[10];
            float value = 0.0f;
            if (sscanf(cmd_str, "PH:CAL,%9[^,],%f", mode, &value) >= 1) {
                sensor->calibrate(mode, value);
                snprintf(result, sizeof(result), "PH:CAL,%s -> Done", mode);
                printf("%s", result);
            } else {
                snprintf(result, sizeof(result), "PH:CAL -> Invalid");
                printf("%s", result);
            }
        }
    } else if (strncmp(cmd_str, PH_CMD_STATUS, 9) == 0) {
        char status[32];
        sensor->getStatus(status, sizeof(status));
        snprintf(result, sizeof(result), "PH:STATUS -> %s", status);
        printf("%s", result);
    } else if (strncmp(cmd_str, PH_CMD_INFO, 7) == 0) {
        char info[32];
        sensor->getInfo(info, sizeof(info));
        snprintf(result, sizeof(result), "PH:INFO -> %s", info);
        printf("%s", result);
    } else if (strncmp(cmd_str, PH_CMD_BAUD, 7) == 0) {
        uint32_t baud;
        if (sscanf(cmd_str, "PH:BAUD,%lu", &baud) == 1) {
            sensor->setBaudWithConfirm(baud);
            snprintf(result, sizeof(result), "PH:BAUD,%lu -> Done", baud);
            printf("%s", result);
        } else {
            snprintf(result, sizeof(result), "PH:BAUD -> Invalid");
            printf("%s", result);
        }
    } else if (strncmp(cmd_str, PH_CMD_FACTORY, 10) == 0) {
        sensor->factoryResetWithConfirm();
        snprintf(result, sizeof(result), "PH:FACTORY -> Done");
        printf("%s", result);
    } else if (strncmp(cmd_str, PH_CMD_PLOCK, 8) == 0) {
        int lock;
        if (sscanf(cmd_str, "PH:PLOCK,%d", &lock) == 1) {
            sensor->setProtocolLockWithConfirm(lock != 0);
            snprintf(result, sizeof(result), "PH:PLOCK,%d -> Done", lock);
            printf("%s", result);
        } else {
            snprintf(result, sizeof(result), "PH:PLOCK -> Invalid");
            printf("%s", result);
        }
    } else if (strncmp(cmd_str, PH_CMD_I2C, 6) == 0) {
        uint8_t addr;
        if (sscanf(cmd_str, "PH:I2C,%hhu", &addr) == 1) {
            sensor->setI2cAddressWithConfirm(addr);
            snprintf(result, sizeof(result), "PH:I2C,%u -> Done", addr);
            printf("%s", result);
        } else {
            snprintf(result, sizeof(result), "PH:I2C -> Invalid");
            printf("%s", result);
        }
    } else if (strncmp(cmd_str, PH_CMD_HELP, 7) == 0) {
        // No action - redraw handled above
    }
}