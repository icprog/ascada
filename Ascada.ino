//--------------------------------------------------------------------------------------
#include "Tools.h"
#include "ControlLoop.h"
#include "Modbus.h"
#include <EEPROM.h>
//--------------------------------------------------------------------------------------
#define VDATE "Jan 25 01 2018"                            //__DATE__ //"Jan 25 01 2018"
#define VTIME "12:52:02"                                  //__TIME__ //"12:52:02"
//--------------------------------------------------------------------------------------
ReadFuncPtr ReadReg(uint16_t address,uint16_t* value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  if(address>=REGION_INPUT_REGISTER_START){
    if(address>=REGION_HOLDING_REGISTER_START){
      if(address<=REGION_END(REGION_HOLDING_REGISTER_START)){
        result=EXCEPTION_NONE;
        //todo read value
      }
    }else if(address<=REGION_END(REGION_INPUT_REGISTER_START)){
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
              *value=(VDATE[8]<<8)|(VDATE[12]);           //age year nr and decade year nr in ascii
              break;                                      //
            case 4:                                       //
              *value=(VDATE[13]<<8);                      //single year nr, in ascii
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
          *value=cl_ds.status;          
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
        }else if(address==0x09){
          *value=ALARM_BIT_CNT;                           //read number of alarm bits, can be converter to nr of bytes
        }else if((address>=0x10)&&(address<(0x10+ALARM_WORD_CNT))){
          address=address-0x10;
          *value=cl_ds.alarms[address];
        }
        //todo return status information
      }else if(address<=0x01C6){        
        address=address-0xFF;
        *value=GET_REGISTER(address)&0xFF;
      }else if(address<=0x5C6){
        address=address-0x01C7;
        *value=EEPROM.read(address)&0xFF;
        *value=(*value<<8);
        address++;
        if(address<0x400)
          *value=*value|(EEPROM.read(address)&0xFF);
      }
    }
  }  
  return result;
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
      //first 0xFF registers are reserved
      if((address>0xFF)&&(address<0x4FF)){
        address = address-0xFF;
        if(!cl_ds.isRunning){
          EEPROM.write(address,value&0xFF);
          return EXCEPTION_NONE;
        }
      }      
    }//todo else do some other thins
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
  switch(function){
    case 0x50:      
      return clStart();
    case 0x51:
      return clPause();
    case 0x52:
      return clStop();      
  }
  return EXCEPTION_INVALID_FUNCTION;
}
//--------------------------------------------------------------------------------------
void setup(){  
  if(CheckEepromForMagic()){    
    if(ConfigFromEeprom(0,MB_SETTING_SIZE,mb_ds.settings)){
      //ConfigFromEeprom(MB_SETTING_SIZE,CL_SETTING_SIZE,cl_ds.settings);  
      cl_ds.configLoaded=true;
    }
  }
  clSetup();
  mbSetup(115200,247);
  mbReadRegister=ReadReg; 
  mbReadBit=ReadBit; 
  mbWriteRegister=WriteReg;
  mbWriteBit=WriteBit;
  cl_ds.unexpectedShutdown=CheckResetRegister();
  if(cl_ds.configLoaded)  
    cl_ds.isRunning=true;    
}
//--------------------------------------------------------------------------------------
void loop(){
  clLoop();  
}
//--------------------------------------------------------------------------------------
