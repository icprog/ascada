//--------------------------------------------------------------------------------------
//      Author: bolke
//      Date: 05-04-2018
//      Description: 
//--------------------------------------------------------------------------------------
#include "Tools.h"
#include "ControlLoop.h"
#include "Modbus.h"
//--------------------------------------------------------------------------------------
void serialEvent() {
	mbSerialEvent();
}
//--------------------------------------------------------------------------------------
void setup()
{
  if (CheckEepromForMagic())
  {
    if (ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))
    {
      ConfigFromEeprom(MB_SETTING_SIZE, sizeof(mb_ds.settings), cl_ds.settings);
      cl_ds.configLoaded=true;
    }
  }

  mb_ds.ReadRegister=HandleModbusCall;
  mb_ds.ReadBit=HandleModbusCall;
  mb_ds.WriteRegister=HandleModbusCall;
  mb_ds.WriteBit=HandleModbusCall;
  
  clSetup();
}
//--------------------------------------------------------------------------------------
void loop()
{
  clLoop();
}
//--------------------------------------------------------------------------------------

