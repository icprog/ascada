//--------------------------------------------------------------------------------------
//      Author: bolke
//      Date: 05-04-2018
//      Description: 
//--------------------------------------------------------------------------------------
#include "Ascada.h"
#include "ControlLoop.h"
#include "Modbus.h"
#include <EEPROM.h>
//--------------------------------------------------------------------------------------
#define VDATE "Apr 03 2018"																															//__DATE__ //"Apr 03 2018"
#define VTIME "12:14:23"																																//__TIME__ //"12:14:23"
//holding registers
#define HR_DEVICE_START 0x0000
#define HR_DEVICE_END 0x00C6
#define HR_EEPROM_START 0x00C7
#define HR_EEPROM_END 0x04C6
//input registers
#define IR_VERSION_START 0x0000
#define IR_VERSION_END 0x0007
#define IR_STATUS 0x0008
#define IR_CL_STATUS 0x0009
#define IR_ALARM_CNT 0x000A
#define IR_ALARM_START 0x000B
#define IR_ALARM_END (IR_ALARM_START+ALARM_WORD_CNT-1)
#define IR_UPTIME_MILLIS_LOW (IR_ALARM_END+1)
#define IR_UPTIME_MILLIS_HIGH (IR_UPTIME_MILLIS_LOW+1)
//--------------------------------------------------------------------------------------
ReadFuncPtr ReadReg(uint16_t address, uint16_t* value)
{																																													//read a register, return value through pointer
  uint16_t realAddress=address;																														//placeholder for converted address
  uint8_t result=EXCEPTION_INVALID_ADDRESS;																								//default failing on address
  *value=0;																																								//and default value to 0
  if (address>=REGION_HOLDING_REGISTER_START)																							//check if address is within range
  {																																												//but reading holding registers      
    if (address<=REGION_END(REGION_HOLDING_REGISTER_START))
    {
      result=EXCEPTION_NONE;																															//success
      realAddress=address-REGION_HOLDING_REGISTER_START;																	//convert address to something between 0 and 9998
      if (realAddress<=HR_DEVICE_END)																											//read value from actual device register          
        *value=GET_REGISTER(realAddress)&0xFF;																						//grab value from device, binary and with 0xFF
      else if (realAddress<=HR_EEPROM_END)
      {																																										//read value from eeprom address
        realAddress=realAddress-HR_EEPROM_START;																					//convert to actual address
        *value=EEPROM.read(realAddress)&0xFF;																							//read from eeprom and return value
      }
      else
        result=clReadReg(address, value);																									//read register from control loop, project specific addresses etc
    }
  }
  else if ((address>=REGION_INPUT_REGISTER_START)&&(address<=REGION_END(REGION_INPUT_REGISTER_START)))
  {
    result=EXCEPTION_NONE;																																//
    realAddress=address-REGION_INPUT_REGISTER_START;																			//convert address to something between 0 and 9998
    if (realAddress<=0xFF)
    {																																											//first 255 registers are reserved for device and status information
      if (realAddress<=IR_VERSION_END)
      {																																										//request compile time information, used to distinquish version
        switch (realAddress)
        {																																									//
          case IR_VERSION_START:																													//
            *value=(VDATE[0]<<8)|(VDATE[1]);																							//month, first 2 characters (Jan/Feb/Mar/etc) in ascii
            break;																																				//
          case IR_VERSION_START+1:																												//
            *value=(VDATE[2]<<8)|(VDATE[4]);																							//month, last character and largest day nr in ascii
            break;																																				//
          case IR_VERSION_START+2:																												//
            *value=(VDATE[5]<<8)|(VDATE[7]);																							//smallest day nr and century year nr in ascii
            break;																																				//
          case IR_VERSION_START+3:																												//
            *value=(VDATE[8]<<8)|(VDATE[9]);																							//age year nr and decade year nr in ascii
            break;																																				//
          case IR_VERSION_START+4:																												//
            *value=(VDATE[10]<<8)|(' ');																									//single year nr, in ascii and space
            break;																																				//
          case IR_VERSION_START+5:																												//
            *value=(VTIME[0]<<8)|(VTIME[1]);																							//time hour, in ascii
            break;																																				//
          case IR_VERSION_START+6:																												//
            *value=(VTIME[3]<<8)|(VTIME[4]);																							//time minute, in ascii
            break;																																				//
          case IR_VERSION_START+7:																												//
            *value=(VTIME[6]<<8)|(VTIME[7]);																							//time second, in ascii
            break;																																				//
        }																																									//
      }
      else if (realAddress==IR_STATUS)																										//else request runtime information
        *value=cl_ds.status;																															//status information, described below
        //runtime information
        /* bit description
         * [0] running
         * [1] halted bit 0 (0==false,1==halted,2==paused,3==stopped)
         * [2] halted bit 1
         * [3] unexpected shutdown bit 0 same as register 0x54 bit 1/2/3
         * [4] unexpected shutdown bit 1
         * [5] unexpected shutdown bit 2
         * [6] alarm available
         * [7] config loaded
         * [8]
         * [9]
         * [A]
         * [B]
         * [C]
         * [D]
         * [E]
         * [F]
         */
      else if (realAddress==IR_CL_STATUS)
        *value=cl_ds.settings[0];
      else if (realAddress==IR_ALARM_CNT)
        *value=ALARM_BIT_CNT;																															//read number of alarm bits, can be converted to nr of bytes
      else if ((realAddress>=IR_ALARM_START)&&(realAddress<=(IR_ALARM_END)))
      {
        realAddress=realAddress-IR_ALARM_START;
        *value=cl_ds.alarms[realAddress];
      }
      else if (realAddress==IR_UPTIME_MILLIS_LOW)
        *value=millis()&0xFFFF;
      else if (realAddress==IR_UPTIME_MILLIS_HIGH)
        *value=(millis()>>16);
      else
        result=clReadReg(address, value);
    }
  }
  return result;																																					//return error code, if any
}
//--------------------------------------------------------------------------------------
ReadFuncPtr ReadBit(uint16_t address, uint16_t* value)
{
  uint16_t realAddress=address;
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  if (address>=REGION_OUTPUT_COIL_START)
  {
    if (address>=REGION_DISCRETE_INPUT_START)
    {
      if (address<=REGION_END(REGION_DISCRETE_INPUT_START))
        result=clReadBit(address, value);
    }
    else if (address<=REGION_END(REGION_OUTPUT_COIL_START))
      result=clReadBit(address, value);
  }
  return result;
}
//--------------------------------------------------------------------------------------
WriteFuncPtr WriteReg(uint16_t address, uint16_t value)
{
  uint16_t realAddress=address;
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  if (address>=REGION_HOLDING_REGISTER_START)
  {
    if (address<=REGION_END(REGION_HOLDING_REGISTER_START))
    {
      realAddress=address-REGION_HOLDING_REGISTER_START;
      if (realAddress<0xC7)
        SET_REGISTER(realAddress, value&0xFF);
      else if (realAddress<=0x4C6)
      {
        realAddress=realAddress-0xC7;
        if (!cl_ds.isRunning)
        {
          EEPROM.write(realAddress, value&0xFF);
          return EXCEPTION_NONE;
        }
      }
      else
        result=clWriteReg(address, value);
    }
  }
  return result;
}
//--------------------------------------------------------------------------------------
WriteFuncPtr WriteBit(uint16_t address, uint16_t value)
{
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  uint16_t realAddress=address;
  if (address>=REGION_OUTPUT_COIL_START)
  {
    if (address<=REGION_END(REGION_OUTPUT_COIL_START))
    {
      realAddress=address-REGION_OUTPUT_COIL_START;
      switch (realAddress)
      {
        case 0x50:
        case 0x51:
        case 0x52:
          if (value==0xFF00)
            return ExecuteFunction(realAddress);
      }
      result=clWriteBit(address, value);
    }
  }
  return result;
}
//--------------------------------------------------------------------------------------
ExecuteFuncPtr ExecuteFunction(uint8_t function)
{
  uint8_t result=EXCEPTION_INVALID_FUNCTION;
  switch (function)
  {
    case 0x50:
      result=clStart();
      if ((result==EXCEPTION_NONE)&&cl_ds.defaultOffline)
      {
        if (ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))
          mb_ds.mbSetup(mb_ds.baudrate, mb_ds.slaveId);
      }
      break;
    case 0x51:
      return clPause();
    case 0x52:
      return clStop();
  }
  return result;
}
//--------------------------------------------------------------------------------------
void setup()
{
  if (CheckEepromForMagic())
  {
    if (ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))
    {
      ConfigFromEeprom(MB_SETTING_SIZE, CL_SETTING_SIZE, cl_ds.settings);
      cl_ds.configLoaded=true;
    }
  }
  mb_ds.mbReadRegister=ReadReg;
  mb_ds.mbReadBit=ReadBit;
  mb_ds.mbWriteRegister=WriteReg;
  mb_ds.mbWriteBit=WriteBit;
  if (cl_ds.configLoaded)
  {
    clSetup();
    if ((!cl_ds.startRunning)&&cl_ds.defaultOffline)
      mb_ds.mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);
    else
      mb_ds.mbSetup(mb_ds.baudrate, mb_ds.slaveId);
    cl_ds.isRunning=cl_ds.startRunning;
  }
  else
  {
    mb_ds.mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);
  }
  cl_ds.unexpectedShutdown=CheckResetRegister();
}
//--------------------------------------------------------------------------------------
void loop()
{
  clLoop();
}
//--------------------------------------------------------------------------------------
