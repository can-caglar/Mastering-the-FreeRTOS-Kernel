#include "FreeRTOS.h"
#include <stdio.h>
#include "task.h"
#include "event_groups.h"
#include "cc_button.h"

/*
This example demonstrates how to:
- Create an event group.
- Set bits in an event group from an interrupt service routine.
- Set bits in an event group from a task.
- Block on an event group
*/

EventGroupHandle_t xMyEventGroup = NULL;

const EventBits_t buttonPress = (1 << 0);
const EventBits_t printReady = (1 << 1);

void vTaskAllowPrint(void* pvParam);
void vTaskPrint(void* pvParam);

int main(void)
{
  // An event group will be made.
  // 1 bit will signify a button press,
  // another bit will signify a task that will be
  // used to print to console
  
  // Create FreeRTOS structures
  xMyEventGroup = xEventGroupCreate();
  
  // Tasks... Daemon task is set to priority 2, so will preempt these
  xTaskCreate(vTaskAllowPrint, "Allow Print", 0x100, NULL, 1, NULL);
  xTaskCreate(vTaskPrint, "Print", 0x100, NULL, 1, NULL);
  
  // init hardware which will initialise interrupt
  button_init();
  
  vTaskStartScheduler();
  
  for(;;)
  {
    printf("shouldn't come here\n");
    while(1);
  }
}

void vTaskAllowPrint(void* pvParam)
{
  // signify that we are ready to print
  for (;;)
  {
    xEventGroupSetBits(xMyEventGroup, printReady);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void vTaskPrint(void* pvParam)
{
  const EventBits_t moneyEvent = (buttonPress | printReady);
  static uint8_t timesPrinted = 0;
  for (;;)
  {
    // print as soon as we're ready
    EventBits_t eventsNow = xEventGroupWaitBits(xMyEventGroup, (buttonPress | printReady), pdTRUE, pdTRUE, pdMS_TO_TICKS(1000));
    if ((eventsNow & moneyEvent) != moneyEvent)
    {
      printf("Timed out waiting for event groups. Flag now = 0x%X\n", eventsNow);
    }
    else
    {
      printf("vTaskPrint - Printing as both events (0x%x) arrived: %u\n", eventsNow, timesPrinted++);
    }
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_0)  // btn pressed
  {
    static TickType_t tickNow = 0;
    static TickType_t lastBtnPress = 0;
    
    tickNow = xTaskGetTickCountFromISR();
    
    if (tickNow > (lastBtnPress + pdMS_TO_TICKS(300)))  // debounce...
    {
      tickNow = lastBtnPress;
      
      // set event
      BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
      xEventGroupSetBitsFromISR(xMyEventGroup, buttonPress, &pxHigherPriorityTaskWoken);
      
      if (pxHigherPriorityTaskWoken == pdTRUE)
      {
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);  // yield to daemon task at the end of ISR call if possible
      }
    }
  }
}
