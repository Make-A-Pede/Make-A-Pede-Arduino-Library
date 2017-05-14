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

BLEPeripheral blePeripheral;
BLEService mapService;
BLECharacteristic driveCharacteristic;

int leftSpeed = 0;
int rightSpeed = 0;
int leftDir = 0;
int rightDir = 0;

int usTrigPin = 0;
int usEchoPin = 0;

bool obstacleAvoidEnabled = false;

void setupMaP() {
  Serial.begin(9600);

  mapService = BLEService("19B10000-E8F2-537E-4F6C-D104768A1214");
  driveCharacteristic = BLECharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 8);
  
  blePeripheral.setLocalName("Make-A-Pede");
  blePeripheral.setAdvertisedServiceUuid(mapService.uuid());

  blePeripheral.addAttribute(mapService);
  blePeripheral.addAttribute(driveCharacteristic);

  driveCharacteristic.setValue("");

  blePeripheral.begin();
  
  pinMode(leftSpeedPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  
  pinMode(rightSpeedPin, OUTPUT);
  pinMode(rightDirPin, OUTPUT);
}

void bluetoothControl() {
  BLECentral central = blePeripheral.central();

  if (central) {
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      int left = 0;
      int right = 0;
      
      if (driveCharacteristic.written()) {
        char command[8];
        strncpy(command, (char*) driveCharacteristic.value(), driveCharacteristic.valueLength());

        char* p;
        p = strtok(command, ":");
        left = atoi(p)-127;
    
        p = strtok(NULL, ":");
        right = atoi(p)-127;

        leftSpeed = abs(left)*2;
        rightSpeed = abs(right)*2;

        leftDir = sign(left) == 1 ? LOW : HIGH;
        rightDir = sign(right) == 1 ? LOW : HIGH;
      }

      userCode();

      if(usReadDistance() < 10 && obstacleAvoidEnabled) {
        if(leftDir == 0) {
          leftSpeed = 0;
        }

        if(rightDir == 0) {
          rightSpeed = 0;
        }
      }

      setLeftSpeed(leftSpeed);
      setRightSpeed(rightSpeed);

      setLeftDirection(leftDir);
      setRightDirection(rightDir);

      delay(20);
    }
    
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

void setLeftSpeed(int s) {
  analogWrite(leftSpeedPin, s);
}

void setRightSpeed(int s) {
  analogWrite(rightSpeedPin, s);
}

void setLeftDirection(int dir) {
  digitalWrite(leftDirPin, dir);
}

void setRightDirection(int dir) {
  digitalWrite(rightDirPin, dir);
}

void usSetup(int trig, int echo) {
  usTrigPin = trig;
  usEchoPin = echo;

  pinMode(usTrigPin, OUTPUT);
  pinMode(usEchoPin, INPUT);
}

int usReadTime() {
  if(usTrigPin == usEchoPin) return INT_MAX;
  
  digitalWrite(usTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(usTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(usTrigPin, LOW);

  return pulseIn(usEchoPin, HIGH, 500);
}

int usReadDistance() {
  return usReadDistance(IN);
}

int usReadDistance(int unit) {
  return usReadTime() / unit / 2;
}

void enableObstacleAvoid(bool enable) {
  obstacleAvoidEnabled = enable;
}

