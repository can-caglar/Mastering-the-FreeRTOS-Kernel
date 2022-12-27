#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// One task that continuously reads a mail box
// and signifies when it detects new data
// Called at a faster frequency

// Another task that continuously overwrites data in mailbox
// with data

// Tasks
void readMailbox(void* pvParam);
void writeMailBox(void* pvParam);

// Mailbox
xQueueHandle Mailbox;

// The data will be a struct with a random number and time stamp
typedef struct
{
  uint64_t randomNumber;
  UBaseType_t time;
} MyData_s;

MyData_s DataToSend;

int main(void)
{
  // Create mailbox (queue)
  Mailbox = xQueueCreate(1, sizeof(MyData_s));
  
  // Create tasks
  xTaskCreate(readMailbox, "read", 0x100, NULL, 1, NULL);
  xTaskCreate(writeMailBox, "write", 0x100, NULL, 1, NULL);
  
  // give control over to scheduler
  vTaskStartScheduler();
  
  for(;;)
  {
  }
}

void readMailbox(void* pvParam)
{
  // read from mailbox and announce when new item detected!
  MyData_s receivedData = { 0 };
  BaseType_t previousTime = 0;
  while(1)
  {
    // Read from mailbox (peek)
    BaseType_t err = xQueuePeek(Mailbox, &receivedData, pdMS_TO_TICKS(100));
    if (err == errQUEUE_EMPTY)
    {
      printf("Mailbox is empty...\n");
    }
    else
    {
      if (previousTime < receivedData.time)
      {
        printf("New item received! Item: %llu\n", receivedData.randomNumber); 
        previousTime = receivedData.time;
      }
      else
      {
        printf("Item: %llu\n", receivedData.randomNumber);
      }
    }
    
    // Delay
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void writeMailBox(void* pvParam)
{
  // write to mailbox every so often
  while(1)
  {
    // prep data
    DataToSend.randomNumber = rand();
    DataToSend.time = xTaskGetTickCount();
    
    // send
    xQueueOverwrite(Mailbox, (void*)&DataToSend);
    
    // delay
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
