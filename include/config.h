#ifndef CONFIG_H
#define CONFIG_H

// LED Configuration
#define LED_PIN 25

#define BLINK_DELAY_MS 500   // Blink every 500ms
#define TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)  // One above idle priority

#define PH_SENSOR_I2C_ADDRESS 0x63  // Example address for pH sensor (EZO pH is typically 99 decimal = 0x63 hex)
#define PH_READ_DELAY_MS 800       // Delay for pH sensor reading (per EZO spec)
#define PH_LOOP_DELAY_MS 1000      // Loop delay between readings


// UART0 Configuration (stdio/serial output)
#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART0_BAUD_RATE 115200

// UART1 Configuration (CLI)
#define UART1_TX_PIN 8
#define UART1_RX_PIN 9
#define UART1_BAUD_RATE 115200

// I2C0 Configuration (pH sensor)
#define I2C0_SDA_PIN 4
#define I2C0_SCL_PIN 5
#define I2C0_BAUD_RATE 100000  // 100 kHz

// I2C1 Configuration (future expansion, e.g., second sensor)
#define I2C1_SDA_PIN 6
#define I2C1_SCL_PIN 7
#define I2C1_BAUD_RATE 100000  // 100 kHz

// SPI0 Configuration (future expansion, e.g., SD card)
#define SPI0_SCK_PIN 18
#define SPI0_TX_PIN 19  // MOSI
#define SPI0_RX_PIN 16  // MISO
#define SPI0_CS_PIN 17  // Chip Select
#define SPI0_BAUD_RATE 1000000  // 1 MHz

// SPI1 Configuration (future expansion, e.g., additional peripheral)
#define SPI1_SCK_PIN 10
#define SPI1_TX_PIN 11  // MOSI
#define SPI1_RX_PIN 12  // MISO
#define SPI1_CS_PIN 13  // Chip Select
#define SPI1_BAUD_RATE 1000000  // 1 MHz


// CLI Display Definitions
#define PH_CLI_SCROLL_PADDING "\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n"  // 10 lines

#define PH_CLI_SAFE_DISPLAY \
    "+----------------------------------------------------+\r\n" \
    "| EZO pH CLI Status                                  |\r\n" \
    "+----------------------------------------------------+\r\n" \
    "  PH:READ                            Read pH value    \r\n" \
    "  PH:CAL,<type> (mid,low,high,clear) Calibrate       \r\n" \
    "  PH:LED,<n> (ON,OFF,?)              LED on/off      \r\n" \
    "  PH:FIND                            Locate device    \r\n" \
    "  PH:SLOPE                           Calibration slope\r\n" \
    "  PH:TEMP,<n>                        Set temp comp    \r\n" \
    "  PH:STATUS                          Device status    \r\n" \
    "  PH:INFO                            Device info      \r\n" \
    "  PH:DANGER                          Enter danger mode\r\n" \
    "  PH:HELP                            Show this        \r\n" \
    "+----------------------------------------------------+\r\n"

#define PH_CLI_DANGER_DISPLAY \
    "+----------------------------------------------------+\r\n" \
    "| EZO pH CLI - Danger Mode                           |\r\n" \
    "+----------------------------------------------------+\r\n" \
    "  PH:BAUD,<n>                        Set baud rate    \r\n" \
    "  PH:FACTORY                         Factory reset    \r\n" \
    "  PH:PLOCK,<n> (0,1)                 Lock protocol    \r\n" \
    "  PH:I2C,<n>                         Set I2C addr     \r\n" \
    "  EXIT                               Back to safe mode\r\n" \
    "+----------------------------------------------------+\r\n"

#define PH_CLI_SAFE_PROMPT    "PH_PROG_MODE:>"
#define PH_CLI_DANGER_PROMPT  "PH_PROG_DANGER_MODE:>"

#endif // CONFIG_H