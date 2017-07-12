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

#if defined(__arc__)
BLEPeripheral blePeripheral;
BLEService mapService;
BLECharacteristic driveCharacteristic;

#if defined(USE_IMU)
BLECharacteristic headingCharacteristic;

Madgwick filter;
unsigned long microsPerReading;
float heading;
bool headingUpdated = false;
#endif
#endif

#if defined(USE_DISPLAY)
Adafruit_SSD1306 displayRight = Adafruit_SSD1306();
Adafruit_SSD1306 displayLeft = Adafruit_SSD1306();
#endif

int leftSpeed = 0;
int rightSpeed = 0;
int leftDir = 0;
int rightDir = 0;

int usTrigPin = 0;
int usEchoPin = 0;

bool obstacleAvoidEnabled = false;

static constexpr size_t HeadingCharacteristicSize = sizeof(float);

/**
 * Configures Bluetooth and outputs for Make-A-Pede
 *
 * Should be called in setup()
 */
void setupMaP() {
  Serial.begin(115200);

#if defined(__arc__)
  mapService = BLEService("19B10000-E8F2-537E-4F6C-D104768A1214");
  driveCharacteristic = BLECharacteristic(
      "19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 8);

  blePeripheral.setLocalName("Make-A-Pede");
  blePeripheral.setAdvertisedServiceUuid(mapService.uuid());

#if defined(USE_IMU)
  headingCharacteristic =
      BLECharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214",
                        BLERead | BLENotify, HeadingCharacteristicSize);
  mapService.addCharacteristic(headingCharacteristic);

  CurieIMU.begin();
  CurieIMU.setGyroRate(25);
  CurieIMU.setAccelerometerRate(25);
  filter.begin(25);

  CurieIMU.setAccelerometerRange(2);
  CurieIMU.setGyroRange(250);

  microsPerReading = 1000000u / 25;
#endif
  mapService.addCharacteristic(driveCharacteristic);

  blePeripheral.addAttribute(mapService);

  blePeripheral.begin();
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
  Serial1.begin(9600);
#endif

  pinMode(leftSpeedPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);

  pinMode(rightSpeedPin, OUTPUT);
  pinMode(rightDirPin, OUTPUT);

  pinMode(leftAntennaePin, INPUT);
  pinMode(rightAntennaePin, INPUT);

  setupDisplay();
  showEyes();
}

void processCommand(char command[]) {
  Serial.println(command);

  int left = 0;
  int right = 0;

  char* p;
  p = strtok(command, ":");
  left = atoi(p) - 127;

  p = strtok(NULL, ":");
  right = atoi(p) - 127;

  leftSpeed = abs(left) * 2;
  rightSpeed = abs(right) * 2;

  leftSpeed = min(255, leftSpeed);
  leftSpeed = max(-255, leftSpeed);

  rightSpeed = min(255, rightSpeed);
  rightSpeed = max(-255, rightSpeed);

  leftDir = sign(left) == 1 ? LOW : HIGH;
  rightDir = sign(right) == 1 ? LOW : HIGH;
}

/**
 * Receives and processes messages from connected Bluetooth device.
 *
 * Should be called in loop().
 *
 * Calls userCode() repeatedly.
 */
void bluetoothControl() {
#if defined(__arc__)
  BLECentral central = blePeripheral.central();

  if (central) {
#if defined(USE_IMU)
    CurieTimerOne.start(microsPerReading, getHeadingInterrupt);
#endif
#endif
    unsigned long t, t2;
    t = t2 = millis();

#if defined(__arc__)
    while (central.connected()) {
      if (driveCharacteristic.written()) {
        char command[8];
        strncpy(command, (char *)driveCharacteristic.value(),
                driveCharacteristic.valueLength());

        processCommand(command);

        t = millis();
      }

#if defined(USE_IMU)
      if (millis() - t2 > 100 && headingUpdated) {
        char buffer[HeadingCharacteristicSize];
        sprintf(buffer, "%f", heading);
        headingCharacteristic.writeValue(
            reinterpret_cast<unsigned char *>(buffer),
            HeadingCharacteristicSize);

        t2 = millis();
      }
#endif

#else
  while (true) {
    if (Serial1.available() > 0) {
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

      if (millis() - t > 5000) {
        leftSpeed = 0;
        rightSpeed = 0;
      }

      userCode();

      if (obstacleAvoidEnabled &&
          (getLeftAntennae() == HIGH || getRightAntennae() == HIGH)) {
        if (leftDir == 0) {
          leftSpeed = 0;
        }

        if (rightDir == 0) {
          rightSpeed = 0;
        }
      }

      setLeftSpeed(leftSpeed);
      setRightSpeed(rightSpeed);

      setLeftDirection(leftDir);
      setRightDirection(rightDir);

      delay(20);
    }

#if defined(__arc__)
#if defined(USE_IMU)
    CurieTimerOne.stop();
#endif
  }
#endif
}

/**
 * Set the speed of the left side of the drive in the range of 0-255
 */
void setLeftSpeed(int s) { analogWrite(leftSpeedPin, s); }

/**
 * Set the speed of the right side of the drive in the range of 0-255
 */
void setRightSpeed(int s) { analogWrite(rightSpeedPin, s); }

/**
 * Set the direction of the left side of the drive (LOW (forwards) or HIGH
 * (backwards))
 */
void setLeftDirection(int dir) { digitalWrite(leftDirPin, dir); }

/**
 * Set the direction of the right side of the drive (LOW (forwards) or HIGH
 * (backwards))
 */
void setRightDirection(int dir) { digitalWrite(rightDirPin, dir); }

int getLeftAntennae() { return digitalRead(leftAntennaePin); }

int getRightAntennae() { return digitalRead(rightAntennaePin); }

/**
 * Enable/disable the obstacle avoid program using the ultrasonic sensor
 *
 * If enabled, robot will stop driving forwards 10 inches before hitting objects
 */
void enableObstacleAvoid(bool enable) { obstacleAvoidEnabled = enable; }

#if defined(__arc__) && defined(USE_IMU)
float convertRawAcceleration(int aRaw) {
  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  float g = (gRaw * 250.0) / 32768.0;
  return g;
}

void getHeadingInterrupt() {
  if (!CurieIMU.dataReady())
    return;

  int aix, aiy, aiz;
  int gix, giy, giz;
  float ax, ay, az;
  float gx, gy, gz;

  noInterrupts();
  CurieIMU.readMotionSensor(aix, aiy, aiz, gix, giy, giz);
  interrupts();

  ax = convertRawAcceleration(aix);
  ay = convertRawAcceleration(aiy);
  az = convertRawAcceleration(aiz);
  gx = convertRawGyro(gix);
  gy = convertRawGyro(giy);
  gz = convertRawGyro(giz);

  filter.updateIMU(gx, gy, gz, ax, ay, az);
  heading = filter.getYaw();

  headingUpdated = true;
}
#endif

#if defined(USE_DISPLAY)
void setupDisplay() {
  displayRight.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  displayLeft.begin(SSD1306_SWITCHCAPVCC, 0x3D);

  displayRight.display();
  displayLeft.display();

  delay(250);
}

void showEyes() {
  displayRight.clearDisplay();

  displayRight.drawLine(40, 7, displayRight.width() - 40, 3, WHITE);

  displayRight.drawRoundRect(displayRight.width() / 2 - OUTER_EYE_WIDTH / 2,
                             displayRight.height() - OUTER_EYE_HEIGHT - 1,
                             OUTER_EYE_WIDTH, OUTER_EYE_HEIGHT,
                             OUTER_EYE_HEIGHT / 2 + 2, WHITE);
  displayRight.fillRoundRect(displayRight.width() / 2 - INNER_EYE_WIDTH / 2,
                             displayRight.height() - INNER_EYE_HEIGHT - 1,
                             INNER_EYE_WIDTH, INNER_EYE_HEIGHT,
                             INNER_EYE_HEIGHT / 2 + 1, WHITE);

  displayRight.display();

  displayLeft.clearDisplay();

  displayLeft.drawLine(40, 3, displayLeft.width() - 40, 7, WHITE);

  displayLeft.drawRoundRect(displayLeft.width() / 2 - OUTER_EYE_WIDTH / 2,
                            displayLeft.height() - OUTER_EYE_HEIGHT - 1,
                            OUTER_EYE_WIDTH, OUTER_EYE_HEIGHT,
                            OUTER_EYE_HEIGHT / 2 + 2, WHITE);
  displayLeft.fillRoundRect(displayLeft.width() / 2 - INNER_EYE_WIDTH / 2,
                            displayLeft.height() - INNER_EYE_HEIGHT - 1,
                            INNER_EYE_WIDTH, INNER_EYE_HEIGHT,
                            INNER_EYE_HEIGHT / 2 + 1, WHITE);

  displayLeft.display();
}
#endif
