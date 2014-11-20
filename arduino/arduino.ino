/* pouzity zdroje:
	http://www.instructables.com/id/Arduino-controlled-light-dimmer-The-circuit/?ALLSTEPS
	http://vinduino.blogspot.cz/2013/07/hc-sr04-ultrasonic-sensor.html

 HC-SR04 Ping distance sensor:
 VCC to arduino 5v 
 GND to arduino GND
 Echo to Arduino pin 7 
 Trig to Arduino pin 8
 */

#include  <TimerOne.h>          // Avaiable from  http://www.arduino.cc/playground/Code/Timer1

#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin
#define LEDPin 13 // Onboard LED

#define MIN 110
#define MAX 0
int lastDistance;

byte ledPin = 10;

int maximumRange = 100; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

volatile int i=0;               // Variable to use as a counter
volatile boolean zero_cross=0;  // Boolean to store a "switch" to tell us if we have crossed zero
int AC_pin = 9;                 // Output to Opto Triac
int dim = MIN;              // Dimming level (0-128)  0 = on, 128 = 0ff
// 40W - off=118  on=0    minimum=110

int freqStep = 75;

void setup() {    
  Serial.begin(9600);  // Begin setup
  pinMode(AC_pin, OUTPUT);                          // Set the Triac pin as output
  attachInterrupt(0, zero_cross_detect, RISING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  Timer1.initialize(freqStep);                      // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(dim_check, freqStep);                                         
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)
}

void zero_cross_detect() {    
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  i=0;
  digitalWrite(AC_pin, LOW);       // turn off TRIAC (and AC)
}                                 

// Turn on the TRIAC at the appropriate time
void dim_check() {                   
  if(zero_cross == true) {              
    if(i>=dim) {                     
      digitalWrite(AC_pin, HIGH); // turn on light       
      i=0;  // reset time step counter                         
      zero_cross = false; //reset zero cross detection
    } 
    else {
      i++; // increment time step counter                     
    }                                
  }                                  
}                                   

void loop() {
  
 /* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;
 //MINIMUM
 
 if (distance >= maximumRange) {
   dim = MAX;
 } else if (distance <= minimumRange) {
   dim = MIN;
 } else {
   dim = map(distance, minimumRange, maximumRange, MIN, MAX);
        //map(value, fromLow, fromHigh, toLow, toHigh) 
 }
  
 Serial.print("val= ");
 Serial.println(distance);

 //Delay 50ms before next reading.
 delay(50);
}

