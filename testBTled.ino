uint8_t incomingData = 0;
int command = 0;
bool newData = false;
char buf[6];
char tempBuf[2];
int bufIndex = 0;
uint8_t r,g,b = 0;

//BT ==========================
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN       15 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 300 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 0 // Time (in milliseconds) to pause between pixels

void turnLedOn()
  {   
    for(int i=0; i<NUMPIXELS; i++) 
      {
        pixels.setPixelColor(i, pixels.Color(r, g, b));
        delay(DELAYVAL); // Pause before next pass through loop
      }
      pixels.show();   // Send the updated pixel colors to the hardware.
  }


void receiveOneChar(){
  if (SerialBT.available() > 0) {
    incomingData = SerialBT.read();
    newData = true;
  }
}

void setRGB() {
  if (newData == true) {
    buf[bufIndex] = (char)incomingData;
    newData = false;
    bufIndex++;
    Serial.print("bufindex ");
    Serial.println(bufIndex);
    if(bufIndex >= 6)
    {
      for(int i = 0; i < bufIndex; i+=2)
      {
        for(int j = 0; j < 2; j++)
          tempBuf[j] = buf[i+j];
        
        if(i==0)
           r = (int)strtol(tempBuf, NULL, 16);
           Serial.print("r ");
           Serial.println(r);     
        if(i==2)
           g = (int)strtol(tempBuf, NULL, 16);
           Serial.print("r ");
           Serial.println(g);
        if(i==4)
           b = (int)strtol(tempBuf, NULL, 16);
           Serial.print("r ");
           Serial.println(b);
      }
      for(int i = 0; i < strlen(buf); i++)
      {
        Serial.print(buf[i]);
      }
      Serial.println();
    
      command = 1;
      bufIndex = 0;  
    }
  }
}

void doCommand(){

  if(command == 1)
    {
      turnLedOn();
      command = 0;
    }
}

void setup()
{
//BT
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  delay(100);
  pixels.clear();
}

void loop()
{
  receiveOneChar();
  setRGB();
  doCommand();

  delay(1);  
}
