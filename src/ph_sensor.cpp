#include "ph_sensor.h"
#include "config.h"
#include "hardware/uart.h"
#include <cstdlib>
#include <string.h>

PhSensor::PhSensor(i2c_inst_t* i2c, uint8_t address) 
    : m_device(address, "pH", i2c) {
    m_mutex = xSemaphoreCreateMutex();
}

PhSensor::~PhSensor() {
    vSemaphoreDelete(m_mutex);
}

void PhSensor::getInfo(char* buffer, size_t size) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_cmd("i");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(buffer, size);
    xSemaphoreGive(m_mutex);
}

void PhSensor::getStatus(char* buffer, size_t size) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_cmd("Status");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(buffer, size);
    xSemaphoreGive(m_mutex);
}

float PhSensor::readPh() {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_read_cmd();
    vTaskDelay(pdMS_TO_TICKS(PH_READ_DELAY_MS));
    float result = -1.0f;
    if (m_device.receive_read_cmd() == Ezo_board::SUCCESS) {
        result = m_device.get_last_received_reading();
    }
    xSemaphoreGive(m_mutex);
    return result;
}

bool PhSensor::calibrate(const char* mode, float value) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char cmd[20];
    if (strcmp(mode, "clear") == 0) {
        snprintf(cmd, sizeof(cmd), "Cal,clear");
    } else {
        snprintf(cmd, sizeof(cmd), "Cal,%s,%.2f", mode, value);
    }
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(1000));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    xSemaphoreGive(m_mutex);
    return response[0] == '1';
}

int PhSensor::getCalibrationStatus() {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char response[10];
    m_device.send_cmd("Cal,?");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(response, sizeof(response));
    int result = (response[0] == '?') ? atoi(response + 5) : 0;
    xSemaphoreGive(m_mutex);
    return result;
}

void PhSensor::getSlope(float& acidPercent, float& basePercent, float& mVOffset) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
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
    xSemaphoreGive(m_mutex);
}

bool PhSensor::setTemperatureCompensation(float temp) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char cmd[10];
    snprintf(cmd, sizeof(cmd), "T,%.1f", temp);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    xSemaphoreGive(m_mutex);
    return response[0] == '1';
}

float PhSensor::getTemperatureCompensation() {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char response[10];
    m_device.send_cmd("T,?");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(response, sizeof(response));
    float result = (response[0] == '?') ? atof(response + 3) : 0.0f;
    xSemaphoreGive(m_mutex);
    return result;
}

bool PhSensor::setLed(bool on) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_cmd(on ? "L,1" : "L,0");
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    xSemaphoreGive(m_mutex);
    return response[0] == '1';
}

bool PhSensor::getLedState() {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char response[10];
    m_device.send_cmd("L,?");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(response, sizeof(response));
    bool result = (response[0] == '?' && response[3] == '1');
    xSemaphoreGive(m_mutex);
    return result;
}

void PhSensor::find() {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_cmd("Find");
    vTaskDelay(pdMS_TO_TICKS(300));
    xSemaphoreGive(m_mutex);
}

bool PhSensor::setBaud(uint32_t baud) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char cmd[15];
    snprintf(cmd, sizeof(cmd), "Baud,%lu", baud);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    xSemaphoreGive(m_mutex);
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
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_cmd("Factory");
    vTaskDelay(pdMS_TO_TICKS(300));
    xSemaphoreGive(m_mutex);
    return true;
}

bool PhSensor::factoryResetWithConfirm() {
    if (confirmAction("WARNING: Factory reset wipes all—y/n?")) {
        return factoryReset();
    }
    return false;
}

bool PhSensor::setProtocolLock(bool lock) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_cmd(lock ? "Plock,1" : "Plock,0");
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    xSemaphoreGive(m_mutex);
    return response[0] == '1';
}

bool PhSensor::setProtocolLockWithConfirm(bool lock) {
    if (confirmAction(lock ? "WARNING: Locking protocol—y/n?" : "WARNING: Unlocking protocol—y/n?")) {
        return setProtocolLock(lock);
    }
    return false;
}

bool PhSensor::setI2cAddress(uint8_t address) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char cmd[10];
    snprintf(cmd, sizeof(cmd), "I2C,%u", address);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    xSemaphoreGive(m_mutex);
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
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char cmd[20];
    snprintf(cmd, sizeof(cmd), "Name,%s", name);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    xSemaphoreGive(m_mutex);
    return response[0] == '1';
}

void PhSensor::exportCalibration(char* buffer, size_t& size) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_cmd("Export");
    vTaskDelay(pdMS_TO_TICKS(300));
    m_device.receive_cmd(buffer, size);
    xSemaphoreGive(m_mutex);
}

bool PhSensor::importCalibration(const char* data) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "Import,%s", data);
    m_device.send_cmd(cmd);
    vTaskDelay(pdMS_TO_TICKS(300));
    char response[10];
    m_device.receive_cmd(response, sizeof(response));
    xSemaphoreGive(m_mutex);
    return response[0] == '1';
}

void PhSensor::sleep() {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_device.send_cmd("Sleep");
    xSemaphoreGive(m_mutex);
}

bool PhSensor::enableExtendedScale(bool enable) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    // Placeholder - custom command TBD
    xSemaphoreGive(m_mutex);
    return true;  // Assume success until defined
}

bool PhSensor::confirmAction(const char* warning) {
    uart_puts(uart1, warning);
    uart_puts(uart1, "\r\n");

    TickType_t start = xTaskGetTickCount();
    char response;
    while (xTaskGetTickCount() - start < pdMS_TO_TICKS(10000)) {
        if (uart_is_readable(uart1)) {
            response = uart_getc(uart1);
            uart_putc(uart1, response);
            uart_puts(uart1, "\r\n");
            if (response == 'y' || response == 'Y') return true;
            if (response == 'n' || response == 'N') return false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    uart_puts(uart1, "Timeout - canceled\r\n");
    return false;
}