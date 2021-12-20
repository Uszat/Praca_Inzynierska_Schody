#ifndef AUTO_MODE_H
#define AUTO_MODE_H

    #include "Animations.h"
    #include "Comm.h"
    #include "Sensors.h"

    void initVariables(void);
    void setAutoMode(void);
    void resetAutoMode(void);
    bool getAutoMode(void);
    void performAutomaticLights(void);
    void performRainbowAnimation(void);
    bool timeout(void);
    void resetState(void);
    void manageSensorLED(void);

#endif /* AUTO_MODE_H */