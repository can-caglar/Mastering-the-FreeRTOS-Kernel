/*
Write a program that:
  Turns on an LED when a button is pressed.
  The LED remains on for 1 second.
  If the button is pressed, the time-out for the LED is increased by 1 second again.
  This is to emulate a backlight of an LCD which stays on and refreshes everytime a button is pressed.
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
void ledOffTask(void* pvParam);
void ledToggleTask(void* pvParam);
  
int main(void)
{
  // Init HAL and hardware
  HAL_Init();
  button_init();
  led_init();
  
  // Create timer
  ledOffTimer = xTimerCreate("LedOff", pdMS_TO_TICKS(1000), pdFALSE, 0, timerLedOffCb);
  
  // Create semaphore
  ledSem = xSemaphoreCreateCounting(3, 0);
  
  // Create task
  BaseType_t err = xTaskCreate(ledOffTask, "led off", 0x100, NULL, 2, NULL);
  err |= xTaskCreate(ledToggleTask, "led toggle", 0x100, NULL, 1, NULL);
  
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
  if (GPIO_Pin == GPIO_PIN_0) // Check if the interrupt was triggered by the button pin
  {
    printf("HAL_GPIO_EXTI_Callback - interrupted\n");
    xSemaphoreGiveFromISR(ledSem, &xHigherPriorityWoken);
    xSemaphoreGiveFromISR(ledSem, &xHigherPriorityWoken);
    xSemaphoreGiveFromISR(ledSem, &xHigherPriorityWoken);
    printf("HAL_GPIO_EXTI_Callback - gave semaphore\n");
    portYIELD_FROM_ISR(xHigherPriorityWoken);
  }
}

void timerLedOffCb(TimerHandle_t xTimer)
{
  // on orange, off green
  led_off(GREEN_LED_PIN);
  led_on(ORANGE_LED_PIN);
}

void ledOffTask(void* pvParam)
{
  printf("ledOffTask - Entering ledOffTask\n");
 
  while(1)
  {
    printf("ledOffTask - waiting for semaphore!\n");
    xSemaphoreTake(ledSem, portMAX_DELAY);
    printf("ledOffTask - ledSem taken!\n");
    // turn on green, off orange
    led_on(GREEN_LED_PIN);
    led_off(ORANGE_LED_PIN);
 
    // start led off timer
    BaseType_t err = xTimerReset(ledOffTimer, pdMS_TO_TICKS(500));
    if (err != pdPASS)
    {
      printf("ledOffTask - Failed to reset timer from isr\n");
    }
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
/*

Step 1:
[ ] Design
  - Set up interrupt for button
  - A timer callback to turn off LED
[ ] LED driver
[ ] Button driver

*/
