# Make-A-Pede
Arduino library for the [Make-A-Pede](http://makeapede.com)

[Make-A-Pede Android App](https://github.com/Make-A-Pede/Make-A-Pede-Android-App)

## Getting Started
Download the repository as a .zip file. In the Arduino IDE, add the library (Sketch->Include Library->Add .ZIP library...). This will add the library to your "libraries" folder. To use the library in your sketch, include it by adding ``#include <MakeAPede.h>`` to the top of the file.

Open the remote control example program by clicking File->Examples->Make-A-Pede->RemoteControl. Upload the program to your robot.

**If using the Arduino 101, the "Intel Curie Boards" package must be installed using the Boards Manager (Tools->Board->Boards Manager) and "Arduino/Genuino 101" must be selected in the Tools->Board menu.**

## Example Programs
### RemoteControl
Simple remote program using smartphone Bluetooth control.

### RemoteControlOLED
Remote program using smartphone Bluetooth control and OLED eyes.

### ObstacleAvoidAutonomous
Simple autonomous program that avoids obstacles using the antennae.

### SoundDemo
Autonomous program that will make the Make-A-Pede start driving around when it detects sound.

### StatusLEDDemo
Program that will cycle an RGB status LED through its available colors.

### PIRDemo
Autonomous program that will make the Make-A-Pede start driving around when it detects motion.

### ColorDemo
Program to demonstrate a color sensor built using an RGB LED and an LDR.

### ColorDemoIR
Autonomous program that avoids obstacles using an IR rangefinder and responds to colored cards.

# Documentation
See the [wiki](https://github.com/Automata-Development/Make-A-Pede/wiki) for complete documentation.
