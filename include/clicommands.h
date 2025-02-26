#ifndef CLICOMMANDS_H
#define CLICOMMANDS_H

// Command defines for Ezo_I2c_lib_pico I2C implementation
// Based on EZO-pH Embedded pH Circuit Datasheet V6.1, I2C Mode (pages 39-62)

// Calibration Commands
#define EZO_CAL_MIDPOINT "Cal,mid,"     // Single point calibration at midpoint (e.g., "Cal,mid,7.00")
#define EZO_CAL_LOWPOINT "Cal,low,"     // Two point calibration at lowpoint (e.g., "Cal,low,4.00")
#define EZO_CAL_HIGHPOINT "Cal,high,"   // Three point calibration at highpoint (e.g., "Cal,high,10.00")
#define EZO_CAL_CLEAR "Cal,clear"       // Delete calibration data
#define EZO_CAL_STATUS "Cal,?"          // Check calibration status (?Cal,0/1/2/3)

// Reading Commands
#define EZO_READ "R"                    // Returns a single pH reading (900ms delay)

// Device Control Commands
#define EZO_LED_ON "L,1"                // Enable LED
#define EZO_LED_OFF "L,0"               // Disable LED
#define EZO_LED_STATUS "L,?"            // Check LED state (?L,1 or ?L,0)
#define EZO_FIND "Find"                 // Blink LED white to locate device
#define EZO_SLEEP "Sleep"               // Enter sleep mode/low power
#define EZO_PROTOCOL_LOCK_ON "Plock,1"  // Lock to I2C mode
#define EZO_PROTOCOL_LOCK_OFF "Plock,0" // Unlock protocol
#define EZO_PROTOCOL_LOCK_STATUS "Plock,?" // Check protocol lock (?Plock,1 or ?Plock,0)

// Device Information Commands
#define EZO_INFO "i"                    // Device information (?i,pH,<firmware>)
#define EZO_STATUS "Status"             // Voltage at VCC and restart reason (?Status,<reason>,<voltage>)
#define EZO_SLOPE "Slope,?"             // Slope of pH probe (?Slope,<acid>,<base>,<offset>)

// Calibration Export/Import Commands
#define EZO_EXPORT "Export"             // Export calibration string
#define EZO_EXPORT_INFO "Export,?"      // Calibration string info (<strings>,<bytes>)
#define EZO_IMPORT "Import,"            // Import calibration string (append string, e.g., "Import,59 6F 75...")

// Temperature Compensation
#define EZO_TEMP_COMP "T,"              // Set temperature compensation (append value, e.g., "T,19.5")
#define EZO_TEMP_COMP_READ "RT,"        // Set temp and take reading (append value, e.g., "RT,19.5")
#define EZO_TEMP_STATUS "T,?"           // Current compensated temperature (?T,<temp>)

// Extended pH Scale
#define EZO_PHEXT_ON "pHext,1"          // Enable extended pH scale (-1.6 to 15.6)
#define EZO_PHEXT_OFF "pHext,0"         // Disable extended pH scale (0 to 14)
#define EZO_PHEXT_STATUS "pHext,?"      // Check extended pH scale (?pHext,1 or ?pHext,0)

// Device Naming
#define EZO_NAME "Name,"                // Set device name (append name, e.g., "Name,PHMeter1")
#define EZO_NAME_CLEAR "Name,"          // Clear device name
#define EZO_NAME_STATUS "Name,?"        // Show current name (?Name,<name>)

// Mode Switching and Reset
#define EZO_SWITCH_TO_UART "Baud,"      // Switch to UART (append baud, e.g., "Baud,9600")
#define EZO_FACTORY_RESET "Factory"     // Factory reset (retains I2C address)

// Processing Delays (in milliseconds)
#define EZO_DELAY_READ 900              // Delay for single reading (R)
#define EZO_DELAY_CALIBRATION 900       // Delay for calibration commands
#define EZO_DELAY_DEFAULT 300           // Default delay for most other commands

#endif // CLICOMMANDS_H