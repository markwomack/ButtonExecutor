/**
 * Code written by Mark Womack
 * Distributed under the Apache License 2.0, a copy of which should accompany this file.
 *
 * This library provides code that will monitor a push button on an Arduino microcontroller.
 * When the button is pushed, registered callbacks will be called to start and stop the
 * execution of code. Other callbacks can also be registered that will only be executed when
 * the program is started. Setup and usage is very simple. Please see the examples for
 * guidance on how to use in your own program.
 *
 * This library is dependent on Simon Monk's Timer library which at the time of this
 * writing can be found at https://github.com/JChristensen/Timer. It will need to be installed
 * as an Arduino library in your Arduino development environment for this libary to
 * correctly compile.
 *
 */

#ifndef ButtonExecutor_h
#define ButtonExecutor_h

#include <Arduino.h>
#include <inttypes.h>
#include "Timer.h"

#define CALLBACK_NOT_INSTALLED TIMER_NOT_AN_EVENT

typedef struct ExecutorContext {
	Timer timer;
	int8_t buttonPin;
	int oldButtonState;
	boolean isExecuting;
	void (*sketchStartCallback)();
	void (*sketchStopCallback)();
	int8_t callbackReferences[MAX_NUMBER_OF_EVENTS];
};

class ButtonExecutor {

public:
	ButtonExecutor(void);
	
	void setup(int8_t buttonPin, void (*sketchSetupCallback)(),
		void (*sketchStartCallback)(), void (*sketchStopCallback)());
	void loop();
	int8_t callbackEvery(unsigned long period, void (*callback)(void*),
		void* context);
	int8_t callbackStop(int8_t threadId);
	void abortExecution();
	
protected:
	ExecutorContext _executorContext;
};

#endif
