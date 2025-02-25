#ifndef IOT_CMD_H
#define IOT_CMD_H

#include "pico/stdlib.h"
#include "Ezo_i2c.h"

#define CMD_MAX_LENGTH 256

// used to receive commands from the serial port, print them,
// strip them of whitespace, and convert them to uppercase for processing.
// Returns true if a command was received and false if it wasn't.
bool receive_command(char *buffer, size_t max_len);

void process_command(const char *command, Ezo_board device_list[], 
                     uint8_t device_list_len, Ezo_board* &default_board);

void process_command(const char *command, Ezo_board* device_list[], 
                     uint8_t device_list_len, Ezo_board* &default_board);
// handles the common functions of the IOT kit command processing
// such as listing boards, sending commands to one or all boards, and
// switching which board commands are sent to by default.

void list_devices(Ezo_board device_list[], uint8_t device_list_len, Ezo_board* default_board);
                      
void list_devices(Ezo_board* device_list[], uint8_t device_list_len, Ezo_board* default_board);
// prints a list of all the boards in the device list array, with an arrow showing which board is
// the default board.

void iot_cmd_print_listcmd_help();
// prints the help section for the list command.

void iot_cmd_print_allcmd_help();
// prints the help section for the all query.

void iot_cmd_print_namedquery_help();
// prints the help section for the named reference query.

#endif
