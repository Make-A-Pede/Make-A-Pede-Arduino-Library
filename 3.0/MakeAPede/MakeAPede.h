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
#include <CurieBLE.h>
#include <limits.h>

#define sign(i) (i < 0 ? -1 : 1)
#define CM 28
#define IN 71

const int leftSpeedPin = 5;
const int leftDirPin = 4;

const int rightSpeedPin = 6;
const int rightDirPin = 7;

void setupMaP();

void bluetoothControl();

void setLeftSpeed(int s);
void setRightSpeed(int s);

void setLeftDirection(int dir);
void setRightDirection(int dir);

void usSetup(int trig, int echo);
int usReadDistance();
int usReadDistance(int unit);

void enableObstacleAvoid(bool enable);

void userCode();

extern BLEPeripheral blePeripheral;
extern BLEService mapService;
extern BLECharacteristic driveCharacteristic;

extern int leftSpeed;
extern int rightSpeed;
extern int leftDir;
extern int rightDir;

extern int usTrigPin;
extern int usEchoPin;

extern bool obstacleAvoidEnabled;

#endif

