/**
 * Code written by Mark Womack
 * Distributed under the Apache License 2.0, a copy of which should accompany this file.
 */

#include "Arduino.h"
#include "Timer.h"
#include "ButtonExecutor.h"

#define MAX_NUMBER_OF_CALLBACKS (MAX_NUMBER_OF_EVENTS - 1)

static final long BUTTON_INTERVAL_MS(10);

ButtonExecutor::ButtonExecutor(void) {
	
}

// Called to set everything up for the executor to execute the sketch.
public void ButtonExecutor::setup(int8_t buttonPin, void (*sketchSetupCallback)(),
		void (*sketchStartCallback)(), void (*sketchStopCallback)()) {

	SerialDebugger.println("*** Setting up");
			
	_executorContext.buttonPin = buttonPin;
	_executorContext.oldButtonState = LOW;
	_executorContext.isExecuting = false;
	_executorContext.sketchStartCallback = sketchStartCallback;
	_executorContext.sketchStopCallback = sketchStopCallback;
	for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
		_executorContext.callbackReferences[index] = TIMER_NOT_AN_EVENT;
	}
	
	// Call the sketchSetupCallback just once
    (*(sketchSetupCallback))();
    
    // Register internal callback for tracking button pushes
	pinMode(_executorContext.buttonPin, INPUT);
	_executorContext.timer.every(BUTTON_INTERVAL_MS, checkButton, (void*)&_executorContext);
	
	SerialDebugger.println("*** Ready to start execution");
}

// Called by the sketch to execute a loop.
public void ButtonExecutor::loop() {
	_executorContext.timer.update();
}

// Called by the sketch to register a callback that will get executed as part
// of the loop processing.
int8_t ButtonExecutor::callbackEvery(unsigned long period,
		void (*callback)(void*), void* context) {
			
	for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
		if (_executorContext.callbackReferences[index] != TIMER_NOT_AN_EVENT) {
			continue;
		}
		_executorContext.callbackReferences[index] = 
			_executorContext.timer.every(period, callback, context);
		return _executorContext.callbackReferences[index];
	}
	return CALLBACK_NOT_INSTALLED;
}

// Called by the sketch to stop the execution of specific callback with the
// given thread id.
public int8_t ButtonExecutor::callbackStop(int8_t threadId) {
	for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
		if (_executorContext.callbackReferences[index] != threadId) {
			continue;
		}
		_executorContext.callbackReferences[index] = 
			_executorContext.timer.stop(threadId);
		return _executorContext.callbackReferences[index];
	}
	return CALLBACK_NOT_INSTALLED;
}

// Can be called by the sketch to abort execution of the executor.
public void ButtonExecutor::abortExecution() {
	SerialDebugger.println("*** Aborting execution by request!");
	stopExecution(&_executorContext);
}

/**
 * This is an internal method that is called periodically to check the state of the
 * pin that is connected to the button being monitored for state. It is checked
 * every BUTTON_INTERVAL_MS to allow for bouncing/noise in the button.
 */
private void checkButton(void* context) {
	ExecutorContext* executorContext = (ExecutorContext*)context;
	int currentButtonState = digitalRead(executorContext->buttonPin);
	if (currentButtonState == HIGH && executorContext->oldButtonState == LOW) {
		if (!executorContext->isExecuting) {
			startExecution(executorContext);
		} else {
			stopExecution(executorContext);
		}
	}
	executorContext->oldButtonState = currentButtonState;
}

// This is a static method that is used to start execution.  It first calls
// the sketchSetupCallback method and then is schedules execution of the execute method.
private void startExecution(ExecutorContext* executorContext) {
	// If already executing, then just return
	if (executorContext->isExecuting) {
		return;
	}
	SerialDebugger.println("*** Starting execution");
	(*(executorContext->sketchStartCallback))();
	executorContext->isExecuting = true;
}

// This is a static method that is used to end execution.  It first stops the
// calls to the execute method and then it calls the sketchStopCallback method.
private void stopExecution(ExecutorContext* executorContext) {
	// If not executing, then just return
	if (!executorContext->isExecuting) {
		return;
	}
	SerialDebugger.println("*** Stopping execution");
	
	// Stop execution of all registered callbacks
	for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
		executorContext->callbackReferences[index] = 
			executorContext->timer.stop(
				executorContext->callbackReferences[index]);
	}
	
	(*(executorContext->sketchStopCallback))();
	executorContext->isExecuting = false;
	SerialDebugger.println("*** Ready to start execution");
}
