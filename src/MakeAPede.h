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
#include <limits.h>

//#define USE_IMU
//#define USE_DISPLAY

#if defined (__arc__)
#include <CurieBLE.h>

#if defined(USE_IMU)
#include <CurieTimerOne.h>
#include <CurieIMU.h>
#include <MadgwickAHRS.h>
#endif
#endif

#if defined(USE_DISPLAY)
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define OUTER_EYE_WIDTH 30
#define OUTER_EYE_HEIGHT 20
#define INNER_EYE_WIDTH 16
#define INNER_EYE_HEIGHT 10
#endif

#define sign(i) (i < 0 ? -1 : 1)

const int leftSpeedPin = 5;
const int leftDirPin = 4;

const int rightSpeedPin = 6;
const int rightDirPin = 7;

const int leftAntennaePin = 12;
const int rightAntennaePin = 13;

void setupMaP();

void bluetoothControl();

void setLeftSpeed(int s);
void setRightSpeed(int s);

void setLeftDirection(int dir);
void setRightDirection(int dir);

int getLeftAntennae();
int getRightAntennae();

void enableObstacleAvoid(bool enable);

void userCode();

#if defined (__arc__)
extern BLEPeripheral blePeripheral;
extern BLEService mapService;
extern BLECharacteristic driveCharacteristic;

#if defined(USE_IMU)
extern BLECharacteristic headingCharacteristic;

void getHeading();

extern Madgwick filter;
extern unsigned long microsPerReading;
extern float heading;
#endif
#endif

#if defined(USE_DISPLAY)
void setupDisplay();
void showEyes();

extern Adafruit_SSD1306 displayRight;
extern Adafruit_SSD1306 displayLeft;
#endif

extern int leftSpeed;
extern int rightSpeed;
extern int leftDir;
extern int rightDir;

extern bool obstacleAvoidEnabled;

#endif
