#ifndef CONFIG_H
#define CONFIG_H

// Define valid programming-mode commands.
#define CMD_HELP "?"         // Display programming help.
#define CMD_EXIT "EXIT"      // Exit programming mode.
#define CMD_SET_NAME "NAME"  // e.g., NAME,MyPHProbe
#define CMD_CAL_MID "CAL,MID"    // e.g., CAL,MID,7.00
#define CMD_CAL_LOW "CAL,LOW"    // e.g., CAL,LOW,4.00
#define CMD_CAL_HIGH "CAL,HIGH"  // e.g., CAL,HIGH,10.00
#define CMD_FACTORY "FACTORY"    // Factory reset.
#define CMD_SLOPE "SLOPE"        // Display calibration slope.

#endif
