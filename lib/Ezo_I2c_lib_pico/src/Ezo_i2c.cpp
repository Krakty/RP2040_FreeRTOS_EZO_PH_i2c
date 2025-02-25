#include "Ezo_i2c.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Ezo_board::Ezo_board(uint8_t address){
    this->i2c_address = address;
}

Ezo_board::Ezo_board(uint8_t address, const char* name){
    this->i2c_address = address;
    this->name = name;
}

Ezo_board::Ezo_board(uint8_t address, i2c_inst_t* i2c_inst) : Ezo_board(address){
    this->i2c = i2c_inst;
}

Ezo_board::Ezo_board(uint8_t address, const char* name, i2c_inst_t* i2c_inst) : Ezo_board(address, name){
    this->i2c = i2c_inst;
}

const char* Ezo_board::get_name(){
    return this->name;
}

void Ezo_board::set_name(const char* name){
    this->name = name;
}

uint8_t Ezo_board::get_address(){
    return i2c_address;
}

void Ezo_board::set_address(uint8_t address){
    this->i2c_address = address;
}

void Ezo_board::send_cmd(const char* command) {
    // Using Pico SDK I2C functions in place of Wire functions
    size_t len = strlen(command);
    i2c_write_blocking(this->i2c, this->i2c_address, (const uint8_t*)command, len, false);
    this->issued_read = false;
}

void Ezo_board::send_read_cmd(){
    send_cmd("r");
    this->issued_read = true;
}

void Ezo_board::send_cmd_with_num(const char* cmd, float num, uint8_t decimal_amount){
    // Using snprintf to format the command with the number appended
    char buffer[32];
    char format[8];
    snprintf(format, sizeof(format), "%%s%%.%uf", decimal_amount);
    snprintf(buffer, sizeof(buffer), format, cmd, num);
    send_cmd(buffer);
}

void Ezo_board::send_read_with_temp_comp(float temperature){
    send_cmd_with_num("rt,", temperature, 3);
    this->issued_read = true;
}

enum Ezo_board::errors Ezo_board::receive_read_cmd(){
    
    char _sensordata[this->bufferlen];
    this->error = receive_cmd(_sensordata, bufferlen);
    
    if(this->error == SUCCESS){
        if(this->issued_read == false){
            this->error = NOT_READ_CMD;
        }
        else{
            this->reading = atof(_sensordata);
        }
    }
    return this->error;
}

bool Ezo_board::is_read_poll(){
    return this->issued_read;
}

float Ezo_board::get_last_received_reading(){  
    return this->reading;
}

enum Ezo_board::errors Ezo_board::get_error(){
    return this->error;
}

enum Ezo_board::errors Ezo_board::receive_cmd( char * sensordata_buffer, uint8_t buffer_len) {
  // Original Arduino code:
  //  byte sensor_bytes_received = 0;
  //  byte code = 0;
  //  byte in_char = 0;
  //
  //  memset(sensordata_buffer, 0, buffer_len);        // clear sensordata array;
  //
  //  wire->requestFrom(this->i2c_address, (uint8_t)(buffer_len-1), (uint8_t)1);
  //  code = wire->read();
  //
  //  //wire->beginTransmission(this->i2c_address);
  //  while (wire->available()) {
  //    in_char = wire->read();
  //
  //    if (in_char == 0) {
  //      //wire->endTransmission();
  //      break;
  //    }
  //    else {
  //      sensordata_buffer[sensor_bytes_received] = in_char;
  //      sensor_bytes_received++;
  //    }
  //  }
  //  
  //  //should last array point be set to 0 to stop string overflows?
  //  switch (code) {
  //    case 1:
  //	  this->error = SUCCESS;
  //      break;
  //
  //    case 2:
  //	  this->error = FAIL;
  //      break;
  //
  //    case 254:
  //	  this->error = NOT_READY;
  //      break;
  //
  //    case 255:
  //	  this->error = NO_DATA;
  //	  break;
  //  }
  //  return this->error;
  
  // Refactored for Pico SDK:
  uint8_t sensor_bytes_received = 0;
  uint8_t code = 0;
  
  memset(sensordata_buffer, 0, buffer_len);        // clear sensordata array
  
  // Create a temporary buffer to hold the incoming I2C data
  uint8_t data[32] = {0};
  // Read data from the I2C device; read up to buffer_len bytes
  int ret = i2c_read_blocking(this->i2c, this->i2c_address, data, buffer_len, false);
  if (ret > 0) {
      code = data[0];  // first byte is the status code
      // Copy subsequent bytes until a 0 is encountered or buffer is full
      for (int i = 1; i < ret && sensor_bytes_received < buffer_len - 1; i++) {
          if (data[i] == 0) {
              break;
          }
          sensordata_buffer[sensor_bytes_received++] = data[i];
      }
  }
  
  // Interpret the response code
  switch (code) {
    case 1:
      this->error = SUCCESS;
      break;
    case 2:
      this->error = FAIL;
      break;
    case 254:
      this->error = NOT_READY;
      break;
    case 255:
      this->error = NO_DATA;
      break;
    default:
      this->error = FAIL;
      break;
  }
  return this->error;
}
