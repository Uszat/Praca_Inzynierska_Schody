#include "Comm.h"

#define DEFAULT 0

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
extern animationState animation;

uint8_t incomingData = 0;
bool newData = false;
char buf[7];
int bufIndex = 0;
char tempBuf[2];
bool rainbowSet = false;
bool ledOn = false;

void initSerial()
{
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
}

bool arrayContains(char givenArray[], long x, char givenString[])
{
  int maxIter = strlen(givenArray);
  for(int j = 0; j<strlen(givenString); j++){
    if(givenArray[j] != givenString[j] || j > maxIter){
       return false;
    }
  }
  return true;
}

void receiveBTData(){
 if(SerialBT.available() > 0){
    incomingData = SerialBT.read(); 
    if((char)incomingData == '\n')
    {
      Serial.println();
      setAction();
      
    }else{
      buf[bufIndex] = (char)incomingData;
      Serial.print(buf[bufIndex]);
      
      resetAutoMode();
      bufIndex++;
    }
  }
}

void setAction() 
{
  if(arrayContains(buf, 0, "off"))
  {
    animationOff(DEFAULT);
    resetState();
    ledOn = false;
  } 
  else if (arrayContains(buf, 0, "on"))
  {
    ledOn = true;
    if(!rainbowSet){
      animationOn(DEFAULT);
    }
  } 
  else if(arrayContains(buf, 0, "auto"))
  {
    animationOff(DEFAULT);
    resetState();
    setAutoMode();
  } 
  else if(arrayContains(buf, 0, "br"))
  {
    setLedBrightness(tempBuf, buf);
  } 
  else if(arrayContains(buf, 0, "rbow"))
  {
    rainbowSet = true;
  }
  else if(arrayContains(buf, 0, "anim0"))
  {
    animation = MOVING_IN_IN; //don't know if its going to work cause its extern here so i probably cant assign value to it
  }
  else if(arrayContains(buf, 0, "anim1"))
  {
    animation = MOVING_IN_OUT;
  }
  else if(arrayContains(buf, 0, "anim2"))
  {
    animation = FADE;
  }
  else if(arrayContains(buf, 0, "isOn"))
  {
    if (SerialBT.available()) 
    {
      ledOn ? SerialBT.println("on") : SerialBT.println("off");
    }
  }
  else 
  {
    rainbowSet = false;
    setAndShowColor(tempBuf, buf);   
  }

  bufIndex = 0;  
  resetBuffers();
}

void resetBuffers(){
  for(int i = 0; i<sizeof(buf)/sizeof(buf[0]); i++){
    buf[i] = '\0';
  }
  for(int i = 0; i<sizeof(tempBuf)/sizeof(tempBuf[0]); i++){
    tempBuf[i] = '\0';
  }
}

// void debugData(bool isOn){
//   if(isOn){
  
//     if(ON_STAIRS)
//     {
//       Serial.println("ON_STAIRS True");
//     }
//   else
//     {
//       Serial.println("ON_STAIRS False");
//     };
  
//   Serial.print("peopleOnStairs ");
//   Serial.println(peopleOnStairs);
  
// if(enteredFromTop)
//     {
//       Serial.println("enteredFromTop True");
//     }
//   else
//     {
//       Serial.println("enteredFromTop False");
//     };
  
//   if(enteredFromBottom)
//     {
//       Serial.println("enteredFromBottom True");
//     }
//   else
//     {
//       Serial.println("enteredFromBottom False");
//     };

//   if(getSensorTop())
//     {
//       Serial.println("sensorTop True");
//     }
//   else
//     {
//       Serial.println("sensorTop False");
//     };
//   if(getSensorBottom())
//     {
//       Serial.println("sensorBottom True");
//     }
//   else
//     {
//       Serial.println("sensorBottom False");
//     };
//   Serial.println();
//   }
// }
