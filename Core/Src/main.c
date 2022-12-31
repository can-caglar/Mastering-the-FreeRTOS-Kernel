#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <stdio.h>

/*
Demonstrate how a semaphore unblocks only the highest priority task
that is waiting on it
*/

void vTaskPrint(void* pvParam);  
void vTaskUnblockSemaphore(void* pvParam);  

SemaphoreHandle_t xMySemaphore = NULL;

const char* Strings[2] =
{
  "HighPriorityTask",
  "LowPriorityTask",
};

int main(void)
{
  // Create FreeRTOS objects
  xTaskCreate(vTaskUnblockSemaphore, "Task semph", 0x100, NULL, 2, NULL);
  xTaskCreate(vTaskPrint, "Task HP", 0x100, (void*)Strings[0], 2, NULL);
  xTaskCreate(vTaskPrint, "Task LP", 0x100, (void*)Strings[1], 1, NULL);
  
  xMySemaphore = xSemaphoreCreateBinary();

  if (xMySemaphore  == NULL)
  {
    printf("xMySemaphore could not be created!\n");
  }
  
  // Start scheduler
  vTaskStartScheduler();
  
  for (;;)
  {
    // should never come here
    printf("Shouldn't come here!");
    while(1);
  }
}

void vTaskPrint(void* pvParam)
{
  // grab string to print to identify task
  char* printStr = (char*)pvParam;

  for (;;)
  {
    // Attempt to take semaphore, report if taken or timed out
    if (xSemaphoreTake(xMySemaphore, pdMS_TO_TICKS(500)))
    {
      printf("%s: SUCCESSFULLY grabbed semaphore\n", printStr);
    }
    else
    {
      printf("%s: FAILED to grab semaphore in time\n", printStr);
    }
    // Block 200 ms to go once again afterwards
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void vTaskUnblockSemaphore(void* pvParam)
{
  for (;;)
  {
    // wait 500 ms and give semaphore each loop
    vTaskDelay(pdMS_TO_TICKS(2000));
    xSemaphoreGive(xMySemaphore);
  }
}

