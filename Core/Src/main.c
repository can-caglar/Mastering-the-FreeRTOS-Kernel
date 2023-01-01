#include "FreeRTOS.h"
#include <stdio.h>
#include "task.h"
#include "event_groups.h"
#include "cc_button.h"
#include <stdlib.h>

/*
This example demonstrates the use of task notifications
for an ISR to notify a task directly.
*/

TaskHandle_t xTaskPrint = NULL;
void vTaskPrint(void* pvParam);

int main(void)
{
  // init button with interrupt
  button_init();  
  
  // create task
  xTaskCreate(vTaskPrint, "Print", 0x100, NULL, 1, &xTaskPrint);
  
  // run scheduler
  vTaskStartScheduler();
  
  for (;;)
  {
      //should never come here
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_0) // user btn
  {
    static TickType_t tickNow = 0;
    static TickType_t lastBtnPress = 0;
    
    tickNow = xTaskGetTickCountFromISR();
    
    if (tickNow > (lastBtnPress + pdMS_TO_TICKS(100)))  // debounce...
    {
      // btn press registered
      BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
      
      vTaskNotifyGiveFromISR(xTaskPrint, &pxHigherPriorityTaskWoken);  // notify print task directly
      
      portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);  // switch to highest prio task right away
      
      lastBtnPress = tickNow; // update last btn press tick count
    }
  }
}

void vTaskPrint(void* pvParam)
{
  for (;;)
  {
    uint32_t taskNotificationValue = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10000));
    if (taskNotificationValue > 0)
    {
      printf("Button pressed (notif value = %u)!\n", taskNotificationValue);
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
    else
    {
      printf("Button press not detected in expected time. Trying again...\n");
    }
  }
}
