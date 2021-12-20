#ifndef COLORS_H
#define COLORS_H

    #include "Animations.h"
    #include <Adafruit_NeoPixel.h>

    void initGlobalRGB(void);
    // void setGlobalRGB(int localR, int localG, int localB);
    // long getGlobalRGB();
    void setLocalRGBtoGlobalRGB(int* localR, int* localG, int* localB);
    float getGlobalBrightness();
    void initPixels(void);
    void setPixelColor(int i, int r, int g, int b);
    void showPixels(void);
    void makeNewBrightness(void);
    void setLedBrightness(char*, char*);
    void setAndShowColor(char* tempBuf, char* buf);
    long HSBtoRGB(float, float, float);
    float RGBtoHUE(int, int, int);

#endif /* COLORS_H */
