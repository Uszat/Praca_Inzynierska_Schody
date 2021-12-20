#include "Colors.h"

#define PIN       15
#define NUMPIXELS 300

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

float globalBrightness    = 0.5;
unsigned char globalRed   = 0;
unsigned char globalGreen = 0;
unsigned char globalBlue  = 0;
extern bool ledOn;
extern bool rainbowSet;







typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}









void initGlobalRGB()
{
  globalRed = 255;
  globalGreen = 0;
  globalBlue = 0;
}

float getGlobalBrightness()
{
  return globalBrightness;
}

// void setGlobalRGB(int localR, int localG, int localB)
// {
//   globalRed = localR;
//   globalGreen = localG;
//   globalBlue = localB;
// }

// long getGlobalRGB()
// {
//   return long((globalRed << 16) | (globalGreen << 8) | (globalBlue));
// }

void setLocalRGBtoGlobalRGB(int* localR, int* localG, int* localB)
{
  *localR = globalRed;
  *localG = globalGreen;
  *localB = globalBlue;
}

void initPixels()
{
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  delay(100);
  pixels.clear();
}

void setPixelColor(int i, int r, int g, int b)
{
  pixels.setPixelColor(i, pixels.Color(r, g, b));
}

void showPixels()
{
  pixels.show();
}

//extract hue from rgb, then create new rgb from the hue and new brightness
void makeNewBrightness()
{
  float saturation = 1;
  float _brightness = globalBrightness;
//  float hue = RGBtoHUE(globalRed, globalGreen, globalBlue);
  long color; 
//  color = HSBtoRGB(hue, saturation, _brightness);


    rgb inColors;
    inColors.r = globalRed;
    inColors.g = globalGreen;
    inColors.b = globalBlue;

    hsv hsvColor = rgb2hsv(inColors);
    color = HSBtoRGB(hsvColor.h , hsvColor.s, getGlobalBrightness());

    globalRed   = color >> 16 & 255;
    globalGreen = color >> 8 & 255;
    globalBlue  = color & 255;
}

//set the actual new brightness into a brightness variable, create new rgb and turn leds on
void setLedBrightness(char* tempBuf, char* buf)
{
  float br;
  for(int i = 2, j = 0; i<4; i++, j++)
  {
    tempBuf[j] = buf[i];
  }
  br = (float)strtol(tempBuf, NULL, 10);

  globalBrightness = br / 100.0;

  makeNewBrightness();

  if(ledOn && !rainbowSet)
  {
    turnLedOn();
  }
}

void setAndShowColor(char* tempBuf, char* buf){
  for(int i = 0; i < 6; i+=2)
    {
      for(int j = 0; j < 2; j++)
      {
        tempBuf[j] = buf[i+j];
      }
      if(i==0)
      {
        globalRed = (int)strtol(tempBuf, NULL, 16);   
      }
      else if(i==2)
      {
        globalGreen = (int)strtol(tempBuf, NULL, 16);;
      }
      else if(i==4)
      {
        globalBlue = (int)strtol(tempBuf, NULL, 16);
      }
    }
    
    long color; 
    float saturation;
    // if(globalRed == 255 && globalGreen == 255 && globalBlue == 255)
    // {
    //   saturation = 0;
    // } else
    // {
    //   saturation = 1;
    // }
    
    rgb inColors;
    inColors.r = globalRed;
    inColors.g = globalGreen;
    inColors.b = globalBlue;

    hsv hsvColor;
    hsvColor = rgb2hsv(inColors);

    // float hue = RGBtoHUE(globalRed ,globalGreen, globalBlue);

    // Serial.print("hue ");
    // Serial.println(hue);
    // color = HSBtoRGB(hue, saturation, getGlobalBrightness());
    color = HSBtoRGB(hsvColor.h , hsvColor.s, getGlobalBrightness());
    globalRed   = color >> 16 & 255;
    globalGreen = color >> 8 & 255;
    globalBlue  = color & 255;

    Serial.println(globalRed);
    Serial.println(globalGreen);
    Serial.println(globalBlue);
    Serial.println();
    
    if(ledOn)
    {
      turnLedOn();
    }
}

long HSBtoRGB(float _hue, float _sat, float _brightness) 
{
  float red = 0.0;
  float green = 0.0;
  float blue = 0.0;
  
  if (_sat == 0.0) 
  {
    red = _brightness;
    green = _brightness;
    blue = _brightness;
  } else {
    if (_hue == 360.0) 
    {
      _hue = 0;
    }

    int slice = _hue / 60.0;
    float hue_frac = (_hue / 60.0) - slice;

    float aa = _brightness * (1.0 - _sat);
    float bb = _brightness * (1.0 - _sat * hue_frac);
    float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
    
    switch(slice) 
    {
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
