/**
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

#if defined (__arc__)
BLEPeripheral blePeripheral;
BLEService mapService;
BLECharacteristic driveCharacteristic;
#endif

int leftSpeed = 0;
int rightSpeed = 0;
int leftDir = 0;
int rightDir = 0;

int usTrigPin = 0;
int usEchoPin = 0;

bool obstacleAvoidEnabled = false;

/**
 * Configures Bluetooth and outputs for Make-A-Pede
 *
 * Should be called in setup()
 */
void setupMaP() {
  Serial.begin(9600);

#if defined (__arc__)
  mapService = BLEService("19B10000-E8F2-537E-4F6C-D104768A1214");
  driveCharacteristic = BLECharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 8);

  blePeripheral.setLocalName("Make-A-Pede");
  blePeripheral.setAdvertisedServiceUuid(mapService.uuid());

  blePeripheral.addAttribute(mapService);
  blePeripheral.addAttribute(driveCharacteristic);

  driveCharacteristic.setValue("");

  blePeripheral.begin();
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
  Serial1.begin(9600);
#endif

  pinMode(leftSpeedPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);

  pinMode(rightSpeedPin, OUTPUT);
  pinMode(rightDirPin, OUTPUT);
}

/**
 * Receives and processes messages from connected Bluetooth device.
 *
 * Should be called in loop().
 *
 * Calls userCode() repeatedly.
 */
void bluetoothControl() {
#if defined (__arc__)
  BLECentral central = blePeripheral.central();

  if (central) {
#endif
    unsigned long t = millis();
    unsigned long t2 = millis();

#if defined (__arc__)
    while (central.connected()) {
      if (driveCharacteristic.written()) {
        char command[8];
        strncpy(command, (char*) driveCharacteristic.value(), driveCharacteristic.valueLength());

        processCommand(command);

        t = millis();
      }
#else
    while(true) {
      if(Serial1.available() > 0) {
        char command[8];
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
        Serial1.readBytes(command, 8);
#else
        Serial.readBytes(command, 8);
#endif
        processCommand(command);

        t = millis();
      }
#endif

      if(millis()-t > 5000) {
        leftSpeed = 0;
        rightSpeed = 0;
      }

      userCode();

      if(obstacleAvoidEnabled) {
        if(usReadDistance() < 20) {
          if (millis()-t2 > 500) {
            if(leftDir == 0) {
              leftSpeed = 0;
            }

            if(rightDir == 0) {
              rightSpeed = 0;
            }
          }
        } else {
          t2 = millis();
        }
      }

      setLeftSpeed(leftSpeed);
      setRightSpeed(rightSpeed);

      setLeftDirection(leftDir);
      setRightDirection(rightDir);

      delay(20);
    }

#if defined (__arc__)
  }
#endif
}

void processCommand(char command[]) {
    int left = 0;
    int right = 0;

    char* p;
    p = strtok(command, ":");
    left = atoi(p)-127;

    p = strtok(NULL, ":");
    right = atoi(p)-127;

    leftSpeed = abs(left)*2;
    rightSpeed = abs(right)*2;

    leftSpeed = min(255, leftSpeed);
    leftSpeed = max(-255, leftSpeed);

    rightSpeed = min(255, rightSpeed);
    rightSpeed = max(-255, rightSpeed);

    leftDir = sign(left) == 1 ? LOW : HIGH;
    rightDir = sign(right) == 1 ? LOW : HIGH;
}

/**
 * Set the speed of the left side of the drive in the range of 0-255
 */
void setLeftSpeed(int s) {
  analogWrite(leftSpeedPin, s);
}

/**
 * Set the speed of the right side of the drive in the range of 0-255
 */
void setRightSpeed(int s) {
  analogWrite(rightSpeedPin, s);
}

/**
 * Set the direction of the left side of the drive (LOW (forwards) or HIGH (backwards))
 */
void setLeftDirection(int dir) {
  digitalWrite(leftDirPin, dir);
}

/**
 * Set the direction of the right side of the drive (LOW (forwards) or HIGH (backwards))
 */
void setRightDirection(int dir) {
  digitalWrite(rightDirPin, dir);
}
/**
 * Setup the ultrasonic sensor with the given trigger and echo pins
 */
void usSetup(int trig, int echo) {
  usTrigPin = trig;
  usEchoPin = echo;

  pinMode(usTrigPin, OUTPUT);
  pinMode(usEchoPin, INPUT);
}

unsigned long usReadTime() {
  if(usTrigPin == usEchoPin) return INT_MAX;

  digitalWrite(usTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(usTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(usTrigPin, LOW);

  unsigned long startMicros = micros();

  while(digitalRead(usEchoPin) == LOW) {
    if(micros()-startMicros > 500)
      return INT_MAX;
  }

  startMicros = micros();

  while(digitalRead(usEchoPin) == HIGH) {
    if(micros()-startMicros > 10000)
      return INT_MAX;
  }

  //unsigned long t = pulseIn(usEchoPin, HIGH, 10000);
  unsigned long t = micros()-startMicros;

  //Serial.println(t);

  return t;
}

/**
 * Read distance from ultrasonic sensor in inches
 */
int usReadDistance() {
  return usReadDistance(IN);
}

/**
 * Read distance from ultrasonic sensor in the specified units (CM or IN)
 */
int usReadDistance(int unit) {
  return usReadTime() / unit / 2;
}

/**
 * Enable/disable the obstacle avoid program using the ultrasonic sensor
 *
 * If enabled, robot will stop driving forwards 10 inches before hitting objects
 */
void enableObstacleAvoid(bool enable) {
  obstacleAvoidEnabled = enable;
}
