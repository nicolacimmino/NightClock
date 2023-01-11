
//
//  Copyright (C) 2022 Nicola Cimmino
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include <Wire.h>
#include <uRTCLib.h>
#include <Adafruit_APDS9960.h>
#include <TM1637Display.h>
#include "ToFSensor.h"

#define DISPLAY_CLK 4
#define DISPLAY_DIO 5
#define DISPLAY_COLON 128

uRTCLib rtc(0x68);
Adafruit_APDS9960 apds;
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);
bool dim = true;
bool active = false;
ToFSensor *tofSensor;

uint8_t displayData[] = {0x00, 0x00, 0x00, 0x00};

unsigned long lastActionTime = 0;

void blankDisplay()
{
    for (uint8_t ix = 0; ix < 4; ix++)
    {
        displayData[ix] = 0;
    }

    display.setSegments(displayData);
}

void onAction(uint8_t action)
{
    if (!active)
    {
        lastActionTime = millis();
        active = true;
    }
}

void setup()
{
    Wire.begin();

    //rtc.set(0, 05, 18, 6, 2, 5, 15);

    Serial.begin(115200);

    apds.begin();
    apds.enableProximity(true);
    apds.setProxGain(APDS9960_PGAIN_4X);

    apds.enableColor(true);

    display.setBrightness(0x00);

    blankDisplay();

    tofSensor = new ToFSensor(onAction);
}

void showTime()
{
    rtc.refresh();

    display.setBrightness(dim ? 0x00 : 0x07);

    displayData[0] = display.encodeDigit(rtc.hour() / 10);
    displayData[1] = display.encodeDigit(rtc.hour() % 10) + (millis() % 1000 > 500 ? DISPLAY_COLON : 0);
    displayData[2] = display.encodeDigit(rtc.minute() / 10);
    displayData[3] = display.encodeDigit(rtc.minute() % 10);

    display.setSegments(displayData);
}

void loop()
{
    tofSensor->loop();

    if (millis() - lastActionTime > 3000)
    {
        active = false;
        blankDisplay();
    }

    if (active)
    {
        showTime();
    }

    uint16_t r, g, b, c;

    if (apds.colorDataReady())
    {
        apds.getColorData(&r, &g, &b, &c);

        uint16_t lux = apds.calculateLux(r, g, b);

        dim = (lux < 10);
    }
}
