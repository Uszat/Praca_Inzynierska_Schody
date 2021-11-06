//GENERAL IDEA
bool ON_STAIRS;
int peopleOnStairs;
bool sensorTop;
bool sensorBottom;
bool enteredFromTop;
bool enteredFromBottom;
unsigned long currentTime;
unsigned long timeMarker;
uint8_t incomingData = 0;
int command = 0;
bool newData = false;
char buf[6];
int bufIndex = 0;
int r,g,b = 0;

#define TIMEOUT 60000 //in ms so 60sec 
#define MIN_SENSOR1_DISTANCE 900
#define MIN_SENSOR2_DISTANCE 3000
#define WHITE 180, 0, 180
#define RED   255, 0, 0

//BT ==========================
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//TOF =============================
#include "Adafruit_VL53L0X.h"

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 17
#define SHT_LOX2 16

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
int distance = 0;
int distance2 = 0;
int distance_in_pixels = 0;
int newPixels = 0;
int prevPixels = 0;
#define MINDIST 50.0
#define MAXDIST 500.0

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

//LED ==============================
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN       15 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 300 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 0 // Time (in milliseconds) to pause between pixels

//TOF 
void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
}

void read_dual_sensors() {
  
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

   //print sensor one reading
  //Serial.print(F("1: "));
  if(measure1.RangeStatus != 4) {     // if not out of range
    //Serial.print(measure1.RangeMilliMeter);
    distance = measure1.RangeMilliMeter;
  } else {
    //Serial.print(F("Out of range"));
  }
  
  //Serial.print(F(" "));

   //print sensor two reading
  //Serial.print(F("2: "));
  if(measure2.RangeStatus != 4) {
    //Serial.print(measure2.RangeMilliMeter);
    distance2 = measure2.RangeMilliMeter;
  } else {
    //Serial.print(F("Out of range "));
    //Serial.print(measure2.RangeMilliMeter);
    distance2 = 8000;
  }
  
  //Serial.println();
}

void setup() {

  //GENERAL IDEA
  ON_STAIRS = false;
  peopleOnStairs = 0;
  sensorTop = false;
  sensorBottom = false;
  enteredFromTop = false;
  enteredFromBottom = false;
  turnLedOffTop();
  command = 0;
  
//TOF
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name

  // wait until serial port opens for native USB devices
  while (! Serial) { delay(1); }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  Serial.println(F("Shutdown pins inited..."));

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);

  Serial.println(F("Both in reset mode...(pins are low)"));
  
  
  Serial.println(F("Starting..."));
  setID();
 

 //LED 
 
 // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  delay(100);
  pixels.clear();
}

void turnLedOn()
  {   
    for(int i=0; i<NUMPIXELS; i++) 
      {
        pixels.setPixelColor(i, pixels.Color(r, g, b));
        delay(DELAYVAL); // Pause before next pass through loop
      }
      pixels.show();   // Send the updated pixel colors to the hardware.
  }


void turnLedOnTop()
  {   
    for(int i=0; i<NUMPIXELS; i++) 
      {
        pixels.setPixelColor(i, pixels.Color(WHITE));
        pixels.show();   // Send the updated pixel colors to the hardware.
        delay(DELAYVAL); // Pause before next pass through loop
      }
  }

void turnLedOnBottom()
  {   
    for(int i=NUMPIXELS-1; i>=0; i--) 
      {
        pixels.setPixelColor(i, pixels.Color(WHITE));
        pixels.show();   // Send the updated pixel colors to the hardware.
        delay(DELAYVAL); // Pause before next pass through loop
      }
  }

void turnLedOffTop()
{
  for(int i=0; i<NUMPIXELS; i++) 
    {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
    }
}

void turnLedOffBottom()
{
  for(int i=NUMPIXELS-1; i>=0; i--) 
    {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
    }
}

void checkSensors()
{
  read_dual_sensors();
  if(distance < MIN_SENSOR1_DISTANCE)
    sensorTop = true;
  else
    sensorTop = false;
    
  if(distance2 < MIN_SENSOR2_DISTANCE)
    sensorBottom = true;
  else
    sensorBottom = false;
}

bool timeout()
{
  currentTime = millis();
  if((currentTime - timeMarker) >= TIMEOUT)
    return true;
  else
    return false;
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

    if(bufIndex == 2)
    {
      r = (int)strtol(buf, NULL, 16);
    }else if(bufIndex == 4)
    {
      g = (int)strtol(buf, NULL, 16);
    }else if(bufIndex == 6)
    {
      b = (int)strtol(buf, NULL, 16);
    }
    
    if(bufIndex >= 6)
    {
      for(int i = 0; i < bufIndex; i++)
      {
        Serial.print(buf[i]);
      }
      command = 1;
      bufIndex = 0;  
      Serial.println();
    }
  }
}

void loop() {

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }


  receiveOneChar();
  setRGB();


  if(command == 1)
    {
      turnLedOn();
      command = 0;
    }
  
  //Zarzadzanie LED i iloscia osob zaczuynajac wchodzenie od gory
  checkSensors();
  if(sensorTop && !ON_STAIRS) //wlacz LED jak nikogo nie ma na schodach i wchodzi zaczal od gory
    {
      turnLedOnTop();
      ON_STAIRS = true;
      enteredFromTop = true;
      peopleOnStairs++;
      timeMarker = millis();
      Serial.println("===1===");
    }
  else if(sensorTop  && ON_STAIRS && enteredFromTop) //dodaj liczbe osob jak wchodzi od gory i zaczal od gory
    {
      Serial.println("===2===");
      peopleOnStairs++;  
      delay(500);  
    }
  else if(sensorBottom && ON_STAIRS && enteredFromTop && peopleOnStairs > 1) //odejmij liczbe osob jak wyszedl z dolu i zaczal od gory i na schodach byla wiecej niz 1 osoba
    {
      peopleOnStairs--;
      Serial.println("===3===");
      delay(500);
    }
  else if(sensorBottom && ON_STAIRS && enteredFromTop && peopleOnStairs == 1) //jesli wyszedl z dolu, byl ktos na schodach, zaczal od gory i tylko 1 os byla na schodach to LED off
    {
      turnLedOffTop();
      ON_STAIRS = false;
      enteredFromTop = false;
      peopleOnStairs = 0;
      Serial.println("===4===");
    }

   //Zarzadzanie LED i iloscia osob zaczuynajac wchodzenie od dolu
   else if(sensorBottom && !ON_STAIRS) //wlacz LED jak nikogo nie ma na schodach i wchodzi zaczal od dolu
    {
      turnLedOnBottom();
      ON_STAIRS = true;
      enteredFromBottom = true;
      peopleOnStairs++;
      timeMarker = millis();
      Serial.println("===5===");
    }
  else if(sensorBottom  && ON_STAIRS && enteredFromBottom) //dodaj liczbe osob jak wchodzi od dolu i zaczal od dolu
    {
      peopleOnStairs++;   
      delay(500);
      Serial.println("===6==="); 
    }
  else if(sensorTop && ON_STAIRS && enteredFromBottom && peopleOnStairs > 1) //odejmij liczbe osob jak wyszedl z gory i zaczal od dolu i na schodach byla wiecej niz 1 osoba
    {
      peopleOnStairs--;
      Serial.println("===7===");
      delay(500);
    }
  else if(sensorTop && ON_STAIRS && enteredFromBottom && peopleOnStairs == 1) //jesli wyszedl z gory, byl ktos na schodach, zaczal od dolu i tylko 1 os byla na schodach to LED off
    {
      turnLedOffBottom();
      ON_STAIRS = false;
      enteredFromBottom = false;
      peopleOnStairs = 0;
      Serial.println("===8===");
    }
  
  

  //wylaczanie LED after timeout ( 60 sekund )
  if(ON_STAIRS && timeout())
    {
      turnLedOffTop();
      ON_STAIRS = false;
      peopleOnStairs = 0;
      enteredFromBottom = false;
      enteredFromTop = false;
    }



  debugData(false);


  delay(10);
  
}



void debugData(isOn)
{

  if(isOn){
  
    if(ON_STAIRS)
    {
      Serial.println("ON_STAIRS True");
    }
  else
    {
      Serial.println("ON_STAIRS False");
    };
  
  Serial.print("peopleOnStairs ");
  Serial.println(peopleOnStairs);
  
if(enteredFromTop)
    {
      Serial.println("enteredFromTop True");
    }
  else
    {
      Serial.println("enteredFromTop False");
    };
  
  if(enteredFromBottom)
    {
      Serial.println("enteredFromBottom True");
    }
  else
    {
      Serial.println("enteredFromBottom False");
    };

  if(sensorTop)
    {
      Serial.println("sensorTop True");
    }
  else
    {
      Serial.println("sensorTop False");
    };
  if(sensorBottom)
    {
      Serial.println("sensorBottom True");
    }
  else
    {
      Serial.println("sensorBottom False");
    };
  Serial.println();
  }
}

/* Improvements */

/*
 * 
 * Add changing colors, wybieranie kolor itp
 * Jak wchodzi pierwsza osoba to i tak policz nastepne osoby
 * Przyspiesz zapalanie swiatel
 * dodaj indykator iloscai przez RED LED
 * 
 */


  
  /* //Czujnik oddalania i przyblizania i accordingly swiatla reaguja
  if (newPixels < prevPixels){ //zmniejszylo sie
    for (int i = prevPixels; i>=distance_in_pixels; i--) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
      } 
  } else if (newPixels > prevPixels){
    for (int i = prevPixels; i<distance_in_pixels; i++) {
      pixels.setPixelColor(i, pixels.Color(125, 0, 125));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
      }
    }

    prevPixels = distance_in_pixels;
  */



  
  
//  float dzielenie = (distance - MINDIST - 20) / (MAXDIST - MINDIST);
//  distance_in_pixels = dzielenie * (NUMPIXELS);
//  if (distance_in_pixels > NUMPIXELS){
//    distance_in_pixels = NUMPIXELS;
//  } else if (distance_in_pixels <0){
//    distance_in_pixels = 0;
//  }
//  
//  newPixels = distance_in_pixels;
 
  
