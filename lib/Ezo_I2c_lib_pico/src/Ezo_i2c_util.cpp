#include "Ezo_i2c_util.h"
#include "pico/stdlib.h"
#include <stdio.h>

// prints the board's name and I2C address
void print_device_info(Ezo_board &Device) {
    // Print the device name and address (assumes get_name() returns a C-string)
    printf("%s %d", Device.get_name(), Device.get_address());
}

// prints either a success message (formatted as a string) if a command was successful
// or prints an error message based on the device's error status
void print_success_or_error(Ezo_board &Device, const char* success_string) {
    switch (Device.get_error()) {
        case Ezo_board::SUCCESS:
            printf("%s", success_string);   // command was successful, print success string
            break;
        case Ezo_board::FAIL:
            printf("Failed ");              // command failed
            break;
        case Ezo_board::NOT_READY:
            printf("Pending ");             // command still processing
            break;
        case Ezo_board::NO_DATA:
            printf("No Data ");             // no data available from sensor
            break;
        case Ezo_board::NOT_READ_CMD:
            printf("Not Read Cmd ");        // read command was not issued before attempting to read
            break;
    }
}

// receives a response from the device and prints it along with the device info
void receive_and_print_response(Ezo_board &Device) {
    char receive_buffer[32];                    // buffer to hold the device's response
    Device.receive_cmd(receive_buffer, 32);     // retrieve the response into the buffer

    print_success_or_error(Device, " - ");
    print_device_info(Device);
    printf(": %s\n", receive_buffer);           // print the response followed by a newline
}

// receives a reading from the device, formats the reading to two decimal places,
// and prints the device name and the formatted reading or error message.
void receive_and_print_reading(Ezo_board &Device) {
    printf("%s: ", Device.get_name());
    Device.receive_read_cmd();  // update Device's internal reading value

    // Format the last received reading as a float with 2 decimals
    char reading_buffer[16];
    snprintf(reading_buffer, sizeof(reading_buffer), "%.2f", Device.get_last_received_reading());
    print_success_or_error(Device, reading_buffer);
    printf("\n");
}
