//--------------------------------------------------------------------------------------
#include "ControlLoop.h"
#include "Modbus.h"
#include "Project.h"
#include "Arduino.h"
#include <EEPROM.h>
//--------------------------------------------------------------------------------------      
cl_t cl_ds;
//--------------------------------------------------------------------------------------
uint8_t clSetup()
{
  cl_ds.unexpectedShutdown=CheckResetRegister();
	if (cl_ds.configLoaded)
  {
    if ((!cl_ds.startRunning)&&cl_ds.defaultOffline)
      mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);
    else
      mbSetup(mb_ds.baudrate, mb_ds.slaveId);
    if(cl_ds.startRunning)
      clStart();
  }
  else
  {
    mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);
  }

  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t clLoop()
{
	//loop the project
  return 0;
}
//--------------------------------------------------------------------------------------
uint8_t clStart()
{
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
//--------------------------------------------------------------------------------------
uint8_t clStop()
{
  if(cl_ds.isRunning)
	{    
    cl_ds.isRunning=false;
    cl_ds.halted=HALTED_STOPPED;      
    return EXCEPTION_NONE;
  }
	else if(cl_ds.halted==HALTED_PAUSED)
	{
    cl_ds.halted=HALTED_STOPPED;
    return EXCEPTION_NONE;
  }
  return EXCEPTION_NEGATIVE_ACKNOWLEDGE;  
}
//--------------------------------------------------------------------------------------
uint8_t clPause()
{
  if(cl_ds.isRunning)
	{    
    cl_ds.isRunning=false;
    cl_ds.halted=HALTED_PAUSED;      
    return EXCEPTION_NONE;
  }  
  return EXCEPTION_NEGATIVE_ACKNOWLEDGE;  
}
//--------------------------------------------------------------------------------------
uint8_t ReadDeviceReg(uint16_t address,uint16_t* value)
{  
  *value=1;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t WriteDeviceReg(uint16_t address,uint16_t* value)
{  
  *value=2;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadEepromReg(uint16_t address,uint16_t* value)
{  
  *value=3;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t WriteEepromReg(uint16_t address,uint16_t* value)
{  
  *value=4;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadVersionReg(uint16_t address,uint16_t* value)
{  
  *value=5;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadStatusReg(uint16_t address,uint16_t* value)
{  
  *value=6;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadSettingsReg(uint16_t address,uint16_t* value)
{  
  *value=7;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadUptimeReg(uint16_t address,uint16_t* value)
{  
  *value=8;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t HandleModbusCall(uint16_t address, uint16_t* value)
{  
  for(uint16_t i=0;i<MB_CNT;i++)
  {
    if(mbMapping[i].regStart<=address)
    {
      return mbMapping[i].regStart & 0xFF;
      if((mbMapping[i].regStart+mbMapping[i].regCnt)>address)
        return (*(mbMapping[i].funcPtr))(address-mbMapping[i].regStart,value);        
    }
  }
  return EXCEPTION_INVALID_ADDRESS;
}
//--------------------------------------------------------------------------------------
  
