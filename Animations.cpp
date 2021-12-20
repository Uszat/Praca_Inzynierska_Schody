#include "Animations.h"

#define DELAYVAL  0
#define PIN       15
#define NUMPIXELS 300

animationState animation = FADE;

int counter = 0;
int newCounter = 0;
int numColors = 255;
int r,g,b = 0;
float localBrightness;

void initAnimationsRGB()
{
  setLocalRGBtoGlobalRGB(&r, &g, &b);
  localBrightness = getGlobalBrightness();
}

void turnLedOn()
  {   
  initAnimationsRGB();
    
    for(int i=0; i<NUMPIXELS; i++) 
      {
        setPixelColor(i, r, g, b);
        delay(DELAYVAL); // Pause before next pass through loop
      }
      showPixels();   // Send the updated pixel colors to the hardware.
  }

void turnLedOnTop()
  {   
    initAnimationsRGB();

    for(int i=0; i<NUMPIXELS; i++) 
      {
        setPixelColor(i, r, g, b);
        showPixels();   // Send the updated pixel colors to the hardware.
        delay(DELAYVAL); // Pause before next pass through loop
      }
  }

void turnLedOnBottom()
  {   
    initAnimationsRGB();

    for(int i=NUMPIXELS-1; i>=0; i--) 
      {
        setPixelColor(i, r, g, b);
        showPixels();   // Send the updated pixel colors to the hardware.
        delay(DELAYVAL); // Pause before next pass through loop
      }
  }

void turnLedOffTop()
{
  for(int i=0; i<NUMPIXELS; i++) 
    {
      setPixelColor(i, 0, 0, 0);
      showPixels();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
    }
}

void turnLedOffBottom()
{
  for(int i=NUMPIXELS-1; i>=0; i--) 
    {
      setPixelColor(i, 0, 0, 0);
      showPixels();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
    }
}

void turnLedOff()
{
  for(int i=NUMPIXELS-1; i>=0; i--) 
    {
      setPixelColor(i, 0, 0, 0);
      delay(DELAYVAL); // Pause before next pass through loop
    }
    showPixels();   // Send the updated pixel colors to the hardware.
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

void rainbow(){

  initAnimationsRGB();

  float colorNumber; 
  float saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
  float _brightness = getGlobalBrightness();
  float hue; //later change it to int to check if calculations are quicker
  long color; 
  counter = newCounter;

  for(int i=0; i<NUMPIXELS; i++) 
    {
      colorNumber = counter > numColors ? counter - numColors: counter;
      hue = (colorNumber / float(numColors)) * 360; // Number between 0 and 360
      color = HSBtoRGB(hue, saturation, _brightness);
      r = color >> 16 & 255;
      g = color >> 8 & 255;
      b = color & 255;

      setPixelColor(i, r, g, b);
      counter = (counter + 1) % (numColors * 2); //increase counter but also limit it, resets after a whole strip has exchanged twice
    }
    showPixels();
    newCounter = (newCounter+1) % 510; //510 = 2 * 255
}

void fadeIn()
{
  initAnimationsRGB();
  int rt, gt, bt;
  rt = r;
  gt = g;
  bt = b;
  float saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
  float _brightness; // = brightness; // Between 0 and 1 (0 = dark, 1 is full brightness)
  float hue = RGBtoHUE(rt,gt,bt); //extract hue from rgb values
  long color; 

  for(float j = 0.0; j<localBrightness; j+=0.01) //go through brightness levels
    {
      _brightness = j;
      for(int i=0; i<NUMPIXELS; i++) //go through specific pixels
        {
          color = HSBtoRGB(hue, saturation, _brightness); //make same color with new brightness
          rt = color >> 16 & 255;
          gt = color >> 8 & 255;
          bt = color & 255;
          setPixelColor(i, rt, gt, bt);
        }
          showPixels();   // Send the updated pixel colors to the hardware
    }
}

void fadeOut()
{
  initAnimationsRGB();
  int rt, gt, bt;
  rt = r;
  gt = g;
  bt = b;
  float _saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
  float _brightness;// = brightness; // Between 0 and 1 (0 = dark, 1 is full brightness)
  float _hue = RGBtoHUE(rt,gt,bt);
  long color; 
  

  for(float j = localBrightness; j>=0; j-=0.01)
    {
      _brightness = j;
      for(int i=0; i<NUMPIXELS; i++) 
      {
        color = HSBtoRGB(_hue, _saturation, _brightness);
        rt = color >> 16 & 255;
        gt = color >> 8 & 255;
        bt = color & 255;
        setPixelColor(i, rt, gt, bt);
      }
        showPixels();   // Send the updated pixel colors to the hardware
    }
  turnLedOff();
}
