
#ifndef __TOF_SENSOR__
#define __TOF_SENSOR__

#include <Wire.h>
#include <VL53L0X.h>

#define GESTURE_TIMEOUT_MS 10

class ToFSensor
{
public:
    ToFSensor(void (*actionCallback)(uint8_t action));
    void loop();
    bool isActive();
    
private:
    VL53L0X *sensor;
    uint16_t averageDistance;
    unsigned long lastBeamCutTime = 0;
    bool beamCut = false;
    uint8_t beamCuts = 0;
    void (*actionCallback)(uint8_t action);
    void calibrate();
};

#endif