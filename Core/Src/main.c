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
#include <stdio.h>

TimerHandle_t ledOffTimer = NULL;
void timerLedOffCb(TimerHandle_t xTimer);

int main(void)
{
  // Init HAL and hardware
  HAL_Init();
  button_init();
  led_init();
  
  // Create timer
  ledOffTimer = xTimerCreate("LedOff", pdMS_TO_TICKS(1000), pdFALSE, 0, timerLedOffCb);
  
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
    // Turn on LED
    led_on(GREEN_LED_PIN);
    // Start timer
    BaseType_t err = xTimerResetFromISR(ledOffTimer, &xHigherPriorityWoken);
    if (err != pdPASS)
    {
      printf("Failed to reset timer from isr\n");
    }
  }
}

void timerLedOffCb(TimerHandle_t xTimer)
{
  led_off(GREEN_LED_PIN);
}

/*

Step 1:
[ ] Design
  - Set up interrupt for button
  - A timer callback to turn off LED
[ ] LED driver
[ ] Button driver

*/
