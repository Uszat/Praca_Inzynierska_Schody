#ifndef ANIMATIONS_H
#define ANIMATIONS_H

    #include "Colors.h"

    typedef enum
    {
        MOVING_IN_IN,
        MOVING_IN_OUT,
        FADE,
        OUTSIDE_IN,
        NO_ANIM,
    } animationState;

    void initAnimationsRGB(void);
    void turnLedOn(void);
    void turnLedOnTop(void);
    void turnLedOnBottom(void);
    void turnLedOffTop(void);
    void turnLedOffBottom(void);
    void turnLedOff(void);
    void animationOn(bool top);
    void animationOff(bool top);
    void rainbow(void);
    void fadeIn(void);
    void fadeOut(void);

#endif /* ANIMATIONS_H */
