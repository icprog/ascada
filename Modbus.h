//      Author: bolke
//      Date: 20-01-2018
//      Description: Code for acting like a modbus slave. Use default modbus functions to
//                   read and write to and from the arduino. Set outputs, read inputs,
//                   read analogs, etc. 
//                   Couple your own functions for reading and writing through the function pointers.
//                   These functions will be given the translated addresses (holding register or input
//                   register, etc.

#ifndef MODBUSBLIB_H
#define MODBUSBLIB_H

#include "Ascada.h"                                       //universal defintions

#define DEFAULT_BAUDRATE 9600                             //default baudrate, 8n1 setting
#define DEFAULT_SLAVE_ID 247                              //default slave id is 247

#define MB_SETTING_SIZE 5                                 //number of bytes stored in eeprom for modbus

#define MESSAGE_LENGTH 0x28                               //total buffer, max 40 characters, also max response length

#define REGION_RANGE 0x270E                               //size of a region

const uint16_t REGION_START[6] {0x0001,                   //discrete output coils, read write
                                0x2711,                   //discrete input contacts, read only
                                0x9C41,                   //output holding registers, read write
                                0x7531,                   //input registers, read only
                                0x0001,                   //discrete output coils, entry for writing
                                0x9C41};                  //output holding registers, entry for writing

#define REGION_OUTPUT_COIL_START REGION_START[0x00]       //output coil region start
#define REGION_DISCRETE_INPUT_START REGION_START[0x01]    //discrete input region start
#define REGION_INPUT_REGISTER_START REGION_START[0x03]    //input register region start
#define REGION_HOLDING_REGISTER_START REGION_START[0x02]  //holding register region start
#define REGION_END(start) (start+REGION_RANGE)            //return inserted region + range as ending

class ModbusRTU{
private:
protected:
public:
uint8_t GetExpectedLength();
uint8_t InitSerial(uint32_t baudrate);
void SendBuffer(uint8_t* buf,uint16_t len);
void HandleException(uint8_t exceptionCode=0x01);
uint8_t HandleBroadcast();
uint8_t ReadRegisters();
uint8_t ReadBits();
uint8_t HandleMisc();
uint8_t HandleRequest();

  union16_t address;                                      //address used in requests goes here, because msb/lsb flip
  union16_t value;                                        //value used in request goes here, because of flipped msb/lsb
  uint16_t silence;                                       //silence period in timer 0 ticks (4us per tick)
  volatile uint8_t silence_cnt;                           //silence amount of ticks executed at this moment
  uint8_t silence_ticks;                                  //silence amount of ticks needed to complete silence period
  uint8_t msgPtr = 0;                                     //ptr to know where we are with the message
  uint8_t expectedLength = 0;                             //expected length of request
	
	union{
    uint8_t msg[MESSAGE_LENGTH] = {0};                    //message buffer, for both request and response   
    struct{                                               //request message block      
      uint8_t msgSlave;                                   //request slave id
      uint8_t msgFunc;                                    //request function
    };
  };  
  
	union{                                                  //settings block start, save able data
    uint8_t settings[MB_SETTING_SIZE]={0};                //settings as a byte array
    struct{                                               
      uint32_t baudrate;                                  //baud-rate
      uint8_t slaveId;                                    //slave id used
    };                                                    
  };  	
//  ModbusRTU();
//	~ModbusRTU();
  uint8_t mbSetup(uint32_t baudrate=DEFAULT_BAUDRATE, 
									uint8_t slaveId=DEFAULT_SLAVE_ID);
  WriteFuncPtr mbWriteBit;                           			//writing bits function
  WriteFuncPtr mbWriteRegister;                      			//writing register function
  ReadFuncPtr mbReadBit;                             			//reading a bit
  ReadFuncPtr mbReadRegister;                        			//reading a register
  ExecuteFuncPtr mbExecuteFunction;                  			//execute given function   
};

extern ModbusRTU mb_ds;

/*
typedef struct{                                           //data structure used in this file
  union16_t address;                                      //address used in requests goes here, because msb/lsb flip
  union16_t value;                                        //value used in request goes here, because of flipped msb/lsb
  uint16_t silence;                                       //silence period in timer 0 ticks (4us per tick)
  volatile uint8_t silence_cnt;                           //silence amount of ticks executed at this moment
  uint8_t silence_ticks;                                  //silence amount of ticks needed to complete silence period
  uint8_t msgPtr = 0;                                     //ptr to know where we are with the message
  uint8_t expectedLength = 0;                             //expected length of request
  
  union{
    uint8_t msg[MESSAGE_LENGTH] = {0};                    //message buffer, for both request and response   
    struct{                                               //request message block      
      uint8_t msgSlave;                                   //request slave id
      uint8_t msgFunc;                                    //request function
    };
  };                                                        
  
  union{                                                  //settings block start, save able data
    uint8_t settings[MB_SETTING_SIZE]={0};                //settings as a byte array
    struct{                                               
      uint32_t baudrate;                                  //baud-rate
      uint8_t slaveId;                                    //slave id used
    };                                                    
  };                                                        
} mb_t;                                                   //mosbus data structure

extern WriteFuncPtr mbWriteBit;                           //writing bits function
extern WriteFuncPtr mbWriteRegister;                      //writing register function
extern ReadFuncPtr mbReadBit;                             //reading a bit
extern ReadFuncPtr mbReadRegister;                        //reading a register
extern ExecuteFuncPtr mbExecuteFunction;                  //execute given function 

extern mb_t mb_ds;                                        //modbus data structure
uint8_t mbSetup(uint32_t baudrate=DEFAULT_BAUDRATE, uint8_t slaveId=DEFAULT_SLAVE_ID);
*/

#endif

