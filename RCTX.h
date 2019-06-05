//library for USD RF24 Dongle fimware for Scratch mBlock interface
//Hardware: AT328P  Arduino Nano Bootloader
//Serial to Nordic RF24L01 Bridge for mBlock software
//Author: Hien PHan
//Project: Negendo Dongle for Scratch 
//Negendo Toys company 

#ifndef RCTX_H
#define RCTX_H
#define DEBUG 1
//#define DEBUG_SERIAL 1////////////////////////////////////
#include "EasyRF.h"
#include <SPI.h>
#include "Scratch.h"
#include "EEPROM.h"
/////Pins define///////////////////////////////////////////
#define CE_PIN	A2
#define CSN_PIN A3

#define KEY 3

#define BF    2
#define BB    4
#define BL    5
#define BR    6
#define F1    9
#define F2    7
#define F3    8
#define F4    A1
#define SLIDE A0
/////State define//////////////////////////////////////////
#define SCAN    0     //check data from PC     ///
#define RF_WRITE         1     //Send data via RF       
#define RF_READ          2     //Reading data from RF
#define CONFIG           3
#define SLEEP            4
///////////////////////////////////////////////////////////
//define  command frame //////////////////////////////
/////////////////////////////////////////////////////////////
#define GET 1
#define RUN 2
#define RESET 4
#define START 5
/////////
#define DEFAULT_ADDRESS 1000
/////
#define MAX_READ_SIZE 32
#define RUN_TIMEOUT    5000L  
#define GET_TIMEOUT    5000L
#define TIMEOUT     5000L
////
class nRFRemote {
public:
nRFRemote() {} //constructor//////////////////////////////// 
int State = SCAN; 
RF24 myRadio = RF24(CE_PIN, CSN_PIN);
EasyRF radio = EasyRF(myRadio);   
void init(); 
void set_address(uint16_t from,uint16_t to);
void checkConfig();
void sendConfig();
void scanKey();
void writeRF();
void readAck();
void run();
uint8_t  keyState = 0; 
uint8_t lastState = 0;
int  varSlide1 = 0;
int  varSlide2 = 0;
uint8_t  last_varSlide1 = 0;
uint8_t  joysticX = 0;
uint8_t  joystickY = 0;
bool     ready = true;
bool     connected = false; 
private: 
uint16_t myNode = 1; 
uint16_t toNode = 2;    
uint16_t Default_Addr = DEFAULT_ADDRESS;
////variable for Serial function
byte index = 0;
byte dataLen = 0;
unsigned char buffer[32]; // buffer for serial read data 
unsigned char Rbuffer[32]; //
double timeStart; 
unsigned long timeout = RUN_TIMEOUT; 
bool first_run = true; 
bool isGetNewAddress = false; 
byte idx = 0;
uint8_t CFGbuffer[32]; 
////////Define  RF Scratch command array processing////////////
   union
  {
    byte byteVal[2];
    short shortVal;
  }valShort;
  //////////////////////////////////////////////////////////////
  void addValue(int pos,uint16_t value);
  void saveConfig();
  void loadConfig();
  void EEPROM_writeInt(int address,uint16_t value);
  uint16_t EEPROM_readInt(int address);

};/////
#endif
