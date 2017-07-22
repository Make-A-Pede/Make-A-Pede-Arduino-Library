# Status LED Example
Sample program for using a RGB status LED with the Make-A-Pede.

## Parts
1 - Assembled Make-A-Pede<br>
1 - RGB LED Breakout Board (common cathode)<br>
4 - M-F Jumper Wires<br>

OR

1 - Assembled Make-A-Pede<br>
1 - RGB LED (common cathode)<br>
3 - Resistors<br>
4 - M-F Jumper Wires<br>

OR

1 - Assembled Make-A-Pede<br>
1 - Red LED<br>
1 - Green LED<br>
1 - Blue LED<br>
3 - Resistors<br>
4 - M-F Jumper Wires<br>

## Wiring
If you are using a breakout board, connect it to your Make-A-Pede according to the table below:

GND -> G pin, port 10<br>
R - > S pin, port 8<br>
G - > S pin, port 9<br>
B - > S pin, port 10

If you are using an RGB LED, connect it to your Make-A-Pede as shown below:
<br><br>
<img src="https://raw.githubusercontent.com/Make-A-Pede/Make-A-Pede-CAD/master/sensor-examples/MaP-Status-LED.png" width="600"/>

If you are using three LEDs, connect them to your Make-A-Pede as shown below:
<br><br>
<img src="https://raw.githubusercontent.com/Make-A-Pede/Make-A-Pede-CAD/master/sensor-examples/MaP-Status-LED-Set.png" width="600"/>

## Testing
Load StatusLEDDemo.ino onto your Arduino. Turn on the Make-A-Pede and trigger one of the antenna sensors to start the program. The LED will cycle through the 7 available colors.

## Programming
There are two commands available in the Make-A-Pede library to control the LED:

###`setupRGB(redPin, greenPin, bluePin);`

setupRGB is used to set which pins will be used to control the LED. Default values are 8, 9, and 10.

###`setRGBColor(color);`

setRGBColor is used to set the LED to a specific color. Valid inputs are:

0 - Off<br>
1 - Red<br>
2 - Yellow<br>
3 - Green<br>
4 - Cyan<br>
5 - Blue<br>
6 - Magenta<br>
7 - White

The default value is 0.

