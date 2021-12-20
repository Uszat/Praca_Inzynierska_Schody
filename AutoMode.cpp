#include "AutoMode.h"

#define TIMEOUT 60000 //in ms so 60sec 
#define BOTTOM  false
#define TOP     true

unsigned long currentTime;
unsigned long timeMarker;

bool ON_STAIRS;
int peopleOnStairs;
bool enteredFromTop;
bool enteredFromBottom;

bool autoMode = true;

extern bool rainbowSet;
extern bool ledOn;

class dupa 
{public: };
void initVariables()
{
  peopleOnStairs    = 0;
  ON_STAIRS         = false;
  enteredFromTop    = false;
  enteredFromBottom = false;
  turnLedOff();
  initGlobalRGB();
}

void setAutoMode() {autoMode = true;}

void resetAutoMode() {autoMode = false;}

bool getAutoMode() {return autoMode;}

void performAutomaticLights()
{
  if(autoMode)
    {
      checkSensors();
      manageSensorLED();
      // debugData(false);
    }
}

void performRainbowAnimation()
{
  if(rainbowSet && ledOn){
    if(autoMode && ON_STAIRS){
      rainbow(); 
    }else if(!autoMode){
      rainbow(); 
    }
   }
}

bool timeout()
{
  currentTime = millis();
  if((currentTime - timeMarker) >= TIMEOUT)
    return true;
  else
    return false;
}

void resetState()
{
  ON_STAIRS         = false;
  peopleOnStairs    = 0;
  enteredFromTop    = false;
  enteredFromBottom = false;
  resetSensorTop();
  resetSensorBottom();
}

void manageSensorLED()
{
  //change variable sensorTop na funkcje sensorTop()
  if(getSensorTop() && !ON_STAIRS) //turn LED on is nobody is on stairs and someone just stepped in from the top
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
  else if(getSensorTop()  && ON_STAIRS && enteredFromTop) //increment a number of people on stairs when entering from the top
    {
      peopleOnStairs++;  
      Serial.println("===2===");
      delay(500);  
    }
  else if(getSensorBottom() && ON_STAIRS && enteredFromTop && peopleOnStairs > 1) //odejmij liczbe osob jak wyszedl z dolu i zaczal od gory i na schodach byla wiecej niz 1 osoba
    {
      peopleOnStairs--;
      Serial.println("===3===");
      delay(500);
    }
  else if(getSensorBottom() && ON_STAIRS && enteredFromTop && peopleOnStairs == 1) //jesli wyszedl z dolu, byl ktos na schodach, zaczal od gory i tylko 1 os byla na schodach to LED off
    {
      animationOff(TOP);
      // turnLedOffTop();
      ON_STAIRS = false;
      enteredFromTop = false;
      peopleOnStairs = 0;
      Serial.println("===4===");
    }

   //Zarzadzanie LED i iloscia osob zaczuynajac wchodzenie od dolu
   else if(getSensorBottom() && !ON_STAIRS) //wlacz LED jak nikogo nie ma na schodach i wchodzi zaczal od dolu
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
  else if(getSensorBottom()  && ON_STAIRS && enteredFromBottom) //dodaj liczbe osob jak wchodzi od dolu i zaczal od dolu
    {
      peopleOnStairs++;   
      delay(500);
      Serial.println("===6==="); 
    }
  else if(getSensorTop() && ON_STAIRS && enteredFromBottom && peopleOnStairs > 1) //odejmij liczbe osob jak wyszedl z gory i zaczal od dolu i na schodach byla wiecej niz 1 osoba
    {
      peopleOnStairs--;
      Serial.println("===7===");
      delay(500);
    }
  else if(getSensorTop() && ON_STAIRS && enteredFromBottom && peopleOnStairs == 1) //jesli wyszedl z gory, byl ktos na schodach, zaczal od dolu i tylko 1 os byla na schodach to LED off
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
