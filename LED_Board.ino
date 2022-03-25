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
 * Common Errors While Running Program:
 *    -avrdude: ser_open(): can't open device "/dev/cu.usbmodem11101": No such file or directory
 *        -Go to Tools -> Board: Select Board
 *                     -> Processor: Select Processor
 *                     -> Port: Select Port
 *    -avrdude: stk500_getsync(): not in sync: resp=0x00
 *        -Restart Arduino IDE
 *        
 *        
 * Future Plans:
 *    -Determine direction of movement
 *    -Alternative Bluetooth Option allowing RGB value commands
 *    -PONG Game
 */ 

 
////////////////////////////////////////////////////////////////////
//DO NOT MODIF ANY OF THE FOLLOWING
////////////////////////////////////////////////////////////////////
#include <FastLED.h> //Library used to address WS2812 LEDs

const byte HALL_PIN = 2;    //Pin connected to Hall Sensor Data Input
const byte LEDS_PIN = 8;    //Pin connected to LED data input
const byte NUM_LEDS = 59;   //Number of LEDS per strip on board

unsigned short int iterations = 0; //Counts the number of cycles copmleted in the duration of LEDS_APART
unsigned short int color = 0; // Iterates between each color

const unsigned short int readingsPerRevolution = 1; //Number of magnets on a wheel
const float calibration = 0.082; // Calibration to match speed


//Time measurement variables
volatile unsigned long period = 10000;    // Measured in Milliseconds
volatile unsigned long lastMeasurement;   // Measured in Milliseconds
volatile unsigned long currentTime;       // Measured in Milliseconds


CRGB leds[NUM_LEDS];  // Initialize array of LEDs



//Function Declarations
void Pulse_Found();
void LED_Speed_Match();
void Ramping_LED_Ends();

////////////////////////////////////////////////////////////////////
//END OF DO NOT EDIT BLOCK
////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////
//YOU MAY MODIFY THE FOLOWWING ITEMS
////////////////////////////////////////////////////////////////////

const byte  LEDS_APART = 10;  //Specify distance between lit LEDs. Every Nth LED will be lit.
const byte colors = 3;      //Specify the number of colors to use.
const byte maxBrightness = 100;  //255 max

CRGB colorArr[colors] = {CRGB(maxBrightness,0,0), CRGB(0,maxBrightness,0), CRGB(0,0,maxBrightness)}; //Specify RGB color values for each color

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
  
  delay(100); //Delays 100ms to ensure millis() is positive and running smoothly.
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

  ++iterations %= LEDS_APART;  //Reset iterations to avoid overflow


  Ramping_LED_Ends();

  FastLED.show();  




  // Wait for LEDs to shine before moving to next set of LEDs to shine
  currentTime = millis();
  while(millis() < currentTime + calibration * readingsPerRevolution * period){
    // We want to use millis() and a while loop rather than delay()
    // since delay() does not allow for interupts.
  }


  
  // If board stops moving, stop lights where they are
  if(millis() - lastMeasurement > 1000){  //If no magnet detection for 1000 milliseconds
    currentTime = millis();
    while(lastMeasurement < currentTime){
      // Wait some more.
    }
  }
}





void Ramping_LED_Ends(){
/////////////////////////////////////////////////////////////////////////////////
//WARNING: pow(base,power) MUST BE LESS THAN LOWEST BRIGHTNESS STATED IN colorArr
/////////////////////////////////////////////////////////////////////////////////

  int base = 2;
  int power = 5; // N - 1 elements to ramp
    
  leds[0].r = leds[0].r / pow(base,power);
  leds[0].g = leds[0].g / pow(base,power);
  leds[0].b = leds[0].b / pow(base,power);

  Serial.println(leds[0].r);

  for(int i = 1; i < power; i++){

    // Front of LED Strip
    leds[i].r = leds[i].r * base;
    leds[i].g = leds[i].g * base;
    leds[i].b = leds[i].b * base;

    // End of LED Strip
    leds[NUM_LEDS-power+i].r = leds[NUM_LEDS-power+i].r / base;
    leds[NUM_LEDS-power+i].g = leds[NUM_LEDS-power+i].g / base;
    leds[NUM_LEDS-power+i].b = leds[NUM_LEDS-power+i].b / base;
  }

  
}



void Pulse_Found(){
  period = millis() - lastMeasurement;
  lastMeasurement = millis();
}
