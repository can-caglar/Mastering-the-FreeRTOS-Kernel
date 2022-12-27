#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

/*
This example creates two sending tasks and one receiving task. The sending tasks send data
to the receiving task on two separate queues, one queue for each task. The two queues are
added to a queue set, and the receiving task reads from the queue set to determine which of
the two queues contain data.
The tasks, queues, and the queue set, are all created in main()—see Listing 63 for its
implementation.
*/

float myfloat = 1.994f;

// Tasks
void sendingTask1(void* vParam);
void sendingTask2(void* vParam);
void receivingTask(void* vParam);

xQueueHandle queue1 = NULL;
xQueueHandle queue2 = NULL;
xQueueSetHandle queueSet = NULL;

int main(void)
{
  // Create a queue
  queue1 = xQueueCreate(5, sizeof(int));
  queue2 = xQueueCreate(5, sizeof(float));
  
  // Create queue set
  queueSet = xQueueCreateSet(10);
  
  // Add queue to set
  xQueueAddToSet(queue1, queueSet);
  xQueueAddToSet(queue2, queueSet);
  
  // Create tasks
  xTaskCreate(sendingTask1, "Sending task 1", 0x100, (void*)777, 2, NULL);
  xTaskCreate(sendingTask2, "Sending task 2", 0x100, (void*)&myfloat, 2, NULL);
  xTaskCreate(receivingTask, "Recv task", 0x100, NULL, 1, NULL);
  
  // Give control over to scheduler
  vTaskStartScheduler();
  
  while(1)
  {
    printf("Should never reach here!\n");
  }
}


// Sends INT data to a queue set
void sendingTask1(void* vParam)
{
  int numberToSend = (int)vParam;  // no need to cast
  while(1)
  {
    xQueueSendToBack(queue1, (void*)&numberToSend, pdMS_TO_TICKS(500));
    vTaskDelay(1000);
  }
}

// Sends FLOAT data to a queue set
void sendingTask2(void* vParam)
{
  float numberToSend = (*(float*)vParam);
  while(1)
  {
    xQueueSendToBack(queue2, (void*)&numberToSend, pdMS_TO_TICKS(500));
    vTaskDelay(500);
  }
}

void receivingTask(void* vParam)
{
  while(1)
  {
    // init data to be received
    int numberReceived = 0;
    float floatReceived = 0.0f;
    
    // Receive data from set
    QueueSetMemberHandle_t member = xQueueSelectFromSet(queueSet, pdMS_TO_TICKS(500));
    
    // Parse set
    if (member == (QueueSetMemberHandle_t)queue1)
    {
      // received from queue 1. it's known to be available, so a block time of 0 can be used.
      xQueueReceive(queue1, (void*)&numberReceived, 0);
      printf("Received from queue 1: %d\n", numberReceived);
    }
    if (member == (QueueSetMemberHandle_t)queue2)
    {
      // received from queue 2. it's known to be available, so a block time of 0 can be used.
      xQueueReceive(queue2, (void*)&floatReceived, 0);
      printf("Received from queue 2: %f\n", floatReceived);
    }
  }
}