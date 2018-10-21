#include "RCTX.h"

void nRFRemote::init(){
Serial.begin(115200);
SPI.begin();
loadConfig();
radio.init(myNode);//init RF and setting Master Node address 
   #ifdef DEBUG 
         Serial.print("Dongle begin with address: ");
         Serial.println(myNode);
         Serial.print("State now: ");
        Serial.println(State);
    #endif
 pinMode(KEY,INPUT_PULLUP);  
	pinMode(BF, INPUT);
	pinMode(BB, INPUT);
	pinMode(BR, INPUT);
	pinMode(BL, INPUT);
  pinMode(F1, INPUT);
	pinMode(F2, INPUT);
	pinMode(F3, INPUT);
	pinMode(F4, INPUT_PULLUP);
  delay(1000);

}
///////////////////////////////////////
void nRFRemote::set_address(uint16_t from,uint16_t to){
	myNode = from; 
  toNode = to;
  radio.init(myNode);
}
///////////////////////////////////////
void nRFRemote::scanKey(){
 //keyState = 0; //clear state buff
bitWrite(keyState,0,!digitalRead(BF)); 
bitWrite(keyState,1,!digitalRead(BB)); 
bitWrite(keyState,2,!digitalRead(BR)); 
bitWrite(keyState,3,!digitalRead(BL)); 
bitWrite(keyState,4,!digitalRead(F1)); 
bitWrite(keyState,5,!digitalRead(F2)); 
bitWrite(keyState,6,!digitalRead(F3)); 
bitWrite(keyState,7,!digitalRead(F4)); 
if (keyState!=lastState)  {   //when any change of keys
 
 varSlide1 = analogRead(SLIDE);
 varSlide1 = map(varSlide1,0,1023,100,0); //map slide to 0-100% 
 #if DEBUG 
 Serial.print("Button State change: "); Serial.println(keyState,BIN);
  Serial.print("Slider value : "); Serial.println(varSlide1);

 #endif
 State = RF_WRITE; 
 first_run = true; 
 lastState = keyState; 
}
checkConfig(); //check if Config Key is pressed to issue new
}

///////////////////////////////////////////////////////////////
void nRFRemote::writeRF(){
  index=0;
  buffer[index++] = 0xFF;
  buffer[index++] = 0x55;
  buffer[index++] = 0x00;   //Len = 6 bytes
  buffer[index++] = 0x00;
  buffer[index++] = 0x02; // SEND COMMAND TYPE
  buffer[index++] = 90;   // MARK AS REMOTE DATA
  buffer[index++] = keyState;   //send button Data
  buffer[index++] = varSlide1;  //Send Slide value

int len = index + 1;
buffer[2] = len-3;
  #ifdef DEBUG 
         Serial.print("..Sending data to address: ");
         Serial.println(toNode);
   #endif
bool OK = radio.RFSend(toNode,buffer,len);
  #ifdef DEBUG 
         Serial.print("Sent!.. ");
   #endif
if (OK) {
   State = SCAN;  //if onnect and send successfully 
   first_run = true;      //set first run for next State

   #ifdef DEBUG 
         Serial.print("Send Successfully to address: ");
         Serial.println(toNode);
         Serial.println("Go to Read RF");
   #endif
}
else {
   if (millis()-timeStart>timeout/2) {
   State = SCAN;    //exit when time out
    first_run = true;      //set first run for next State
     #ifdef DEBUG 
         Serial.print("Sending fail to address: ");
         Serial.println(toNode);
        Serial.println("Go to back to read Serial");
     #endif 
   }
 }
}

///////////////////////////////////
void nRFRemote::run(){
if (first_run)  {
   timeStart = millis();
   #ifdef DEBUG 
         Serial.print("State No: ");
         Serial.println(State);
         Serial.println(" Begin");
   #endif
   first_run = false; 
}  
switch (State) {

  case SCAN :{
  scanKey(); 
  }
  break; 

  case RF_WRITE :{
  writeRF();
  }
  break;
  
  case CONFIG :{
  sendConfig();
  }
  break; 

   } 
}
///////////////////////////
///Private function////////
void nRFRemote::checkConfig() {
bool accessed = false;
if (!digitalRead(KEY)) {
  double start = millis();  
  while (!digitalRead(KEY)) {
    if (millis()-start>500) accessed = true; 
  }
  if (accessed) {//access Sending CONFIG IF PRESS AND HOLD KEY IN 2 SEC
  // configMode = RANDOOM_ADDRESSING;
      #ifdef DEBUG
      Serial.println("Config mode is accessed");
      Serial.println("RANDOM ADDRESSING MODE CONFIG");
      #endif
      randomSeed(millis());
      myNode = random(256,999);      //get randoom of my Address
      toNode = random(1001,2000);  //get randoom of Targeting Address
   //   myNode = (uint16_t)(millis()-start)/2;
   //   toNode = (uint16_t)(millis()-start);
      #ifdef DEBUG
      Serial.println("Got new address");
      Serial.print("My address: ");Serial.print(myNode,HEX);Serial.print("  Target address: ");Serial.println(toNode,HEX);
      #endif
      delay(1000);
      radio.init(myNode); // update my address
     // State = CONFIG;   // go to send config
     // first_run = true; 
      saveConfig();
      sendConfig();
      accessed = false; 
      
      
   }
   else {
      #ifdef DEBUG
      Serial.println("Not access to CONFIG MODE");
      #endif 
   }
 }

}
/////
void nRFRemote::sendConfig(){
 #ifdef DEBUG
      Serial.println("Sending config data:...");
      Serial.print("USB Address: ");Serial.print(myNode); Serial.print("   Robot address: "); Serial.println(toNode);
      #endif
idx = 0;
CFGbuffer[idx++] = 0xFF;
CFGbuffer[idx++] = 0x55;
CFGbuffer[idx++] = 0x00;   //Len = 6 bytes
CFGbuffer[idx++] = 0x00;
CFGbuffer[idx++] = 0x02; //RUN, NOT GET RESPONSE VALUE
CFGbuffer[idx++] = 80;   // CONFIG ADDRESSING TYPE OF COMMAND
addValue(idx,toNode);
addValue(idx,myNode);
//CFGbuffer[idx] = 0xA; // line Feed
int len = idx + 1;
CFGbuffer[2] = len-3;
#ifdef DEBUG
  for (int i=0;i<len;i++) {
    Serial.print(CFGbuffer[i],HEX); Serial.print(" ");
  }
  Serial.println();
  #endif
bool OK=radio.RFSend(DEFAULT_ADDRESS,CFGbuffer,len);
if (OK) { 
  #ifdef DEBUG
  Serial.println("Sent Config addressing successful ");
  #endif
    }
else {
  #ifdef DEBUG
  Serial.println("Sent Config addressing FAIL!");
  #endif
 }  
}
////////////////////////////////////////////////////
void nRFRemote::addValue(int pos,uint16_t val) {
idx = pos;  
valShort.shortVal = val; 
CFGbuffer[idx++] = valShort.byteVal[0];
CFGbuffer[idx++] = valShort.byteVal[1];
}
//////////////////////////////////////////////////
void nRFRemote::saveConfig(){
EEPROM_writeInt(0,myNode);
EEPROM_writeInt(2,toNode);

}
////
void nRFRemote::loadConfig(){
uint16_t myAd,toAd;  
myNode = EEPROM_readInt(0);
toNode = EEPROM_readInt(2);
//set_address(myAd,toAd);
}
//////////////////////////////////////////
void nRFRemote::EEPROM_writeInt(int address,uint16_t value) {
  
      //Decomposition from a int to 2 bytes by using bitshift.
      //One = Most significant -> Two = Least significant byte
      byte two = (value & 0xFF);
      byte one = ((value >> 8) & 0xFF);
      

      //Write the 2 bytes into the eeprom memory.
      EEPROM.write(address, two);
      EEPROM.write(address + 1, one);
     
     
}
/////////////////////////
uint16_t nRFRemote::EEPROM_readInt(int address){
uint16_t two = EEPROM.read(address);
uint16_t one = EEPROM.read(address+1); 
return ((two & 0xFF) + ((one<<8)&0xFFFF));
}