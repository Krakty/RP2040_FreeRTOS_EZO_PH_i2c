#include "ph_sensor.h"
#include "config.h"  // PH_READ_DELAY_MS, etc.
#include "hardware/uart.h"  // uart_puts for prompts
#include "cstdlib"  // atoi, atof

// Constructor
PhSensor::PhSensor(i2c_inst_t* i2c, uint8_t address) 
    : m_device(address, "pH", i2c) {
    // No additional init needed - Ezo_board handles I2C setup
}

// Device Info & Status
void PhSensor::getInfo(char* buffer, size_t size) {
    m_device.send_cmd("i");
    vTaskDelay(pdMS_TO_TICKS(300));  // Datasheet: ~300ms
    m_device.receive_cmd(buffer, size);
}

void PhSensor::getStatus(char* buffer, size_t size) {
    m_device.send_cmd("Status");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(buffer, size);
}

// pH Readings
float PhSensor::readPh() {
    m_device.send_read_cmd();
    vTaskDelay(pdMS_TO_TICKS(PH_READ_DELAY_MS));  // 900ms min, config.h has 800ms - adjust to 1000ms?
    if (m_device.receive_read_cmd() == Ezo_board::SUCCESS) {
        return m_device.get_last_received_reading();
    }
    return -1.0f;  // Error indicator
}

// Calibration
bool PhSensor::calibrate(const char* mode, float value) {
    char cmd[20];
    if (strcmp(mode, "clear") == 0) {
        snprintf(cmd, sizeof(cmd), "Cal,clear");
    } else {
        snprintf(cmd, sizeof(cmd), "Cal,%s,%.2f", mode, value);
    }
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(1000));  // ~1s per datasheet
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    return response[0] == '1';
}

int PhSensor::getCalibrationStatus() {
    char response[10];
    m_device.send_cmd("Cal,?");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(response, sizeof(response));
    if (response[0] == '?') {
        return atoi(response + 5);  // "?Cal,<n>" -> extract n
    }
    return 0;  // Default: uncalibrated
}

void PhSensor::getSlope(float& acidPercent, float& basePercent, float& mVOffset) {
    char response[32];
    m_device.send_cmd("Slope,?");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(response, sizeof(response));
    if (response[0] == '?') {
        sscanf(response, "?Slope,%f,%f,%f", &acidPercent, &basePercent, &mVOffset);
    } else {
        acidPercent = 0.0f;
        basePercent = 0.0f;
        mVOffset = 0.0f;
    }
}

// Temperature Compensation
bool PhSensor::setTemperatureCompensation(float temp) {
    char cmd[10];
    snprintf(cmd, sizeof(cmd), "T,%.1f", temp);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    return response[0] == '1';
}

float PhSensor::getTemperatureCompensation() {
    char response[10];
    m_device.send_cmd("T,?");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(response, sizeof(response));
    if (response[0] == '?') {
        return atof(response + 3);  // "?T,<n>" -> extract n
    }
    return 0.0f;
}

// LED Control
bool PhSensor::setLed(bool on) {
    m_device.send_cmd(on ? "L,1" : "L,0");
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    return response[0] == '1';
}

bool PhSensor::getLedState() {
    char response[10];
    m_device.send_cmd("L,?");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(response, sizeof(response));
    return (response[0] == '?' && response[3] == '1');
}

void PhSensor::find() {
    m_device.send_cmd("Find");
    vTaskDelay(pdMS_TO_TICKS(300));  // Blinks for ~10s async
    // No response expected
}

// Configuration (Risky Ones)
bool PhSensor::setBaud(uint32_t baud) {
    char cmd[15];
    snprintf(cmd, sizeof(cmd), "Baud,%lu", baud);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));  // Device restarts
    return true;  // No response - assume success
}

bool PhSensor::setBaudWithConfirm(uint32_t baud) {
    char warning[64];
    snprintf(warning, sizeof(warning), "WARNING: Baud to %lu may break comms—y/n?", baud);
    if (confirmAction(warning)) {
        return setBaud(baud);
    }
    return false;
}

bool PhSensor::factoryReset() {
    m_device.send_cmd("Factory");
    vTaskDelay(pdMS_TO_TICKS(300));  // Device restarts
    return true;  // No response
}

bool PhSensor::factoryResetWithConfirm() {
    if (confirmAction("WARNING: Factory reset wipes all—y/n?")) {
        return factoryReset();
    }
    return false;
}

bool PhSensor::setProtocolLock(bool lock) {
    m_device.send_cmd(lock ? "Plock,1" : "Plock,0");
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    return response[0] == '1';
}

bool PhSensor::setProtocolLockWithConfirm(bool lock) {
    if (confirmAction(lock ? "WARNING: Locking protocol—y/n?" : "WARNING: Unlocking protocol—y/n?")) {
        return setProtocolLock(lock);
    }
    return false;
}

bool PhSensor::setI2cAddress(uint8_t address) {
    char cmd[10];
    snprintf(cmd, sizeof(cmd), "I2C,%u", address);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    return response[0] == '1';
}

bool PhSensor::setI2cAddressWithConfirm(uint8_t address) {
    char warning[64];
    snprintf(warning, sizeof(warning), "WARNING: I2C to %u may break—y/n?", address);
    if (confirmAction(warning)) {
        return setI2cAddress(address);
    }
    return false;
}

bool PhSensor::setName(const char* name) {
    char cmd[20];
    snprintf(cmd, sizeof(cmd), "Name,%s", name);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    return response[0] == '1';
}

// Calibration Data
void PhSensor::exportCalibration(char* buffer, size_t& size) {
    m_device.send_cmd("Export");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(buffer, size);
    // Note: Multi-line response - buffer must be large enough (~100 bytes)
}

bool PhSensor::importCalibration(const char* data) {
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "Import,%s", data);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    return response[0] == '1';
}

// Power Management
void PhSensor::sleep() {
    m_device.send_cmd("Sleep");
    // No delay - instant shutdown
}

// Custom (Placeholder)
bool PhSensor::enableExtendedScale(bool enable) {
    // Placeholder - not in datasheet, assume custom command
    return true;  // TBD
}

// Private Helper
bool PhSensor::confirmAction(const char* warning) {
    // Print warning to UART1 (assuming CLI context - refine later with Hardware*)
    uart_puts(uart1, warning);
    uart_puts(uart1, "\r\n");

    // Wait for 'y' or 'n' - 10s timeout
    TickType_t start = xTaskGetTickCount();
    char response;
    while (xTaskGetTickCount() - start < pdMS_TO_TICKS(10000)) {
        if (uart_is_readable(uart1)) {
            response = uart_getc(uart1);
            uart_putc(uart1, response);  // Echo back
            uart_puts(uart1, "\r\n");
            if (response == 'y' || response == 'Y') return true;
            if (response == 'n' || response == 'N') return false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Small delay to avoid busy-wait
    }
    uart_puts(uart1, "Timeout - canceled\r\n");
    return false;
}