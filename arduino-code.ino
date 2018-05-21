/*
Working well for all 6 pots
buttons working ok but could be improved

Analog Controller firmware V 1.0 by Andre Bandarra
Use with Arduino Pro Micro
Based on analog read, evan kale's multiMap funtion for log-to-linar and joystick library
for assigning channels

YouTube video: https://youtu.be/JNC-p1kS4yc
Detailed How-to: https://makershare.com/projects/diy-game-controller

*/


#include <Joystick.h>

// Constants that maps the phyical pins to the linear pots
const int slider1Pin = A0;
const int slider2Pin = A1;
const int slider3Pin = A2;
const int slider4Pin = A3;
const int pot1Pin = A6;
const int pot2Pin = A7;


//linear and pots input readings and output readings (for log pots)
int slider1Val = 0;
int rightBrake = 0;

int slider2Val = 0;
int leftBrake = 0;

int slider3Val = 0;
int rightspeed = 0;

int slider4Val = 0;
int leftspeed = 0;

int pot1Val = 0;
int rightpot = 0;

int pot2Val = 0;
int leftpot = 0;


//map for big linear log pot into linear (sliders 1 and 2)
int slider1FromMap[] = {0, 10, 28, 63, 131, 201, 306, 740, 1023};
byte slider1FromMapSize;

//map for little linear log pot into linear (sliders 3 and 4)
int slider3FromMap[] = {0, 10, 44, 83, 275, 540, 780, 940, 1012, 1023};
byte slider3FromMapSize;

//map for circular pots (linear already)
int pot1FromMap[] = {0, 1023};
byte pot1FromMapSize;

//output map for sliders (Joystick axis XYZ -127 to 127)
int sliderToMap[] = {-127, 127};
byte sliderToMapSize;

//output map for pots Joystick rotation axis XYZ 0 to 360)
int potToMap[] = {0, 360};
byte potToMapSize;

void setup() {

  // Initialize Joystick Library
  Joystick.begin();

  // Initialize Button Pins
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);

  //map tranforms?
  slider1FromMapSize = sizeof(slider1FromMap) / sizeof(int);
  slider3FromMapSize = sizeof(slider3FromMap) / sizeof(int);
  pot1FromMapSize = sizeof(pot1FromMap) / sizeof(int);

  sliderToMapSize = sizeof(sliderToMap) / sizeof(int);
  potToMapSize = sizeof(potToMap) / sizeof(int);

}

// Constant that maps the phyical pin to the joystick button.
const int pinToButtonMap = 8;

// Last state of the button
int lastButtonState[3] = {0,0,0};


void loop() {

  slider1Val = analogRead(slider1Pin);
  slider2Val = analogRead(slider2Pin);
  slider3Val = analogRead(slider3Pin);
  slider4Val = analogRead(slider4Pin);
  pot1Val = analogRead(pot1Pin);
  pot2Val = analogRead(pot2Pin);

  rightBrake = multiMap(slider1Val, slider1FromMap, slider1FromMapSize, sliderToMap, sliderToMapSize);
  leftBrake = multiMap(slider2Val, slider1FromMap, slider1FromMapSize, sliderToMap, sliderToMapSize);
  rightspeed = multiMap(slider3Val, slider3FromMap, slider3FromMapSize, sliderToMap, sliderToMapSize);
  leftspeed = multiMap(slider4Val, slider3FromMap, slider3FromMapSize, potToMap, potToMapSize);

  rightpot = multiMap(pot1Val, pot1FromMap, pot1FromMapSize, potToMap, potToMapSize);
  leftpot = multiMap(pot2Val, pot1FromMap, pot1FromMapSize, potToMap, potToMapSize);


  Joystick.setXAxis(leftBrake);
  Joystick.setYAxis(rightBrake);

  Joystick.setZAxis(rightspeed);
  Joystick.setXAxisRotation(leftspeed);

  Joystick.setYAxisRotation(rightpot);
  Joystick.setZAxisRotation(leftpot);

  for (int index = 0; index < 3; index++)
  {
    int currentButtonState = !digitalRead(index + pinToButtonMap);
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
    }
  }

  delay(50);
}


//Linear interpolates a value in fromMap to toMap from Evan Kale
int multiMap(int value, int fromMap[], int fromMapSize, int toMap[], int toMapSize)
{
  //Boundary cases
  if (value <= fromMap[0]) return toMap[0];
  if (value >= fromMap[fromMapSize - 1]) return toMap[toMapSize - 1];

  //Find the fromMap interval that value lies in
  byte fromInterval = 0;
  while (value > fromMap[fromInterval + 1])
    fromInterval++;

  //Find the percentage of the interval that value lies in
  float fromIntervalPercentage = (float)(value - fromMap[fromInterval]) / (fromMap[fromInterval + 1] - fromMap[fromInterval]);

  //Map it to the toMap interval and percentage of that interval
  float toIntervalPercentage = ((fromInterval + fromIntervalPercentage) / (fromMapSize - 1)) * (toMapSize - 1);
  byte toInterval = (byte)toIntervalPercentage;
  toIntervalPercentage = toIntervalPercentage - toInterval;

  //Linear interpolate
  return toMap[toInterval] + toIntervalPercentage * (toMap[toInterval + 1] - toMap[toInterval]);
}
