/**
 * Author:  Mateusz Piekarski
 * 
 * Date:    1.10.2021
 * 
 * Description: Program to control ESP32 managing WS2812B LEDs with BT. 
 * This program is also connected with an android app written in Flutter.
 * This program is written for the purposes of Bachelor's Thesis Compuer Science
 */

/*                          Improvements 
 * 
 * dodaj indykator iloscai przez RED LED
 * 
 * Przypisy
 * https://forum.arduino.cc/t/color-changing-rgb-led-rainbow-spectrum/8561
 * https://stackoverflow.com/questions/47785905/converting-rgb-to-hsl-in-c
 * http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
 */  

#include "Comm.h"

void setup() {
  initSerial();
  initVariables();
  initAnimationsRGB();
  resetSensorTop();
  resetSensorBottom();
  resetBuffers();

  while (! Serial) { delay(1); } // wait until serial port opens for native USB devices
  
  Serial.println(F("Starting..."));
  setupTOF();

  /* These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
    Any other board, you can remove this part (but no harm leaving it): */
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif /* END of Trinket-specific code */
  
  initPixels();
}

void loop() 
{  
   receiveBTData(); //Receive Bluetooth command
   performAutomaticLights(); //Zarzadzanie LED i iloscia osob zaczuynajac wchodzenie od gory
   performRainbowAnimation();   
}