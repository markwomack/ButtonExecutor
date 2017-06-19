/**
 * Code written by Mark Womack
 * Distributed under the Apache License 2.0, a copy of which should accompany
 * this file.
 *
 * This library provides code that will monitor a push button on an Arduino
 * microcontroller. When the button is pushed, registered callbacks will be
 * called to start and stop the execution of code. Other callbacks can also be
 * registered that will only be executed when the program is started. Setup and
 * usage is very simple. Please see the examples for guidance on how to use in
 * your own program.
 *
 * This library is dependent on Simon Monk's Timer library which at the time of
 * this writing can be found at https://github.com/JChristensen/Timer. It will
 * need to be installed as an Arduino library in your Arduino development
 * environment for this libary to correctly compile.
 *
 */

#ifndef BUTTON_EXECUTOR_H
#define BUTTON_EXECUTOR_H

#include <Arduino.h>
#include <inttypes.h>
#include <Print.h>
#include "Timer.h"

#define CALLBACK_STOPPED (1);
#define CALLBACK_NOT_INSTALLED (TIMER_NOT_AN_EVENT);

class ButtonExecutor {

public:
  ButtonExecutor();
  ButtonExecutor(Print* serialOutput);

  /**
   * Called to set everything up. When called the sketchSetupCallback method
   * will be called. Any required setup should be performed in that method.
   * After it returns, the buttonPin will be actively monitored for any button
   * activity. Normally this method would be called from the Arduino setup
   * method.
   *
   * When the button is pushed, the sketchStartCallback method will be called.
   * That method should perform any initialization required and register all
   * callbacks that should be executed until the button is pushed again. After
   * it returns, then the registered callbacks will be called as registered (see
   * loop method).
   *
   * When the buttons is pushed again, all of the callbacks registered in the
   * sketchStartCallback (or after the button was first pushed) will be stopped.
   * Then the sketchStopCallback method will be called. That method should
   * perform an cleanup required. After it returns, the callbacks will be
   * stopped and unregistered. The program state should be ready for the next
   * button push to start again.
   *
   * When completed, the button can be pushed again, and the cycle will repeat.
   *
   * buttonPin - Pin number to monitor for push button activity.
   * sketchSetupCallback - Callback method that is executed only once to setup
   *   the code for execution.
   * sketchStartCallback - Callback method that is executed whenever the button
   *   is pushed to start execution.
   * sketchStopCallback - Callback method that is executed whenever the button
   *   is pushed to stop execution.
   */
  void setup(int8_t buttonPin, int8_t expectedButtonPressState,
    void (*sketchSetupCallback)(void),
    void (*sketchStartCallback)(void),
    void (*sketchStopCallback)(void));
    
  /**
   * Periodically call this method after setup to monitor button activity and to
   * start and stop the execution of the program. Normally this method would be
   * called from the Arduino loop method.
   */
  void loop();

  /**
   * Call this method to register callbacks that should be executed after the
   * button is pushed. Normally called from the sketchStartCallback method
   * registered in the ButtonExecutor.setup method. However, it can be called
   * at any time after as well.
   *
   * The callback will be called periodically, as given by the periodInMs
   * parameter. The callback will be called every periodInMs milliseconds.
   * Setting a very small value, the callback may not be executed in the time
   * frame expected since other code execution may delay.
   *
   * Any method registered will need to be re-registered after the button is
   * pushed to stop execution. Callback registration is not maintained between
   * starts and stops of execution.
   *
   * periodInMs - Period of time, in milliseconds, to execute the callback.
   * callback - Callback method that should be executed.
   * Returns a reference to the registered callback that can be used in a
   * subsequent call to ButtonExecutor.stopCallback to stop the execution of the
   * callback before the button is pushed to stop execution. Can also return
   * CALLBACK_NOT_INSTALLED if the callback could not be installed due to the
   * maximum number of callbacks already registered.
   */
  int8_t callbackEveryByMillis(unsigned long periodInMs,
    void (*callback)(void));
  
  /**
   * Call this method to register callbacks that should be executed after the
   * button is pushed. Normally called from the sketchStartCallback method
   * registered in the ButtonExecutor.setup method. However, it can be called
   * at any time after as well.
   *
   * Same as the callbackEveryByMillis method, except period is given in hertz
   * by the periodInHz parameter. For example a value of 3 means the callback
   * will be executed 3 times a second. A value of 100 would execute the
   * callback 100 times a second. Setting a very large value means a short
   * period between calls, so the callback may not be executed in the time
   * frame expected since other code execution may delay.
   *
   * Any method registered will need to be re-registered after the button is
   * pushed to stop execution. Callback registration is not maintained between
   * starts and stops of execution.
   *
   * periodinHz - Period in hertz, number of times per second to execute the
   *   callback.
   * callback - Callback method that should be executed.
   * Returns a reference to the registered callback that can be used in a
   *   subsequent call to ButtonExecutor.stopCallback to stop the execution of
   *   the callback before the button is pushed to stop execution. Can also
   *   return CALLBACK_NOT_INSTALLED if the callback could not be installed due
   *   to the maximum number of callbacks already registered.
   */
  int8_t callbackEveryByHertz(unsigned long periodinHz, void (*callback)(void));
  
  /**
   * Call this method to stop the execution of a previously registered callback.
   * 
   * callbackId - A reference to the callback returned by the
   * ButtonExecutor.callbackEvery method.
   * Returns either CALLBACK_STOPPED if the callback was stopped or
   * CALLBACK_NOT_INSTALLED is the callbackId does not match any registered
   * callbacks.
   */
  int8_t stopCallback(int8_t callbackId);
  
  /**
   * Call at any time to abort any current execution. This is the code
   * equivalent of pushing the button to stop execution.
   */
  void abortExecution();
};

#endif
