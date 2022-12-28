/*
Write a program that:
  Turns on an LED when a button is pressed.
  The LED remains on for 1 second.
  If the button is pressed, the time-out for the LED is increased by 1 second again.
  This is to emulate a backlight of an LCD which stays on and refreshes everytime a button is pressed.
*/

#include "cc_led.h"
#include "cc_button.h"

int main(void)
{
  HAL_Init();

  button_init();
  led_init();
 
  led_on(RED_LED_PIN);
  led_on(ORANGE_LED_PIN);
  led_on(GREEN_LED_PIN);
  led_on(BLUE_LED_PIN);
  
  while(1)
  {
    volatile int a = 6;
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
