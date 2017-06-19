/**
 * Code written by Mark Womack
 * Distributed under the Apache License 2.0, a copy of which should accompany
 * this file.
 */

#include "ButtonExecutor.h"

static int MAX_NUMBER_OF_CALLBACKS(MAX_NUMBER_OF_EVENTS - 1);
static long BUTTON_INTERVAL_MS(10);

static Print* _printer;
static Timer _timer;
static int8_t _buttonPin;
static int8_t _expectedButtonPressState;
static int _oldButtonState;
static boolean _isExecuting;
static void (*_sketchStartCallback)(void);
static void (*_sketchStopCallback)(void);
static int8_t _callbackReferences[MAX_NUMBER_OF_EVENTS];

void checkButton(void);
void startExecution(void);
void stopExecution(void);
void printMsg(const char msg[]);

ButtonExecutor::ButtonExecutor() {
  _printer = NULL;
}

ButtonExecutor::ButtonExecutor(Print* serialOutput) {
  _printer = serialOutput;
}

void ButtonExecutor::setup(int8_t buttonPin, int8_t expectedButtonPressState,
    void (*sketchSetupCallback)(void),
    void (*sketchStartCallback)(void),
    void (*sketchStopCallback)(void)) {

  printMsg("*** Setting up");
			
  _buttonPin = buttonPin;
  _expectedButtonPressState = expectedButtonPressState;
  _oldButtonState = !expectedButtonPressState;
  _isExecuting = false;
  _sketchStartCallback = sketchStartCallback;
  _sketchStopCallback = sketchStopCallback;
  for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
    _callbackReferences[index] = TIMER_NOT_AN_EVENT;
  }

  // Call the sketchSetupCallback just once
  (*(sketchSetupCallback))();

  // Register internal callback for tracking button pushes
  pinMode(_buttonPin, INPUT);
  _timer.every(BUTTON_INTERVAL_MS, checkButton);
  
  printMsg("*** Ready to start execution");
}

void ButtonExecutor::loop() {
  _timer.update();
}

int8_t ButtonExecutor::callbackEveryByMillis(unsigned long periodInMs,
    void (*callback)(void)) {

  // Find the next open callback reference index
  for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
    if (_callbackReferences[index] != TIMER_NOT_AN_EVENT) {
      continue;
    }
    
    // Register the callback, store and return the reference
    _callbackReferences[index] = _timer.every(periodInMs, callback);
    return _callbackReferences[index];
  }
  
  // Maximum number of callbacks already installed!
  return CALLBACK_NOT_INSTALLED;
}

int8_t ButtonExecutor::callbackEveryByHertz(unsigned long periodInHz,
    void (*callback)(void)) {
  // Convert frequency in hertz to milliseconds
  int callbackMillis = (1000/periodInHz);
  return this->callbackEveryByMillis(callbackMillis, callback);
}
  

int8_t ButtonExecutor::stopCallback(int8_t callbackId) {

  // Find the callback id in the stored references
  for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
	  if (_callbackReferences[index] != callbackId) {
		  continue;
	  }
	  
	  // Stop the callback, clear the stored reference
    _timer.stop(callbackId);
	  _callbackReferences[index] = TIMER_NOT_AN_EVENT;
	  return CALLBACK_STOPPED;
  }
  
  // The callback id not found
  return CALLBACK_NOT_INSTALLED;
}

void ButtonExecutor::abortExecution() {
  printMsg("*** Aborting execution by request!");
  stopExecution();
}

/**
 * This is an internal static method that is called periodically to check the
 * state of the pin that is connected to the button being monitored for state.
 * It is checked every BUTTON_INTERVAL_MS to allow for bouncing/noise in the
 * button.
 */
void checkButton(void) {
  int currentButtonState = digitalRead(_buttonPin);
  if (currentButtonState == _expectedButtonPressState 
        && currentButtonState != _oldButtonState) {
	  if (!_isExecuting) {
		  startExecution();
	  } else {
		  stopExecution();
	  }
  }
  _oldButtonState = currentButtonState;
}

/**
 * This is an internal static method that is used to start the execution of the
 * code. It calls the sketchStartCallback that was registered in the
 * ButtonExecutor.setup method.
 */
void startExecution(void) {
  // If already executing, then just return
  if (_isExecuting) {
	  return;
  }
  
  printMsg("*** Starting execution");
  
  (*(_sketchStartCallback))();
  _isExecuting = true;
}

/**
 * This is an internal static method that is used to stop the execution of the
 * code. It stops all registered callbacks and then calls the sketchStopCallback
 * method that was registered in the ButtonExecutor.setup method.
 */
void stopExecution(void) {
  // If not executing, then just return
  if (!_isExecuting) {
	  return;
  }
  
	printMsg("*** Stopping execution");
	
  // Stop execution of all registered callbacks
  for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
        _timer.stop(_callbackReferences[index]);
	  _callbackReferences[index] = TIMER_NOT_AN_EVENT;
  }

  (*(_sketchStopCallback))();
  _isExecuting = false;
  
	printMsg("*** Ready to start execution");
}

/**
 * Helper method to print debug messages.
 */
void printMsg(const char msg[]) {
  if (_printer) {
    _printer->println(msg);
  }
}
