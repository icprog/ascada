//--------------------------------------------------------------------------------------
#include "Tools.h"
#include "ControlLoop.h"
#include "Modbus.h"
#include <EEPROM.h>
//--------------------------------------------------------------------------------------
#define VDATE "Mar 04 2018"                               //__DATE__ //"Mar 04 2018"
#define VTIME "12:23:02"                                  //__TIME__ //"12:23:02"
//--------------------------------------------------------------------------------------
ReadFuncPtr ReadReg(uint16_t address,uint16_t* value){    //read a register, return value through pointer
  uint8_t result=EXCEPTION_INVALID_ADDRESS;               //default failing  
  *value=0;
  if(address>=REGION_HOLDING_REGISTER_START)
  {                                                       //but reading holding registers      
    if(address<=REGION_END(REGION_HOLDING_REGISTER_START))
    {
      result=EXCEPTION_NONE;                              //success
      address=address-REGION_HOLDING_REGISTER_START;      //convert address to something between 0 and 9998
      if(address<0xC7)                                    //read value from actual device register          
        *value=GET_REGISTER(address)&0xFF;                //grab value from device, binary and with 0xFF
      else if(address<=0x4C6)
      {                                                   //read value from eeprom address
        address=address-0xC7;                             //convert to actual address
        *value=EEPROM.read(address)&0xFF;                 //read from eeprom and return value
      }   
      //todo read value
    }
  }
  else if((address>=REGION_INPUT_REGISTER_START) && (address<=REGION_END(REGION_INPUT_REGISTER_START)))
  {
    result=EXCEPTION_NONE;                              //
    address=address-REGION_INPUT_REGISTER_START;        //convert address to something between 0 and 9998
    if(address<=0xFF){                                  //first 255 registers are reserved for device and status information
      if(address<=0x07){                                //request compile time information, used to distinquish version
        switch(address){                                //
          case 0:                                       //
            *value=(VDATE[0]<<8)|(VDATE[1]);            //month, first 2 characters (Jan/Feb/Mar/etc) in ascii
            break;                                      //
          case 1:                                       //
            *value=(VDATE[2]<<8)|(VDATE[4]);            //month, last character and largest day nr in ascii
            break;                                      //
          case 2:                                       //
            *value=(VDATE[5]<<8)|(VDATE[7]);            //smallest day nr and century year nr in ascii
            break;                                      //
          case 3:                                       //
            *value=(VDATE[8]<<8)|(VDATE[9]);            //age year nr and decade year nr in ascii
            break;                                      //
          case 4:                                       //
            *value=(VDATE[10]<<8)|(' ');                //single year nr, in ascii and space
            break;                                      //
          case 5:                                       //
            *value=(VTIME[0]<<8)|(VTIME[1]);            //time hour, in ascii
            break;                                      //
          case 6:                                       //
            *value=(VTIME[3]<<8)|(VTIME[4]);            //time minute, in ascii
            break;                                      //
          case 7:                                       //
            *value=(VTIME[6]<<8)|(VTIME[7]);            //time second, in ascii
            break;                                      //
        }                                               //
      }else if(address==0x08){                          //else request runtime information
        *value=cl_ds.status;                            //status information, described below     
        //runtime information
        /* bit description
         * [0] running 
         * [1] halted bit 0 (0==false,1==halted,2==paused,3==stopped)
         * [2] halted bit 1 
         * [3] unexpected shutdown bit 0 same as register 0x54 bit 1/2/3
         * [4] unexpected shutdown bit 1
         * [5] unexpected shutdown bit 2
         * [6] config loaded 
         * [7] alarm available
         * [8]
         * [9]
         * [A]
         * [B]
         * [C]
         * [D]
         * [E]
         * [F]
         */
      }else if(address==0x09)
        *value=ALARM_BIT_CNT;                           //read number of alarm bits, can be converted to nr of bytes
      else if((address>=0x0A)&&(address<(0x0A+ALARM_WORD_CNT)))
      { 
        address=address-0x0A;
        *value=cl_ds.alarms[address];
      }
      else if(address==(0x0A+ALARM_WORD_CNT))
        *value=millis()&0xFFFF;          
      else if(address==(0x0B+ALARM_WORD_CNT))
        *value=(millis()>>16);      
    }
  }
  return result;                                          //return error code, if any
}
//--------------------------------------------------------------------------------------
ReadFuncPtr ReadBit(uint16_t address,uint16_t* value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  if(address>=REGION_OUTPUT_COIL_START){
    if(address>=REGION_DISCRETE_INPUT_START){
      if(address<=REGION_END(REGION_DISCRETE_INPUT_START)){
        result=EXCEPTION_NONE;
        //todo read value
      }
    }else if(address<=REGION_END(REGION_OUTPUT_COIL_START)){
      result=EXCEPTION_NONE;
      //todo read value
    }
  }  
  return result;  
}
//--------------------------------------------------------------------------------------
WriteFuncPtr WriteReg(uint16_t address,uint16_t value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;    
  if(address>=REGION_HOLDING_REGISTER_START){
    if(address<=REGION_END(REGION_HOLDING_REGISTER_START)){
      address=address-REGION_HOLDING_REGISTER_START;           
      if(address<0xC7)
        SET_REGISTER(address,value&0xFF);
      else if(address<=0x4C6){
        address = address-0xC7;
        if(!cl_ds.isRunning){
          EEPROM.write(address,value&0xFF);
          return EXCEPTION_NONE;
        }
      }      
    }
  }  
  return result;
}
//--------------------------------------------------------------------------------------
WriteFuncPtr WriteBit(uint16_t address,uint16_t value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  if(address>=REGION_OUTPUT_COIL_START){    
    if(address<=REGION_END(REGION_OUTPUT_COIL_START)){
      address=address-REGION_OUTPUT_COIL_START;
      switch(address){
        case 0x50:
        case 0x51:
        case 0x52:
          if(value==0xFF00)
            return ExecuteFunction(address);
      }     
      //todo write bit
    }
  }  
  return result;  
}
//--------------------------------------------------------------------------------------
ExecuteFuncPtr ExecuteFunction(uint8_t function){  
  uint8_t result=EXCEPTION_INVALID_FUNCTION;
  switch(function){
    case 0x50:      
      result=clStart();
      if((result==EXCEPTION_NONE) && cl_ds.defaultOffline)
        mbSetup(mb_ds.baudrate,mb_ds.slaveId);
    case 0x51:
      return clPause();
    case 0x52:
      return clStop();      
  }
  return result;
}
//--------------------------------------------------------------------------------------
void setup(){  
  if(CheckEepromForMagic())
  {
    if(ConfigFromEeprom(0,MB_SETTING_SIZE,mb_ds.settings))
    {
      ConfigFromEeprom(MB_SETTING_SIZE,CL_SETTING_SIZE,cl_ds.settings);  
      cl_ds.configLoaded=true;      
    }
  }
  mbReadRegister=ReadReg; 
  mbReadBit=ReadBit; 
  mbWriteRegister=WriteReg;
  mbWriteBit=WriteBit;
  if(cl_ds.configLoaded)
  {
    clSetup();
    if(!cl_ds.startRunning && cl_ds.defaultOffline)
      mbSetup(DEFAULT_BAUDRATE,DEFAULT_SLAVE_ID);
    else
      mbSetup(mb_ds.baudrate,mb_ds.slaveId);
    cl_ds.isRunning=cl_ds.startRunning;    
  }else{
    mbSetup(DEFAULT_BAUDRATE,DEFAULT_SLAVE_ID);
    
  }
  cl_ds.unexpectedShutdown=CheckResetRegister();  
}
//--------------------------------------------------------------------------------------
void loop(){
  clLoop();  
}
//--------------------------------------------------------------------------------------
