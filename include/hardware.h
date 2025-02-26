#ifndef HARDWARE_H
#define HARDWARE_H

#include "hardware/i2c.h"    // Define i2c_inst_t
#include "hardware/uart.h"   // Define uart_inst_t
#include "hardware/spi.h"    // Define spi_inst_t
#include "pico/stdlib.h"     // Core SDK includes
#include "config.h"          // Pin definitions

class Hardware {
public:
    Hardware();
    void initStdio();
    void initLedGpio();
    void initI2c0();
    void initUart1();
    void initUart0();
    void initI2c1();
    void initSpi0();
    void initSpi1();
    i2c_inst_t* getI2c0() const;
    uart_inst_t* getUart1() const;

private:
    i2c_inst_t* m_i2c0;      // Unique name, I2C0 instance (active)
    i2c_inst_t* m_i2c1;      // Unique name, I2C1 instance (dormant)
    uart_inst_t* m_uart0;    // Unique name, UART0 instance (stdio)
    uart_inst_t* m_uart1;    // Unique name, UART1 instance (active)
    spi_inst_t* m_spi0;      // Unique name, SPI0 instance (dormant)
    spi_inst_t* m_spi1;      // Unique name, SPI1 instance (dormant)
};

#endif // HARDWARE_H