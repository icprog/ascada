#include "ControlLoop.h"
#include "Arduino.h"
#include "PCF8563.h"
      
cl_t cl_ds;
PCF8563 rtc;

uint8_t clSetup(){
  rtc.Setup();
  return EXCEPTION_NONE;
}

uint8_t clLoop(){
  if(cl_ds.isRunning){
    digitalWrite(13, HIGH);
    //todo the control loop
  }else{
    digitalWrite(13, LOW);    
  }
  return 0;
}

uint8_t clStart(){
  uint8_t result=EXCEPTION_NEGATIVE_ACKNOWLEDGE;
  if(cl_ds.configLoaded && !cl_ds.isRunning)
  {
    if(cl_ds.halted==HALTED_NONE || cl_ds.halted==HALTED_PAUSED)
    {
      cl_ds.halted=HALTED_NONE;
      cl_ds.isRunning=true;
      result=EXCEPTION_NONE;
    }
  }
  return result;
}

uint8_t clStop(){
  if(cl_ds.isRunning){    
    cl_ds.isRunning=false;
    cl_ds.halted=HALTED_STOPPED;      
    return EXCEPTION_NONE;
  }else if(cl_ds.halted==HALTED_PAUSED){
    cl_ds.halted=HALTED_STOPPED;
    return EXCEPTION_NONE;
  }
  return EXCEPTION_NEGATIVE_ACKNOWLEDGE;  
}

uint8_t clPause(){
  if(cl_ds.isRunning){    
    cl_ds.isRunning=false;
    cl_ds.halted=HALTED_PAUSED;      
    return EXCEPTION_NONE;
  }  
  return EXCEPTION_NEGATIVE_ACKNOWLEDGE;  
}

ReadFuncPtr clReadReg(uint16_t address,uint16_t* value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  return result;
}

ReadFuncPtr clReadBit(uint16_t address,uint16_t* value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  return result;
}

WriteFuncPtr clWriteReg(uint16_t address,uint16_t value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  return result;
}

WriteFuncPtr clWriteBit(uint16_t address,uint16_t value){
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  return result;
}

