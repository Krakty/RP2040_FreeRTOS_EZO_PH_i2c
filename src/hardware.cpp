#include "hardware.h"

// Constructor
Hardware::Hardware() {
    m_i2c0 = i2c0;      // SDK's global i2c0 pointer
    m_i2c1 = i2c1;      // SDK's global i2c1 pointer
    m_uart0 = uart0;    // SDK's global uart0 pointer
    m_uart1 = uart1;    // SDK's global uart1 pointer
    m_spi0 = spi0;      // SDK's global spi0 pointer
    m_spi1 = spi1;      // SDK's global spi1 pointer
}

// Active initialization methods
void Hardware::initStdio() {
    stdio_init_all();
}

void Hardware::initLedGpio() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
}

void Hardware::initI2c0() {
    i2c_init(m_i2c0, I2C0_BAUD_RATE);
    gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA_PIN);
    gpio_pull_up(I2C0_SCL_PIN);
}

void Hardware::initUart1() {
    uart_init(m_uart1, UART1_BAUD_RATE);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
}

// Dormant initialization methods
void Hardware::initUart0() {
    uart_init(m_uart0, UART0_BAUD_RATE);
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);
}

void Hardware::initI2c1() {
    i2c_init(m_i2c1, I2C1_BAUD_RATE);
    gpio_set_function(I2C1_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1_SDA_PIN);
    gpio_pull_up(I2C1_SCL_PIN);
}

void Hardware::initSpi0() {
    spi_init(m_spi0, SPI0_BAUD_RATE);
    gpio_set_function(SPI0_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_CS_PIN, GPIO_FUNC_SPI);
}

void Hardware::initSpi1() {
    spi_init(m_spi1, SPI1_BAUD_RATE);
    gpio_set_function(SPI1_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI1_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI1_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI1_CS_PIN, GPIO_FUNC_SPI);
}

// Getter implementations
i2c_inst_t* Hardware::getI2c0() const {
    return m_i2c0;
}

uart_inst_t* Hardware::getUart1() const {
    return m_uart1;
}