//
#include <FastLED.h>

#define HALL_PIN 2 
#define LEDS_PIN 8
#define NUM_LEDS 59
#define LEDS_APART 8
#define colors 2 //Number of colors desiredl

volatile unsigned long period = 10000;
volatile unsigned long lastMeasurement;
double freq = 0.0;
unsigned int counter = 0;

CRGB leds[NUM_LEDS];

void setup(){
  //Set BAUD Rate
  Serial.begin(9600);

  //Set up Interruption Pin for Rising Signal
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), Pulse_Found, RISING);

  //Set up LED Strip
  FastLED.addLeds<WS2812, LEDS_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,500);
  FastLED.clear();
  FastLED.show();
  
  //Ensure micors() is positive and running smoothly
  delay(200);
}

void loop(){
  
  //Reset Strip so all LEDS are Off
  FastLED.clear();
  int color = 0;

  for(int i = 0; i < NUM_LEDS; i++){ // Loops through all LEDs

    //Monochrmatic Setting
    if((i - counter) % LEDS_APART == 0){
      if((i - counter) % (LEDS_APART * colors) == 0)
        leds[i] = CRGB(100,0,0);
      else
        leds[i] = CRGB(0,100,0);
    }
  }

  FastLED.show();
  delay(0.104 * period / 1000); // 0.104 was calculated from wheel measurements
  Serial.print(period);
    Serial.print("hello");


  counter++;
  if(counter > LEDS_APART)
    counter = 0;
}

void Pulse_Found(){
  period = micros() - lastMeasurement;
  lastMeasurement = micros();
}
