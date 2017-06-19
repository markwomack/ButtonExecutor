/**
 * Code written by Mark Womack
 * Distributed under the Apache License 2.0, a copy of which should accompany
 * this file.
 * 
 * Example code that demonstrates basic usage of the ButtonExecutor class.
 * You will need a basic circuit with a momentary push button. This sample
 * assumes it is connected to pin 12 of the Arduino. A circuit diagram of 
 * the circuit should have accompanied this file.
 */
 
#include <ButtonExecutor.h>

//ButtonExecutor buttonExecutor;
// or
ButtonExecutor buttonExecutor(&Serial);

int count;

void setup() {
  Serial.begin(9600);

  // Monitor pin 12 for button pushes which will be HIGH
  buttonExecutor.setup(12, HIGH, sketchSetup, sketchStart, sketchStop);
}

void loop() {
  buttonExecutor.loop();
}

// Called when the buttonExecutor is set up
void sketchSetup(void) {
  Serial.println("sketchSetup called!");
}

// Called when the buttonExecutor is started with button push
void sketchStart(void) {
  Serial.println("sketchStart called!");
  
  count = 0;

  // Called every second
  buttonExecutor.callbackEveryByMillis(1000, &sampleCallbackOneSecond);
  // or
  //buttonExecutor.callbackEveryByHertz(1, &sampleCallbackOneSecond);
  
  
  // Called every half second
  buttonExecutor.callbackEveryByMillis(500, &sampleCallbackOneHalfSecond);
  // or
  //buttonExecutor.callbackEveryByHertz(2, &sampleCallbackOneHalfSecond);

  // Called every two seconds, there is no equivalent by hertz...
  buttonExecutor.callbackEveryByMillis(2000, &sampleCallbackCounter);
}

// Called when buttonExecutor stopped with button push
void sketchStop(void) {
  Serial.println("sketchStop called!");
}

void sampleCallbackOneSecond(void) {
  Serial.println("sampleCallbackOneSecond called!");
}

void sampleCallbackOneHalfSecond(void) {
  Serial.println("sampleCallbackOneHalfSecond called!");
}

void sampleCallbackCounter(void) {
  count++;
  Serial.print("sampleCallbackCounter called - ");
  Serial.print("Count: ");
  Serial.print(count);
  Serial.println();
}


