#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <stdio.h>

/*
Demonstrate a gatekeeper task pattern
  - Uses a queue
This task will be the only owner of the resource "printf"
Let other tasks use the gatekeeper to print.
Use a tick hook function that will print out every 200 ticks something
*/

// Strings to print
const char* MyStrings[] =
{
  "Task 1:    ************************",
  "Task Tick: ========================",
};


void vTaskPrint1(void* pvParam);  // prints every 100 ms
void vApplicationTickHook(void);  // will print every FreeRTOS scheduler tick cycle

void vPrintGateKeeper(void* pvParam); // the printing gatekeeper task

QueueHandle_t PrintQue; // queue to hold print values

int main(void)
{
  // Create FreeRTOS objects
  xTaskCreate(vTaskPrint1, "Print Task 1", 0x100, NULL, 2, NULL);
  xTaskCreate(vPrintGateKeeper, "Print Gatekeeper", 0x100, NULL, 3, NULL);
  
  PrintQue = xQueueCreate(10, sizeof(char*));
  if (PrintQue == NULL)
  {
    printf("PrintQue could not be created!\n");
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

void vTaskPrint1(void* pvParam)
{
  // send to print queue every 100 ms
  for (;;)
  {
    // send to queue
    BaseType_t err = xQueueSendToBack(PrintQue, &MyStrings[0], pdMS_TO_TICKS(500));
    if (err == errQUEUE_FULL)
    {
      // would like to print but print que is full. just debug via breakpoints for now
      volatile int a = 5;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void vPrintGateKeeper(void* pvParam)
{
  char* printMsg = NULL;
  for (;;)
  {
    BaseType_t err = xQueueReceive(PrintQue, &printMsg, pdMS_TO_TICKS(1000));
    if (err == errQUEUE_EMPTY)
    {
      printf("vPrintGateKeeper - nothing to print!");
    }
    else
    {
      printf("%s\n", printMsg);
    }
  }
}

void vApplicationTickHook(void)
{
  // Every 200 ticks, print immediately by sending to front.
  // Note, this will be called from ISR, so use appropriate functions
  static uint8_t tickCount = 0;
  
  if (tickCount > 200)
  {
    // pxHigherPriorityTaskWoken has no effect as Scheduler will be called straight after, so set it to NULL.
    BaseType_t err = xQueueSendToFrontFromISR(PrintQue, &MyStrings[1], NULL);
    if (err == errQUEUE_FULL)
    {
      // would like to print but print que is full. just debug via breakpoints for now
      volatile int a = 5;
    }
    tickCount = 0;
  }
  
  tickCount++;
}
