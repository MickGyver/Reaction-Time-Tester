# Reaction Time Tester
A simple circuit for accurately testing your reaction time to visual stimuli.

## Building a tester
1. Connect a button to pin 2 and GND of the Arduino.
2. Optional: connect an LED to a pin on the Arduino through a resistor or use the built in LED (default)
3. Upload this code to the Arduino.

## How to use
1. Install Putty or other Telnet client (https://www.putty.org/).
1. Connect the Arduino to USB.
2. In device manager or Arduino IDE, note which COM port of the Arduino.
3. Start Putty and set
       Connection type: Serial 
       Serial line: COM* (where * is the number of the COM port of the Arduino)
       Speed: 115200
   (You can save these settings for later if you wish)
4. Click Open. You should now see the message "Starting Reaction Test!" in the terminal window.
5. Do the test by pressing the button as soon as possible every time the LED turns on. You can stop
   at any time to see your results in the terminal window.
6. To restart the test, unplug and reconnect the Arduino, restart Putty.
