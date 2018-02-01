#include "ControlLoop.h"
#include "Arduino.h"        
cl_t cl_ds;

uint8_t clSetup(){
  return 0;
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
  if(!cl_ds.isRunning){
    if(cl_ds.halted==HALTED_NONE || cl_ds.halted==HALTED_PAUSED){
      cl_ds.halted=HALTED_NONE;
      cl_ds.isRunning=true;
      return EXCEPTION_NONE;
    }else
      return EXCEPTION_NEGATIVE_ACKNOWLEDGE;
  }
  return EXCEPTION_NONE;
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

