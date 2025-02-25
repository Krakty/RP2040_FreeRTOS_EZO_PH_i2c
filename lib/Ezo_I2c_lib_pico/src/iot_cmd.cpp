#include "iot_cmd.h"
#include "Ezo_i2c_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Helper function: Trim whitespace from both ends of a string in place.
static void trim_in_place(char *str) {
    // Remove leading whitespace.
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    // Remove trailing whitespace.
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

// Helper function: Convert a string to uppercase in place.
static void to_upper_in_place(char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Helper function to delay for a specified number of milliseconds.
static void delay_ms_custom(unsigned int ms) {
    sleep_ms(ms);
}

// used to receive commands from the serial port.
// Reads a line from stdin, prints it, trims whitespace, and converts it to uppercase.
// Returns true if a command was received.
bool receive_command(char *buffer, size_t max_len) {
    if (fgets(buffer, max_len, stdin) != NULL) {
        // Print the received command.
        printf("> %s", buffer);
        // Remove newline characters.
        buffer[strcspn(buffer, "\r\n")] = '\0';
        // Trim whitespace.
        trim_in_place(buffer);
        // Convert to uppercase.
        to_upper_in_place(buffer);
        return true;
    }
    return false;
}

// Determines how long to delay based on the command string.
// If the command contains "CAL" or "R", delay 1200ms; otherwise, 300ms.
static void select_delay(const char *str) {
    if (strstr(str, "CAL") != NULL || strstr(str, "R") != NULL) {
        delay_ms_custom(1200);
    } else {
        delay_ms_custom(300);
    }
}

void process_command(const char *command, Ezo_board device_list[], uint8_t device_list_len, Ezo_board* &default_board) {
    // Create an array of pointers from the device list.
    Ezo_board* device_list_ptrs[32];
    if (device_list_len > 32) {
        return;
    }
    for (uint8_t i = 0; i < device_list_len; i++){
        device_list_ptrs[i] = &device_list[i];
    }
    process_command(command, device_list_ptrs, device_list_len, default_board);
}

void process_command(const char *command, Ezo_board* device_list[], uint8_t device_list_len, Ezo_board* &default_board) {
    if (strcmp(command, "LIST") == 0) {  // if our command is "LIST"
        list_devices(device_list, device_list_len, default_board);
    }
    // If the command starts with "ALL:" then send the query to all devices.
    else if (strncmp(command, "ALL:", 4) == 0) {
        const char *cmd = command + 4; // substring after "ALL:"
        for (uint8_t i = 0; i < device_list_len; i++) {
            device_list[i]->send_cmd(cmd);
        }
        select_delay(cmd);
        for (uint8_t i = 0; i < device_list_len; i++) {
            receive_and_print_response(*device_list[i]);
        }
    }
    // Otherwise, if a non-empty command was received...
    else if (command[0] != '\0') {
        // Check if the command contains a colon (":") for device selection.
        const char *colon = strchr(command, ':');
        if (colon != NULL) {
            // Extract device name from the command.
            size_t name_len = colon - command;
            char name_to_find[64];
            if (name_len >= sizeof(name_to_find))
                name_len = sizeof(name_to_find) - 1;
            strncpy(name_to_find, command, name_len);
            name_to_find[name_len] = '\0';
            to_upper_in_place(name_to_find);
            if (strlen(name_to_find) != 0) {
                bool addr_found = false;
                for (uint8_t i = 0; i < device_list_len; i++) {
                    // Get the device name and convert to uppercase.
                    const char *dev_name = device_list[i]->get_name();
                    char dev_name_upper[64];
                    strncpy(dev_name_upper, dev_name, sizeof(dev_name_upper)-1);
                    dev_name_upper[sizeof(dev_name_upper)-1] = '\0';
                    to_upper_in_place(dev_name_upper);
                    if (strcmp(name_to_find, dev_name_upper) == 0) {
                        default_board = device_list[i];
                        addr_found = true;
                        break;
                    }
                }
                if (addr_found) {
                    const char *cmd = colon + 1;
                    default_board->send_cmd(cmd);
                } else {
                    printf("No device named %s\n", name_to_find);
                    return;
                }
            } else {
                printf("Invalid name\n");
            }
        }
        else { // If there's no colon, send the command to the default device.
            default_board->send_cmd(command);
        }
        if (strcmp(command, "SLEEP") != 0) {
            select_delay(command);
            receive_and_print_response(*default_board);
        }
    }
}

void list_devices(Ezo_board device_list[], uint8_t device_list_len, Ezo_board* default_board) {
    for (uint8_t i = 0; i < device_list_len; i++) {
        if (default_board == &device_list[i]) {
            printf("--> ");
        } else {
            printf(" - ");
        }
        print_device_info(device_list[i]);
        printf("\n");
    }
}

void list_devices(Ezo_board* device_list[], uint8_t device_list_len, Ezo_board* default_board) {
    for (uint8_t i = 0; i < device_list_len; i++) {
        if (default_board == device_list[i]) {
            printf("--> ");
        } else {
            printf(" - ");
        }
        print_device_info(*device_list[i]);
        printf("\n");
    }
}

void iot_cmd_print_listcmd_help() {
    printf("list              prints the names and addresses of all the devices in the \n");
    printf("                  device list, with an arrow pointing to the default board \n");
}

void iot_cmd_print_allcmd_help() {
    printf("all:[query]       sends a query to all devices in the device list, and     \n");
    printf("                  prints their responses\n");
}

void iot_cmd_print_namedquery_help() {
    printf("[name]:[query]    sends a query to the device with the name [name], and \n");
    printf("                  makes that device the default board\n");
    printf("                      ex: PH:status sends a status query to the device \n");
    printf("                      named PH\n");
}
