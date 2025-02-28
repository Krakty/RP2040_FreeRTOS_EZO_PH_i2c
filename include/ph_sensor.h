#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include "Ezo_i2c.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
    #include "semphr.h"
}

class PhSensor {
public:
    PhSensor(i2c_inst_t* i2c, uint8_t address);
    ~PhSensor();
    void getInfo(char* buffer, size_t size);
    void getStatus(char* buffer, size_t size);
    float readPh();
    bool calibrate(const char* mode, float value = 0.0);
    int getCalibrationStatus();
    void getSlope(float& acidPercent, float& basePercent, float& mVOffset);
    bool setTemperatureCompensation(float temp);
    float getTemperatureCompensation();
    bool resetTemperatureCompensation();  // New - reset to 25°C
    bool setLed(bool on);
    bool getLedState();
    void find();
    bool setBaud(uint32_t baud);
    bool setBaudWithConfirm(uint32_t baud);
    bool factoryReset();
    bool factoryResetWithConfirm();
    bool setProtocolLock(bool lock);
    bool setProtocolLockWithConfirm(bool lock);
    bool setI2cAddress(uint8_t address);
    bool setI2cAddressWithConfirm(uint8_t address);
    bool setName(const char* name);
    void exportCalibration(char* buffer, size_t& size);
    bool importCalibration(const char* data);
    void sleep();
    bool enableExtendedScale(bool enable);

private:
    Ezo_board m_device;
    SemaphoreHandle_t m_mutex;
    bool confirmAction(const char* warning);
};

#endif // PH_SENSOR_H