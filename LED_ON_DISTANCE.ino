bool ON_STAIRS;
int peopleOnStairs;
bool sensorTop;
bool sensorBottom;
bool enteredFromTop;
bool enteredFromBottom;

unsigned long currentTime;
unsigned long timeMarker;

uint8_t incomingData = 0;
bool newData = false;
char buf[7];
int bufIndex = 0;
int r,g,b = 0;
char tempBuf[2];
int setter = 0;

int counter = 0;
int newCounter = 0;
int numColors = 255;
bool rainbowSet = false;
bool ledOn = false;
float brightness = 0.5;

enum animationState
{
  MOVING_IN_IN,
  MOVING_IN_OUT,
  FADE,
  OUTSIDE_IN,
  NO_ANIM,
};

enum animationState animation = FADE;

void setAction();

#define TIMEOUT 60000 //in ms so 60sec 
#define MIN_SENSOR1_DISTANCE 150//900
#define MIN_SENSOR2_DISTANCE 150//3000
#define WHITE 180, 0, 180
#define RED   255, 0, 0
#define DEFAULT 0
#define BOTTOM  false
#define TOP     true

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

  if(measure1.RangeStatus != 4) {     // if not out of range
    //Serial.print(measure1.RangeMilliMeter);
    distance = measure1.RangeMilliMeter;
  } else {
    //Serial.print(F("Out of range"));
  }
  
  if(measure2.RangeStatus != 4) {
    //Serial.print(measure2.RangeMilliMeter);
    distance2 = measure2.RangeMilliMeter;
  } else {
    //Serial.print(F("Out of range "));
    //Serial.print(measure2.RangeMilliMeter);
    distance2 = 8000;
  }
}

void setup() {

  //GENERAL IDEA
  ON_STAIRS         = false;
  peopleOnStairs    = 0;
  sensorTop         = false;
  sensorBottom      = false;
  enteredFromTop    = false;
  enteredFromBottom = false;
  turnLedOff();
  r = 255;
  g = 0;
  b = 0;

  resetBuffers();
  
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
        pixels.setPixelColor(i, pixels.Color(r,g,b));
        pixels.show();   // Send the updated pixel colors to the hardware.
        delay(DELAYVAL); // Pause before next pass through loop
      }
  }

void turnLedOnBottom()
  {   
    for(int i=NUMPIXELS-1; i>=0; i--) 
      {
        pixels.setPixelColor(i, pixels.Color(r,g,b));
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

void turnLedOff()
{
  for(int i=NUMPIXELS-1; i>=0; i--) 
    {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      delay(DELAYVAL); // Pause before next pass through loop
    }
    pixels.show();   // Send the updated pixel colors to the hardware.
}

void animationOn(bool top)
{
  switch (animation){
    case MOVING_IN_IN: //default moving in-in
      top ? turnLedOnTop() : turnLedOnBottom();
      break;
    case MOVING_IN_OUT: //moving in-out
      top ? turnLedOnTop() : turnLedOnBottom();
      break;
    case FADE:
      fadeIn();
      break;
    case OUTSIDE_IN:
      // fadeIn();
      break;
    case NO_ANIM:
      // fadeIn();
      break;
  }
}

void animationOff(bool top)
{
  switch (animation){
    case MOVING_IN_IN: //default moving in-in
      top ? turnLedOffTop() : turnLedOffBottom();
      break;
    case MOVING_IN_OUT: //default moving in-out
      top ? turnLedOffBottom() : turnLedOffTop();
      break;
    case FADE:
      fadeOut();
      break;
    case OUTSIDE_IN:
      
      break;
    case NO_ANIM:
      
      break;
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

void receiveBTData(){
 if(SerialBT.available() > 0){
    incomingData = SerialBT.read(); 
    if((char)incomingData == '\n')
    {
      setAction();
    }else{
      buf[bufIndex] = (char)incomingData;
      setter = 1;
      bufIndex++;
    }
  }
}

bool arrayContains(char givenArray[], long x, char givenString[])
{
    for(int j = 0; j<sizeof(givenString)/sizeof(givenString[0])-2; j++){ //-2 cause when string is "br" its actually "br\n\t" or sth, theres empty space after r and then end of string 
      if(givenArray[j] != givenString[j]){
         return false;
      }
    }
    return true;
}

void setAction() 
{
  if(arrayContains(buf, 0, "off"))
  {
    turnLedOff();
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
    setter = 0;
  } 
  else if(arrayContains(buf, 0, "br"))
  {
    setLedBrightness();
  } 
  else if(arrayContains(buf, 0, "rbow"))
  {
    rainbowSet = true;
  }
  else if(arrayContains(buf, 0, "anim0"))
  {
    animation = MOVING_IN_IN;
  }
  else if(arrayContains(buf, 0, "anim1"))
  {
    animation = MOVING_IN_OUT;
  }
  else if(arrayContains(buf, 0, "anim2"))
  {
    animation = FADE;
  }
  else 
  {
    rainbowSet = false;
    setColor(); 
  }

  bufIndex = 0;  
  resetBuffers();
}

//extract hue from rgb, then create new rgb from the hue and new brightness
void makeNewBrightness()
{
  float saturation = 1;
  float _brightness = brightness;
  float hue = RGBtoHUE(r,g,b);
  long color; 
  color = HSBtoRGB(hue, saturation, brightness);
  r = color >> 16 & 255;
  g = color >> 8 & 255;
  b = color & 255;
}

//set the actual new brightness into a brightness variable, create new rgb and turn leds on
void setLedBrightness()
{
  float br;
  for(int i = 2, j = 0; i<4; i++, j++){
    tempBuf[j] = buf[i];
  }
  br = (float)strtol(tempBuf, NULL, 10);

  brightness = br / 100.0;

  Serial.println();
  Serial.println("before ");
  Serial.print("r: ");
  Serial.println(r);
  Serial.print("g: ");
  Serial.println(g);
  Serial.print("b: ");
  Serial.println(b);

  makeNewBrightness();

  Serial.println("after ");
  Serial.print("r: ");
  Serial.println(r);
  Serial.print("g: ");
  Serial.println(g);
  Serial.print("b: ");
  Serial.println(b);
  Serial.print("newBrightness: ");
  Serial.println(brightness);
  if(ledOn){
    turnLedOn();
  }
}

void setColor(){
  for(int i = 0; i < 6; i+=2)
    {
      for(int j = 0; j < 2; j++)
      {
        tempBuf[j] = buf[i+j];
      }
      if(i==0)
      {
         r = (int)strtol(tempBuf, NULL, 16);   
      }
      else if(i==2)
      {
         g = (int)strtol(tempBuf, NULL, 16);;
      }
      else if(i==4)
      {
         b = (int)strtol(tempBuf, NULL, 16);
      }
    }
    turnLedOn();
}

long HSBtoRGB(float _hue, float _sat, float _brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    
    if (_sat == 0.0) {
        red = _brightness;
        green = _brightness;
        blue = _brightness;
    } else {
        if (_hue == 360.0) {
            _hue = 0;
        }

        int slice = _hue / 60.0;
        float hue_frac = (_hue / 60.0) - slice;

        float aa = _brightness * (1.0 - _sat);
        float bb = _brightness * (1.0 - _sat * hue_frac);
        float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
        
        switch(slice) {
            case 0:
                red = _brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = _brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = _brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = _brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = _brightness;
                break;
            case 5:
                red = _brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    long ired = red * 255.0;
    long igreen = green * 255.0;
    long iblue = blue * 255.0;
    
    return long((ired << 16) | (igreen << 8) | (iblue));
}

void rainbow(){

  float colorNumber; 
  float saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
  float brightness = 0.4; // Between 0 and 1 (0 = dark, 1 is full brightness)
  float hue; //later change it to int to check if calculations are quicker
  long color; 
  
  counter = newCounter;

  for(int i=0; i<NUMPIXELS; i++) 
    {
      colorNumber = counter > numColors ? counter - numColors: counter;
      hue = (colorNumber / float(numColors)) * 360; // Number between 0 and 360
      color = HSBtoRGB(hue, saturation, brightness);
      r = color >> 16 & 255;
      g = color >> 8 & 255;
      b = color & 255;
      pixels.setPixelColor(i, pixels.Color(r, g, b));
      counter = (counter + 1) % (numColors * 2);
    }
    pixels.show();
    newCounter = (newCounter+1) % 510; //510 = 2 * 255
}

void fadeIn()
{
  float saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
  float _brightness; // = brightness; // Between 0 and 1 (0 = dark, 1 is full brightness)
  float hue = RGBtoHUE(r,g,b); //extract hue from rgb values
  long color; 
  Serial.print("b4 loop brightness: ");
  Serial.println(brightness);
  for(float j = 0.0; j<brightness; j+=0.01) //go through brightness levels
    {
      _brightness = j;
      Serial.print("_brightness: ");
      Serial.println(_brightness);
    for(int i=0; i<NUMPIXELS; i++) //go through specific pixels
      {
        color = HSBtoRGB(hue, saturation, brightness); //make same color with new brightness
        r = color >> 16 & 255;
        g = color >> 8 & 255;
        b = color & 255;
        pixels.setPixelColor(i, pixels.Color(r, g, b));
      }
        pixels.show();   // Send the updated pixel colors to the hardware
    }
}

void fadeOut()
{
  float _saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
  float _brightness = brightness; // Between 0 and 1 (0 = dark, 1 is full brightness)
  float _hue = RGBtoHUE(r,g,b);
  long color; 
  for(float j = brightness; j>=0; j-=0.01)
    {
      _brightness = j;
    for(int i=0; i<NUMPIXELS; i++) 
      {
        color = HSBtoRGB(_hue, _saturation, _brightness);
        r = color >> 16 & 255;
        g = color >> 8 & 255;
        b = color & 255;
        pixels.setPixelColor(i, pixels.Color(r, g, b));
      }
        pixels.show();   // Send the updated pixel colors to the hardware
    }
}

float RGBtoHUE(int rr, int gg, int bb)
{
  float tR, tG, tB;
  float tMin, tMax;
  float tHue;

//make values into range 0-1
  tR = rr / 255.0;
  tG = gg / 255.0;
  tB = bb / 255.0;

//find min value from r,g,b
  tMin = min(min(tR, tG), tB);

//find max value from r,g,b
  tMax = max(max(tR, tG), tB);
  
 if(tMax - tMin == 0){
    return 0;
  }

//there are three cases with different formulas
  if(tMax == tR){
    tHue = (tG-tB)/(tMax-tMin);
  }else if(tMax == tG){
    tHue = 2.0 + (tB-tR)/(tMax-tMin);
  }else{
    tHue = 4.0 + (tR-tG)/(tMax-tMin);
  }

//make it 0-360
  tHue *= 60;
  
//if less then zero make it positive
  if(tHue < 0){
    tHue+=360;
  }

  return tHue;
}

void manageSensorLED()
{
  if(sensorTop && !ON_STAIRS) //wlacz LED jak nikogo nie ma na schodach i wchodzi zaczal od gory
    {
      if(!rainbowSet){
        animationOn(TOP);
      }
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
      animationOff(TOP);
      // turnLedOffTop();
      ON_STAIRS = false;
      enteredFromTop = false;
      peopleOnStairs = 0;
      Serial.println("===4===");
    }

   //Zarzadzanie LED i iloscia osob zaczuynajac wchodzenie od dolu
   else if(sensorBottom && !ON_STAIRS) //wlacz LED jak nikogo nie ma na schodach i wchodzi zaczal od dolu
    {
      if(!rainbowSet){
        animationOn(BOTTOM);
        // turnLedOnBottom();
      }
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
      // turnLedOffBottom();
      animationOff(BOTTOM);
      ON_STAIRS = false;
      enteredFromBottom = false;
      peopleOnStairs = 0;
      Serial.println("===8===");
    }

    //wylaczanie LED after timeout ( 60 sekund )
  if(ON_STAIRS && timeout())
    {
      turnLedOff();
      resetState();
    }
}

void resetState()
{
  ON_STAIRS         = false;
  peopleOnStairs    = 0;
  sensorTop         = false;
  sensorBottom      = false;
  enteredFromTop    = false;
  enteredFromBottom = false;
}

void resetBuffers(){
  for(int i = 0; i<sizeof(buf)/sizeof(buf[0]); i++){
    buf[i] = '\0';
  }
  for(int i = 0; i<sizeof(tempBuf)/sizeof(tempBuf[0]); i++){
    tempBuf[i] = '\0';
  }
}
void loop() {
  //Receive Bluetooth command
  receiveBTData();

  //Zarzadzanie LED i iloscia osob zaczuynajac wchodzenie od gory
   if(setter == 0)
   {
     checkSensors();
     manageSensorLED();
     debugData(false);
   }
   if(rainbowSet && ledOn){
     if(setter == 0 && ON_STAIRS){
       rainbow(); 
     }else if(setter == 1){
       rainbow(); 
     }
   }  
  delay(1);
}

void debugData(bool isOn){
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
 * 
 * dodaj indykator iloscai przez RED LED
 * 
 * Przypisy
 * https://forum.arduino.cc/t/color-changing-rgb-led-rainbow-spectrum/8561
 * 
 * https://stackoverflow.com/questions/47785905/converting-rgb-to-hsl-in-c
 * http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
 */  
