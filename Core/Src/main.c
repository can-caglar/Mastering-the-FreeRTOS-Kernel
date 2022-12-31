#include "FreeRTOS.h"
#include <stdio.h>
#include "task.h"
#include "event_groups.h"
#include "cc_button.h"
#include <stdlib.h>

/*
This example demonstrates syncing of three tasks.
It will print before and after the syncing happens to the 
console.
*/

EventGroupHandle_t xMyEventGroup = NULL;

const EventBits_t xTaskBit1 = (1 << 0);
const EventBits_t xTaskBit2 = (1 << 1);
const EventBits_t xTaskBit3 = (1 << 2);
const EventBits_t xAllBits = (xTaskBit1 | xTaskBit2 | xTaskBit3);

void vTaskSync(void* pvParam);

int main(void)
{
  // Create event group
  xMyEventGroup = xEventGroupCreate();
  
  // Create tasks with varying flag params
  xTaskCreate(vTaskSync, "vTaskSync1", 0x100, (void*)xTaskBit1, 1, NULL);
  xTaskCreate(vTaskSync, "vTaskSync2", 0x100, (void*)xTaskBit2, 1, NULL);
  xTaskCreate(vTaskSync, "vTaskSync3", 0x100, (void*)xTaskBit3, 1, NULL);
  
  // init hardware which will initialise interrupt
  button_init();
  
  vTaskStartScheduler();
  
  for(;;)
  {
    printf("shouldn't come here\n");
    while(1);
  }
}

void vTaskSync(void* pvParam)
{
  EventBits_t thisEventBits = (EventBits_t)pvParam;
  const TickType_t minTime = pdMS_TO_TICKS(500);
  const TickType_t range = pdMS_TO_TICKS(1000);
  
  for (;;)
  {
    // emulate task doing some operation
    TickType_t randDelay = (rand() % range) + minTime;
    vTaskDelay(randDelay);
    
    printf("Ready to sync (0x%x)\n", thisEventBits);
    EventBits_t bits = xEventGroupSync(xMyEventGroup, thisEventBits, xAllBits, portMAX_DELAY);
    printf("All synced up, leaving (0x%x)...\n", bits);
    vTaskDelay(pdMS_TO_TICKS(1000));
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
      
    }
  }
}
