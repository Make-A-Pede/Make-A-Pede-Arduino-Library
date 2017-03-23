/*
 * MakeAPede.h - Software library for the Make-A-Pede (makeapede.com)
 * Copyright (C) 2017 Automata-Development
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MakeAPede_h
#define MakeAPede_h

#include "Arduino.h"
#include <Wire.h>
#include "Adafruit_MCP23017.h"

#define FORWARD 1
#define BACKWARD 2
#define NONE 0

typedef struct pins {
  int pin;
  int pwmValue;
  bool pinState;
  int pwmTickCount;
} PwmPin;

const int pinCount = 16;
const uint8_t motorPorts[16] = {A0, A1, A2, A3, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2};
const uint8_t sensorPorts[16] = {15, 14, 13, 12, 11, 10, 9, 8, 0, 1, 2, 3, 4, 5, 6, 7};

const byte pwmMax = 100;
const unsigned long microInterval = 250;

class MakeAPede {
  public:
    void start();
    void setMotor(uint8_t pin, uint8_t setting);
    void setAllMotors(uint8_t setting);
    void setPodMotors(uint8_t pod, uint8_t lSetting, uint8_t rSetting);
    void setPodConfig(uint8_t podConfig[8][3]);
    void setForwardMotors(uint8_t lSetting, uint8_t rSetting);
    void setBackwardMotors(uint8_t lSetting, uint8_t rSetting);
    void sensorPinMode(uint8_t pin, uint8_t mode);
    uint8_t getSensor(uint8_t pin);
    void setOutput(uint8_t pin, uint8_t setting);
    void setupSpeaker(uint8_t pin);
    void setSpeaker(uint8_t setting);

    void setupPWMPins();
    void updateMotors();
    void wait(unsigned long ms);

    void btControl();
    void btSend(int data);
    void setBTEnabled(bool b);

    void setupUSSensor(int triggerPin, int echoPin);
    float getUSSensor();

    void backgroundTasks();

  private:
    Adafruit_MCP23017 mcp;
  
    uint8_t sensorData[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t pods[8][3] = {{FORWARD, 11, 10},
                          {FORWARD, 9, 8},
                          {FORWARD, 7, 5},
                          {FORWARD, 4, 3},
                          {FORWARD, 2, 1},
                          {NONE, 0, 0},
                          {NONE, 0, 0},
                          {NONE, 0, 0}};


    uint8_t speakerPin = 0;
    uint8_t speakerValue = 0;
    int speakerTickCount = 0;

    unsigned long currentMicros = micros();
    unsigned long previousMicros = 0;

    unsigned long btTimeOfLastMessage = millis();

    PwmPin pwmPins[pinCount];

    bool btEnabled = false;

    int usTriggerPin = 0;
    int usEchoPin = 0;
};

#endif

