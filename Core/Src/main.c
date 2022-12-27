#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdio.h>
#include <stdlib.h>

// Creating a one-shot and auto reload timer

// Create empty timer objects
TimerHandle_t osTimer;
TimerHandle_t arTimer;

void timerCallback1(TimerHandle_t xTimer);
void timerCallback2(TimerHandle_t xTimer);

int main(void)
{
  // Create the timers
  arTimer = xTimerCreate("Timer 1", pdMS_TO_TICKS(500), 1, NULL, timerCallback1);
  osTimer = xTimerCreate("Timer 2", pdMS_TO_TICKS(1500), 0, NULL, timerCallback2);
  
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
  printf("Timer callback1 is being called\n");
}

void timerCallback2(TimerHandle_t xTimer)
{
  printf("Gundi! The callback2 is being called\n");
}
