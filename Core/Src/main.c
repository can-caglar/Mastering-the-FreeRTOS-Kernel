/*
Write a program that:
  Blinks blue LED periodically.
  
  When a button is pressed, turns on green LED which will turn off after 500 ms.
  Every time a button is pressed, the green LED off period will increase.
  
  The interrupt shall defer to the daemon task.
*/

#include "cc_led.h"
#include "cc_button.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include <stdio.h>

// Objects
TimerHandle_t ledOffTimer = NULL;
SemaphoreHandle_t ledSem = NULL;

// Callbacks and tasks
void timerLedOffCb(TimerHandle_t xTimer);
void ledOffPendableFunction(void* pvParam, uint32_t ulParam);
void ledToggleTask(void* pvParam);
  
int main(void)
{
  // Init HAL and hardware
  HAL_Init();
  button_init();
  led_init();
  
  // Create timer
  ledOffTimer = xTimerCreate("LedOff", pdMS_TO_TICKS(1000), pdFALSE, 0, timerLedOffCb);
  
  // Create task
  BaseType_t err = xTaskCreate(ledToggleTask, "led toggle", 0x100, NULL, 1, NULL);
  
  if (err != pdPASS)
  {
    printf("One of the tasks could not be created\n");
  }
  
  vTaskStartScheduler();
  
  while(1)
  {
    volatile int a = 6;
  }
}

// Define the interrupt service routine (ISR) for the button input
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  BaseType_t xHigherPriorityWoken = pdFALSE;
  static uint32_t callTime = 0;
  if (GPIO_Pin == GPIO_PIN_0) // Check if the interrupt was triggered by the button pin
  {
    printf("HAL_GPIO_EXTI_Callback - interrupted\n");
    // send command execution to the Daemon task
    xTimerPendFunctionCallFromISR(ledOffPendableFunction, NULL, callTime++, &xHigherPriorityWoken);
    printf("HAL_GPIO_EXTI_Callback - pend function\n");
    portYIELD_FROM_ISR(xHigherPriorityWoken);
  }
}

void timerLedOffCb(TimerHandle_t xTimer)
{
  // on orange, off green
  led_off(GREEN_LED_PIN);
  led_on(ORANGE_LED_PIN);
}

void ledOffPendableFunction(void* pvParam, uint32_t ulParam)
{
  printf("ledOffPendableFunction - Entered %u times\n", ulParam);
 
  // turn on green, off orange
  led_on(GREEN_LED_PIN);
  led_off(ORANGE_LED_PIN);

  // start led off timer
  BaseType_t err = xTimerReset(ledOffTimer, pdMS_TO_TICKS(500));
  if (err != pdPASS)
  {
    printf("ledOffPendableFunction - Failed to reset timer\n");
  }
}

void ledToggleTask(void* pvParam)
{
  TickType_t thisTick = xTaskGetTickCount();
  while(1)
  {
    led_toggle(BLUE_LED_PIN);
    printf("ledToggleTask - toggled\n");
    vTaskDelayUntil(&thisTick, pdMS_TO_TICKS(150));
  }
}
