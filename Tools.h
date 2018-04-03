#ifndef TOOLSBLIB_H
#define TOOLSBLIB_H  

  #include <stdint.h>

  #define EXCEPTION_NONE 0x00                               //no exception present
  #define EXCEPTION_INVALID_FUNCTION 0x01                   //invalid function code in request
  #define EXCEPTION_INVALID_ADDRESS 0x02                    //invalid address in request
  #define EXCEPTION_INVALID_VALUE 0x03                      //invalid value in request, depends upon value usage
  #define EXCEPTION_SLAVE_DEVICE_FAILURE 0x04               //failure when executing request
  #define EXCEPTION_ACKNOWLEDGE 0x05                        //received request, couldn't finish on time
  #define EXCEPTION_SLAVE_DEVICE_BUSY 0x06                  //can't execute request at the moment
  #define EXCEPTION_NEGATIVE_ACKNOWLEDGE 0x07               //can't acknowledge request
  #define EXCEPTION_NOT_RUNNING 0x08                        //can't do stuff, because not in run

  #define MAGIC_NUMBER 0xC369                               //eeprom value used to check if eeprom is set
  #define MAGIC_ADDRESS 0x00                                //eeprom address to read

  #define SET_REGISTER(addr,value) ((*(volatile uint8_t *)addr)=value)
  #define GET_REGISTER(addr) (*(volatile uint8_t *)addr)    //return register value
  
  typedef union{
    uint8_t buf[2];
    uint16_t val;
  } union16_t;

  typedef union{
    uint8_t buf[4];
    uint32_t val;
  } union32_t;

  typedef uint8_t (*ExecuteFuncPtr)(uint8_t);
  typedef uint8_t (*WriteFuncPtr)(uint16_t,uint16_t);
  typedef uint8_t (*ReadFuncPtr)(uint16_t,uint16_t*);
  
  bool CheckCrc(uint8_t* buf, uint16_t len);
  uint16_t GetCrc16(uint8_t* buf, uint16_t len);
  
  bool ReadEeprom(uint16_t start, uint16_t cnt, uint8_t* buf);
  bool WriteEeprom(uint16_t start, uint16_t cnt, uint8_t* buf);
  
  bool ConfigFromEeprom(uint16_t start,uint16_t cnt, uint8_t* buf);
  bool ConfigToEeprom(uint16_t start,uint16_t cnt, uint8_t* buf);
  
  bool CheckEepromForMagic();
  uint8_t CheckResetRegister();
#endif
