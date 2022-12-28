#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdio.h>
#include <stdlib.h>

// Creating a one-shot and auto reload timer
// but only having 1 timer callback
// and the auto-reload timer will stop after 5 executions.

// Create empty timer objects
TimerHandle_t osTimer;
TimerHandle_t arTimer;

void timerCallback1(TimerHandle_t xTimer);

int main(void)
{
  // Create the timers
  arTimer = xTimerCreate("Timer 1", pdMS_TO_TICKS(500), pdTRUE, 0, timerCallback1);
  osTimer = xTimerCreate("Timer 2", pdMS_TO_TICKS(1500), pdFALSE, 0, timerCallback1);
  
  // Start them
  volatile BaseType_t t1started = xTimerStart(arTimer, 0);
  volatile BaseType_t t2started = xTimerStart(osTimer, 0);
  
  if (t1started && t2started)
  {
    // give control over to scheduler
    vTaskStartScheduler();
  }
  else
  {
    printf("One of the timers couldnt be started t1 = %lu | t2 = %lu\n", t1started, t2started);
  }
  
  
  for(;;)
  {
  }
}

void timerCallback1(TimerHandle_t xTimer)
{
  // Get the ID associated with it
  int id = (int)pvTimerGetTimerID(xTimer);
  int newID = id + 1;
  vTimerSetTimerID(xTimer, (void*)newID);
  
  // Discern which timer it is
  if (xTimer == osTimer)
  {
    printf("One shot timer executing! ID: %d\n", id);
  }
  else
  {
    printf("Auto-reload timer executing! ID: %d\n", id);
    if (id > 5)
    {
      printf("Stopping auto-reload timer\n");
      xTimerStop(xTimer, pdMS_TO_TICKS(500));
    }
  }
}
