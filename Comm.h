#ifndef COMM_H
#define COMM_H

    #include "BluetoothSerial.h"
    #include "AutoMode.h"
    #ifdef __AVR__
        #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
    #endif /* __AVR__ */

    void initSerial(void);
    void receiveBTData(void);
    bool arrayContains(char*, long x, char*);
    void setAction(void); 
    void debugData(bool isOn);
    void resetBuffers(void);

#endif /* COMM */
