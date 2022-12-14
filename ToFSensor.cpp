
#include "ToFSensor.h"

ToFSensor::ToFSensor(void (*actionCallback)(uint8_t action))
{
    this->sensor = new VL53L0X();
    this->sensor->setTimeout(500);
    this->sensor->init();
    this->sensor->startContinuous();
    this->calibrate();
    this->actionCallback = actionCallback;
}

void ToFSensor::calibrate()
{
    for (uint8_t ix = 0; ix < 10; ix++)
    {
        uint16_t distanceMilliMeters = this->sensor->readRangeContinuousMillimeters();
        this->averageDistance = (this->averageDistance * 0.9) + (distanceMilliMeters * 0.1);
        delay(100);
    }
}

void ToFSensor::loop()
{
    if (!this->sensor->timeoutOccurred())
    {
        uint16_t distanceMilliMeters = this->sensor->readRangeContinuousMillimeters();
        this->averageDistance = max(1, (this->averageDistance * 0.9) + (distanceMilliMeters * 0.1));

        bool beamCurrentlyCut = distanceMilliMeters < (this->averageDistance * 0.8);

        if (beamCurrentlyCut && !this->beamCut)
        {
            this->lastBeamCutTime = millis();
            this->beamCuts++;
            delay(100);
        }

        this->beamCut = beamCurrentlyCut;
    }

    if (this->beamCuts && millis() - lastBeamCutTime > GESTURE_TIMEOUT_MS)
    {
        this->actionCallback(this->beamCuts);
        this->beamCuts = 0;
    }
}

bool ToFSensor::isActive()
{
    return this->beamCut;
}
