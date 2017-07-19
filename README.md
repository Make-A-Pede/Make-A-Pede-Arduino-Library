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

### ObstacleAvoidAutonomous
Simple autonomous program that avoids obstacles using the antennae.

### PIRDemo
Autonomous program that will make the Make-A-Pede start driving around when it detects motion.

# Documentation
See the [wiki](https://github.com/Automata-Development/Make-A-Pede/wiki) for complete documentation.
