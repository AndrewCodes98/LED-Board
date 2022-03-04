/*
 * Program Name: LED_Board
 * Written by: Andrew Froning
 * Date Began: 2/8/22
 * 
 * Purpose: This program is used to control a strip (or strips in parallel) of WS2812 addressable LEDs. 
 *    -Tachometer - used to measured wheel rotation speed (skateboard speed)
 *    -Function to Take any number of colors and light them on LEDs going down board at same speed as board is moving.
 *    
 * Specifications:
 *    -Wheel Diameter (where magnet lies) = 5.1cm    --->   Wheel Circumference = C = 16.0cm = 0.16m
 *    -Distance between each LED = d_LED = (1m / 60 LEDs) = 0.01667m
 *    -Ratio of d_LED to C = 0.01667m / 0.16m = 0.104
 *    -Therefore, time between two consecutive LEDs lighting up = t_LED = 0.104 * t_period
 *    
 */

 
////////////////////////////////////////////////////////////////////
//DO NOT MODIFY ANY OF THE FOLLOWING
////////////////////////////////////////////////////////////////////
#include <FastLED.h> //Library used to address WS2812 LEDs

#define HALL_PIN 2    //Pin connected to Hall Sensor Data Input
#define LEDS_PIN 8    //Pin connected to LED data input
#define NUM_LEDS 59   //Number of LEDS per strip on board

CRGB leds[NUM_LEDS];  // Initialize array of LEDs

//Time measurement variables in microseconds
volatile unsigned long period = 10000; //Will hold 
volatile unsigned long lastMeasurement;

const float ratio = 0.104;
const unsigned short int readingsPerRevolution = 1;

unsigned short int iterations = 0; //Counts the number of cycles copmleted in the duration of LEDS_APART
unsigned short int color = 0; // Iterates between each color

void Pulse_Found();
void LED_Speed_Match();

////////////////////////////////////////////////////////////////////
//END OF DO NOT EDIT BLOCK
////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////
//YOU MAY MODIFY THE FOLOWWING ITEMS
////////////////////////////////////////////////////////////////////

//Specify distance between lit LEDs. Every Nth LED will be lit.
#define LEDS_APART 8  

//Specify the number of colors to use.
#define colors 3 

//Specify RGB color values for each color
CRGB colorArr[colors] = {CRGB(100,0,0), CRGB(0,100,0), CRGB(0,0,100)};

////////////////////////////////////////////////////////////////////
//END OF EDIT BLOCK
////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////
//DO NOT MODIFY ANY OF THE FOLLOWING
////////////////////////////////////////////////////////////////////
void setup(){
 
  Serial.begin(9600); //Set BAUD Rate. High rate is not necessary here, 9600 is plenty.

  //Set up Interruption Pin for Rising Signal
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), Pulse_Found, RISING);

  //Set up LED Strip
  FastLED.addLeds<WS2812, LEDS_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,500);
  FastLED.clear();
  FastLED.show();
  
  delay(100); //Delays 100ms to ensure micors() is positive and running smoothly
}

////////////////////////////////////////////////////////////////////
//END OF DO NOT EDIT BLOCK
////////////////////////////////////////////////////////////////////


void loop(){
  LED_Speed_Match();

  
}


void LED_Speed_Match(){
  
  //Move all array values up one index
  for(int i = NUM_LEDS - 1; i > 0; i--)
    leds[i] = leds[i - 1];

  //Determine first index's value
  if(iterations == 0){
      leds[0] = colorArr[color];
      ++color %= colors;
  }else
    leds[0] = 0;

  //Reset iterations to avoid overflow
  ++iterations %= LEDS_APART;

  FastLED.show();  

  delay(ratio * readingsPerRevolution * period / 1000);
  
  // If board stops moving, stop lights where they are
  if(micros() - lastMeasurement < 2000000){
    for(int i = 0; i < NUM_LEDS; i++){ // Loops through all LEDs
      period
    }
  }
}

void Pulse_Found(){
  period = micros() - lastMeasurement;
  lastMeasurement = micros();
}
