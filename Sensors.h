#ifndef SENSORS_H
#define SENSORS_H

    #include "Adafruit_VL53L0X.h"

    void setSensorTop(void);
    void resetSensorTop(void);
    bool getSensorTop(void);
    void setSensorBottom(void);
    void resetSensorBottom(void);
    bool getSensorBottom(void);
    void setupTOF(void);
    void read_dual_sensors(void);
    void checkSensors(void);

#endif /* SENSORS_H */
