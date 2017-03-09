/*
 * MakeAPede.cpp - Software library for the Make-A-Pede (makeapede.com)
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

#include "MakeAPede.h"

/**
 * Initialize IO for the Make-A-Pede
 * 
 * Must be called in setup before anything other MaP calls
 */
void MakeAPede::start() {
  mcp.begin();

  for (char i = 0; i < 16; i++) {
    pinMode(motorPorts[i], OUTPUT);
    digitalWrite(motorPorts[i], LOW);

    mcp.pinMode(sensorPorts[i], INPUT);
    mcp.pullUp(sensorPorts[i], HIGH);
  }

  setupPWMPins();

  Serial.begin(9600);
}

/**
 * Sets motor on the specified pin (1-16) to the specified setting (0-100)
 */
void MakeAPede::setMotor(uint8_t pin, uint8_t setting) {
  pwmPins[pin-1].pwmValue = setting;
}

/**
 * Sets all motors to the specified setting
 */
void MakeAPede::setAllMotors(uint8_t setting) {
  for (int i = 1; i <= 16; i++) {
    setMotor(i, setting);
  }
}

/**
 * Sets the motors on the specified pod to the specified settings
 */
void MakeAPede::setPodMotors(uint8_t pod, uint8_t lSetting, uint8_t rSetting) {
  setMotor(pods[pod-1][1], lSetting);
  setMotor(pods[pod-1][2], rSetting);
}

/**
 * Sets all motors specified to be running forward in the pod config to the specified setting
 * 
 * Pod config can be changed using setPodConfig
 */
void MakeAPede::setForwardMotors(uint8_t lSetting, uint8_t rSetting) {
  for (int i = 0; i < 8; i++) {
    if (pods[i][0] == FORWARD) {
      setMotor(pods[i][1], lSetting);
      setMotor(pods[i][2], rSetting);
    }
  }
}

/**
 * Sets all motors specified to be running backward in the pod config to the specified setting
 * 
 * Pod config can be changed using setPodConfig
 */
void MakeAPede::setBackwardMotors(uint8_t lSetting, uint8_t rSetting) {
  for (int i = 0; i < 8; i++) {
    if (pods[i][0] == BACKWARD) {
      setMotor(pods[i][1], lSetting);
      setMotor(pods[i][2], rSetting);
    }
  }
}

/**
 * Sets the given array as the pod config
 * 
 * Array must be an array containing 8 arrays with 3 elements each.
 * The first element of the inner arrays is the direction of the pod (FORWARD, BACKWARD, or NONE),
 * the second is the port number of the left motor on the pod (1-16),
 * and the third is port number of the right motor on the pod (1-16).
 */
void MakeAPede::setPodConfig(uint8_t podConfig[8][3]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 3; j++) {
      pods[i][j] = podConfig[i][j];
    }
  }
}

/**
 * Gets the value of the digital sensor on the specified port. Returns either "HIGH" or "LOW".
 */
uint8_t MakeAPede::getSensor(uint8_t pin) {
  return mcp.digitalRead(sensorPorts[pin - 1]);
}

/**
 * Configures a speaker on teh given pin
 */
void MakeAPede::setupSpeaker(uint8_t pin) {
  speakerPin = motorPorts[pin];
}

/**
 * Sets the speaker to the specified value
 */
void MakeAPede::setSpeaker(uint8_t setting) {
  speakerValue = setting;
}

/*
 * Configures software PWM
 */
void MakeAPede::setupPWMPins() {
  for (int index = 0; index < pinCount; index++) {
    pwmPins[index].pin = motorPorts[index];

    pwmPins[index].pwmValue = 0;

    pwmPins[index].pinState = HIGH;
    pwmPins[index].pwmTickCount = 0;
  }
}

/*
 * Updates motor values based on their PWM settings and handles the speaker
 * 
 * Speaker functionality is currently not working
 * 
 * Must be called every ~250 micros to control motors
 */
void MakeAPede::updateMotors() {
  currentMicros = micros();
  
  if (currentMicros - previousMicros >= microInterval) {
    for (int index = 0; index < pinCount; index++) {
      if(pwmPins[index].pin == speakerPin) continue;
      
      pwmPins[index].pwmTickCount++;

      if (pwmPins[index].pinState == HIGH) {
        if (pwmPins[index].pwmTickCount >= pwmPins[index].pwmValue) {
          pwmPins[index].pinState = LOW;
        }
      } else {
        if (pwmPins[index].pwmTickCount >= pwmMax) {
          pwmPins[index].pinState = HIGH;
          pwmPins[index].pwmTickCount = 0;
        }
      }

      if(pwmPins[index].pwmValue == 0) {
        pwmPins[index].pinState = LOW;
      }
      
      digitalWrite(pwmPins[index].pin, pwmPins[index].pinState);
    }

    if(speakerPin != 0 && speakerValue > 0) {
      speakerTickCount++;

      if(speakerTickCount < speakerValue) {
        digitalWrite(speakerPin, HIGH);
      } else if(speakerTickCount >= speakerValue && speakerTickCount < speakerValue*2) {
        digitalWrite(speakerPin, LOW);
      } else {
        speakerTickCount = 0;
      }
    }
    
    previousMicros = currentMicros;
  }
}

/*
 * Waits for the specified time is millis while periodically calling backgroundTasks
 */
void MakeAPede::wait(unsigned long ms) {
  unsigned long t = millis();

  while(millis()-t < ms) {
    backgroundTasks();
    delayMicroseconds(200);
  }
}

/**
 * Recieves bluetooth messages from the app, interprets them, and sets motors to the appropriate values
 * 
 * Must be called frequently for smooth control
 */
void MakeAPede::btControl() {
  if(btEnabled) {
    if(Serial.available() > 0) {
      char command[9];
      Serial.readBytes(command, 9);
      
      char* p;
      p = strtok(command, ":");
      int left = atoi(p);
  
      p = strtok(NULL, ":");
      int right = atoi(p);

      p = strtok(NULL, ":");
      int speaker = atoi(p);
  
      setForwardMotors(left, right);

      setSpeaker(speaker == 1 ? 500 : 0);
  
      btTimeOfLastMessage = millis();
    } else if(millis() - btTimeOfLastMessage > 5000) {
      setForwardMotors(0, 0);
    }
  }
}

/**
 * Sends a int message over bluetooth
 */
void MakeAPede::btSend(int data) {
  Serial.print(data);
}

/**
 * Sets whether or not the bluetooth is enabled
 */
void MakeAPede::setBTEnabled(bool b) {
  btEnabled = b;
}

/**
 * Configures the ultrasonic sensor
 */
void MakeAPede::setupUSSensor(int triggerPin, int echoPin) {
  mcp.pinMode(triggerPin, OUTPUT);
  mcp.pullUp(triggerPin, LOW);
  
  mcp.pinMode(echoPin, INPUT);
  mcp.pullUp(echoPin, LOW);

  usTriggerPin = triggerPin;
  usEchoPin = echoPin;
}

/**
 * Pings the ultrasonic sensor and waits for a response. Returns the distance read by the sensor in inches.
 * 
 * Currently non-functional
 */
float MakeAPede::getUSSensor() {
  mcp.digitalWrite(usTriggerPin, LOW);
  delayMicroseconds(2);
  mcp.digitalWrite(usTriggerPin, HIGH);
  delayMicroseconds(10);
  mcp.digitalWrite(usTriggerPin, LOW);

  unsigned long t = micros();
  
  while(mcp.digitalRead(usEchoPin) != HIGH) {
    if(micros()-t > 1000000) return -1;
    //backgroundTasks();
  }
  
  t = micros();
  
  while(mcp.digitalRead(usEchoPin) != LOW) {
    if(micros()-t > 1000000) return -1;
    //backgroundTasks();
  }
  
  unsigned long elapsedTime = micros()-t;

  return (elapsedTime/2.0) / 29.1;
}

/**
 * Calls updateMotors and btControl. Should be called every ~250 micros.
 */
void MakeAPede::backgroundTasks() {
  updateMotors();
  btControl();
}

