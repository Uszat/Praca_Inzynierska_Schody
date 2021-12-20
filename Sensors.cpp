#include "Sensors.h"

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
// set the pins to shutdown
#define SHT_LOX1 17
#define SHT_LOX2 16
// minimal sensor distance
#define MIN_SENSOR1_DISTANCE 150//900
#define MIN_SENSOR2_DISTANCE 150//3000

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

int distance      = 0;
int distance2     = 0;
bool sensorTop    = false;
bool sensorBottom = false;


void setSensorTop() {sensorTop = true;}

void resetSensorTop() {sensorTop = false;}

bool getSensorTop() {return sensorTop;}

void setSensorBottom() {sensorBottom = true;}

void resetSensorBottom() {sensorBottom = false;}

bool getSensorBottom() {return sensorBottom;}

//TOF 
void setupTOF() {
  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

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
    // Serial.println(measure1.RangeMilliMeter);
    distance = measure1.RangeMilliMeter;
  } else {
    // Serial.print(F("Out of range"));
  }
  
  if(measure2.RangeStatus != 4) {
    // Serial.println(measure2.RangeMilliMeter);
    distance2 = measure2.RangeMilliMeter;
  } else {
    // Serial.print(F("Out of range "));
    // Serial.print(measure2.RangeMilliMeter);
    distance2 = 8000;
  }
}


void checkSensors()
{
  read_dual_sensors();
  if(distance < MIN_SENSOR1_DISTANCE)
    setSensorTop();
  else
    resetSensorTop();
    
  if(distance2 < MIN_SENSOR2_DISTANCE)
    setSensorBottom();
  else
    resetSensorBottom();
}
