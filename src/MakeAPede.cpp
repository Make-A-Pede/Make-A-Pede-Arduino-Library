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

#if defined(USE_DISPLAY)
Adafruit_SSD1306 displayRight = Adafruit_SSD1306();
Adafruit_SSD1306 displayLeft = Adafruit_SSD1306();
#endif

int lSpeedPin = 5;
int lDirPin = 4;

int rSpeedPin = 6;
int rDirPin = 7;

int lAntennaePin = 12;
int rAntennaePin = 13;

int leftSpeed = 0;
int rightSpeed = 0;
int leftDir = 0;
int rightDir = 0;

int usTrigPin = 0;
int usEchoPin = 0;

int redPin = 8;
int greenPin = 9;
int bluePin = 10;

bool obstacleAvoidEnabled = false;

/**
 * Configures Bluetooth and outputs for Make-A-Pede
 *
 * Should be called in setup()
 */
void setupMaP(int lsp, int ldp, int rsp, int rdp, int lap, int rap) {
  Serial.begin(9600);

  lSpeedPin = lsp;
  lDirPin = ldp;

  rSpeedPin = rsp;
  rDirPin = rdp;

  lAntennaePin = lap;
  rAntennaePin = rap;

#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
  Serial1.begin(9600);
#endif

  pinMode(lSpeedPin, OUTPUT);
  pinMode(lDirPin, OUTPUT);

  pinMode(rSpeedPin, OUTPUT);
  pinMode(rDirPin, OUTPUT);

  pinMode(lAntennaePin, INPUT);
  pinMode(rAntennaePin, INPUT);

#if defined(USE_DISPLAY)
  setupDisplay();
  showEyes();
#endif
}

/**
 * Configures RGB status LED
 *
 * Should be called in setup()
 */
void setupRGB(int rp, int gp, int bp) {
  redPin = rp;
  greenPin = gp;
  bluePin = bp;

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Set LED to green once setup is complete
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, LOW);
}

void processCommand(char command[]) {
  Serial.println(command);

  int radius = 0;
  float angle = 0;

  char *p;
  p = strtok(command, ":");
  radius = atoi(p);

  p = strtok(NULL, ":");
  angle = atoi(p);

  angle = (angle * ((2.0 * PI) / 360.0));

  int x = radius * cos(angle);
  int y = radius * sin(angle);

  leftSpeed = (y + x) * (255.0 / 100.0);
  rightSpeed = (y - x) * (255.0 / 100.0);

  leftDir = sign(leftSpeed) == 1 ? LOW : HIGH;
  rightDir = sign(rightSpeed) == 1 ? LOW : HIGH;

  leftSpeed = abs(leftSpeed);
  rightSpeed = abs(rightSpeed);

  leftSpeed = min(255, leftSpeed);
  leftSpeed = max(0, leftSpeed);

  rightSpeed = min(255, rightSpeed);
  rightSpeed = max(0, rightSpeed);

  if (sign(y) == -1) {
    int temp = leftSpeed;
    leftSpeed = rightSpeed;
    rightSpeed = temp;
  }

  if (rightDir != leftDir) {
    if (sign(x) > 0) {
      rightSpeed = 0;
    } else {
      leftSpeed = 0;
    }
  }
}

/**
 * Receives and processes messages from connected Bluetooth device.
 *
 * Should be called in loop().
 *
 * Calls userCode() repeatedly.
 */
void bluetoothControl() {
  unsigned long t, t2;
  t = t2 = millis();

  while (true) {
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
    if (Serial1.available() > 0) {
      char command[8];
      Serial1.readBytes(command, 8);
#else
    if (Serial.available() > 0) {
      char command[8];
      Serial.readBytes(command, 8);
#endif
      processCommand(command);

      t = millis();
    }

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
}

/**
 * Set the speed of the left side of the drive in the range of 0-255
 */
void setLeftSpeed(int s) {
  int speed = min(255, s);
  speed = max(0, speed);
  analogWrite(lSpeedPin, speed);
}

/**
 * Set the speed of the right side of the drive in the range of 0-255
 */
void setRightSpeed(int s) {
  int speed = min(255, s);
  speed = max(0, speed);
  analogWrite(rSpeedPin, speed);
}

/**
 * Set the direction of the left side of the drive (LOW (forwards) or HIGH
 * (backwards))
 */
void setLeftDirection(int dir) { digitalWrite(lDirPin, dir); }

/**
 * Set the direction of the right side of the drive (LOW (forwards) or HIGH
 * (backwards))
 */
void setRightDirection(int dir) { digitalWrite(rDirPin, dir); }

/**
 * Sets the color of the RGB status LED
 * 0-7 = off, red, yellow, green, cyan, blue, magenta, white
 */
void setRGBColor(int color) {
  switch (color) {
  case 1: // red
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
    break;
  case 2: // yellow
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
    break;
  case 3: // green
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
    break;
  case 4: // cyan
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH);
    break;
  case 5: // blue
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH);
    break;
  case 6: // magenta
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH);
    break;
  case 7: // white
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH);
    break;
  default: // off
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
    break;
  }
}

int getLeftAntennae() { return digitalRead(lAntennaePin); }

int getRightAntennae() { return digitalRead(rAntennaePin); }

/**
 * Enable/disable the obstacle avoid program using the ultrasonic sensor
 *
 * If enabled, robot will stop driving forwards 10 inches before hitting objects
 */
void enableObstacleAvoid(bool enable) { obstacleAvoidEnabled = enable; }

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

  displayRight.drawLine(50, 4, displayRight.width() - 50, 4, WHITE);
  displayRight.drawLine(50, 4, 40, 7, WHITE);
  displayRight.drawLine(displayRight.width() - 50, 4, displayRight.width() - 40, 7, WHITE);

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

  displayLeft.drawLine(50, 4, displayLeft.width() - 50, 4, WHITE);
  displayLeft.drawLine(50, 4, 40, 7, WHITE);
  displayLeft.drawLine(displayLeft.width() - 50, 4, displayLeft.width() - 40, 7, WHITE);

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

void showClosedEyes() {
  displayRight.clearDisplay();

  displayRight.drawLine(40, 7, displayRight.width() - 40, 3, WHITE);

  displayRight.drawRoundRect(displayRight.width() / 2 - OUTER_EYE_WIDTH / 2,
                             displayRight.height() - OUTER_EYE_HEIGHT - 1,
                             OUTER_EYE_WIDTH, OUTER_EYE_HEIGHT,
                             OUTER_EYE_HEIGHT / 2 + 2, WHITE);

  displayRight.drawLine(displayRight.width() / 2 - INNER_EYE_WIDTH / 2 - 4,
                        displayRight.height() - INNER_EYE_HEIGHT / 2 - 1,
                        displayRight.width() / 2 + INNER_EYE_WIDTH / 2 + 4,
                        displayRight.height() - INNER_EYE_HEIGHT / 2 - 1,
                        WHITE);

  displayRight.display();

  displayLeft.clearDisplay();

  displayLeft.drawLine(40, 3, displayLeft.width() - 40, 7, WHITE);

  displayLeft.drawRoundRect(displayLeft.width() / 2 - OUTER_EYE_WIDTH / 2,
                            displayLeft.height() - OUTER_EYE_HEIGHT - 1,
                            OUTER_EYE_WIDTH, OUTER_EYE_HEIGHT,
                            OUTER_EYE_HEIGHT / 2 + 2, WHITE);

  displayLeft.drawLine(displayLeft.width() / 2 - INNER_EYE_WIDTH / 2 - 4,
                       displayLeft.height() - INNER_EYE_HEIGHT / 2 - 1,
                       displayLeft.width() / 2 + INNER_EYE_WIDTH / 2 + 4,
                       displayLeft.height() - INNER_EYE_HEIGHT / 2 - 1, WHITE);

  displayLeft.display();
}
#endif
