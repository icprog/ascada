//--------------------------------------------------------------------------------------
#include "Tools.h"
#include "Modbus.h"
#include <EEPROM.h>
//--------------------------------------------------------------------------------------
ReadFuncPtr ReadReg(uint16_t address,uint16_t* value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  if(address>=REGION_INPUT_REGISTER_START){
    if(address>=REGION_HOLDING_REGISTER_START){
      if(address<=REGION_END(REGION_HOLDING_REGISTER_START)){
        result=EXCEPTION_NONE;
        //todo read holding register
      }
    }else if(address<=REGION_END(REGION_INPUT_REGISTER_START)){
      result=EXCEPTION_NONE;                              //
      address=address-REGION_INPUT_REGISTER_START;        //convert address to something between 0 and 9998
      //todo read input register 
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
      //todo do stuff with registers writing
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
      //todo write bits
    }
  }  
  return result;  
}
void setup(){  
  mbSetup(115200,247);
  mbReadRegister=ReadReg; 
  mbReadBit=ReadBit; 
  mbWriteRegister=WriteReg;
  mbWriteBit=WriteBit;
}
//--------------------------------------------------------------------------------------
void loop(){
}
//--------------------------------------------------------------------------------------
