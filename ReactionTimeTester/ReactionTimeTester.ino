/*
 * REACTION TIME TESTER
 * by  MickGyver (Mikael Norrgard) 2020
 * 
 * Based on controller lag test code used for the MiSTer project by the user jorge_ on
 * the "Classic Gaming" Discord server.
 * 
 * Licensed under CC BY-NC-SA 4.0 (Attribution-NonCommercial-ShareAlike 4.0 International)
 * https://creativecommons.org/licenses/by-nc-sa/4.0/
 *
 * BUILDING A TESTER
 * 1. Connect a button to pin 2 and GND of the Arduino.
 * 2. Optional: connect an LED to a pin on the Arduino through a resistor or use the built in LED (default)
 * 3. Upload this code to the Arduino.
 *
 * HOW TO USE
 * 1. Install Putty or other Telnet client (https://www.putty.org/).
 * 1. Connect the Arduino to USB.
 * 2. In device manager or Arduino IDE, note which COM port of the Arduino.
 * 3. Start Putty and set
 *        Connection type: Serial 
 *        Serial line: COM* (where * is the number of the COM port of the Arduino)
 *        Speed: 115200
 *    (You can save these settings for later if you wish)
 * 4. Click Open. You should now see the message "Starting Reaction Test!" in the terminal window.
 * 5. Do the test by pressing the button as soon as possible every time the LED turns on. You can stop
 *    at any time to see your results in the terminal window.
 * 6. To restart the test, unplug and reconnect the Arduino, restart Putty.
 *
 */

// #define KTRL // Leave this commented out, used for my KTRL gamepad only

enum TestState {
  WaitLed,
  WaitPress,
  PrintResults,
  WaitRelease
};

volatile unsigned long startMicros;
volatile unsigned long duration;
volatile unsigned long ISRCounter = 0;
volatile TestState state = WaitLed;

int pinLed = LED_BUILTIN;  // Use the built in LED or connect an external LED via a resistor to a pin.
int pinButton = 2;         // Connect a button to this pin and GND. Needs to be an interrupt pin.

unsigned long timeLed = 0; // Time to turn on LED (will be randomized)

void setup() {

  pinMode(pinButton,INPUT_PULLUP);
  #ifdef KTRL
    DDRB |= B10000000;
  #else
    pinMode(pinLed,OUTPUT);
  #endif
  
  // External interrupt 
  attachInterrupt(digitalPinToInterrupt(pinButton), timePress, FALLING);

  // Waits for serial monitor before starting
  while (!Serial);
  Serial.begin(115200);
  Serial.println("Starting Reaction Test!");
  Serial.println("Get ready!");

  timeLed = millis() + 3000 + random(3000);
}


void loop() 
{
  switch(state)
  {
    case WaitLed:
      if(millis() >= timeLed) 
      {
        if(digitalRead(pinButton) == HIGH)
        {
          #ifdef KTRL
            PORTB |= B10000000;
          #else
            digitalWrite(pinLed, HIGH);
          #endif
          startMicros = micros();
          state = WaitPress;
        }
        else 
        {
          timeLed = millis() + 1000 + random(3000);
        }
      }
      break;

    case WaitPress:
      // Do nothing, interrupt will handle it
      break;

    case PrintResults:
      printResults();
      state = WaitRelease;
      break;

    case WaitRelease:
      if(digitalRead(pinButton) == HIGH) 
      {
         #ifdef KTRL
           PORTB &= ~B10000000;
         #else
           digitalWrite(pinLed, LOW);
         #endif
         timeLed = millis() + 1000 + random(3000);
         state = WaitLed;
      }
      break;
  }
}


// Interrupt function, just timestamps and prints :)
void timePress()
{
  if(state == WaitPress)
  {
    ISRCounter += 1;
    duration = micros() - startMicros;
    state = PrintResults;
  }
}

// Clears screen between stats outputs
void clearScreen()
{
  Serial.write(27);      
  Serial.print("[2J");
  Serial.write(27);
  Serial.print("[H");
}

// Print the results
void printResults() 
{
  clearScreen();
  
  Serial.print("Samples: "); 
  Serial.println(ISRCounter);
  
  Serial.print("Average: ");
  Serial.print(getAverage(duration)/1000,4);  
  Serial.println(" ms");
  
  Serial.print("Max:     ");
  Serial.print(getMax(duration)/1000,3);
  Serial.println(" ms");
  
  Serial.print("Min:     ");
  Serial.print(getMin(duration)/1000,3);
  Serial.println(" ms");
  
  Serial.print("Std Dev: ");
  Serial.print(getStdDev(duration)/1000,3);
  Serial.println(" ms");
}

// Functions to calculate Average, Min, Max, and Std Dev
float getAverage(unsigned long int newNum)
{
  static unsigned long int numSamples = 1;
  static float curAvg;
  
  curAvg = curAvg + (newNum - curAvg) / numSamples;
  numSamples++;
  
  return curAvg;
}

float getMax(unsigned long int newNum)
{
  static unsigned long int maxVal = 0;
  
  if(newNum > maxVal){
    maxVal = newNum;
  }

  return maxVal;
}

float getMin(unsigned long int newNum)
{
  static unsigned long int minVal = 4294967295;
  
  if(newNum < minVal){
    minVal = newNum;
  }

  return minVal;
}

float getStdDev(unsigned long int newNum)
{
  static float M = 0.0;
  static float S = 0.0;
  static unsigned long int i = 1;
  
  float tmpM = M;
  M += (newNum - tmpM) / i;
  S += (newNum - tmpM) * (newNum - M);
  i++;
  
  return sqrt(S/(i-2));
}
