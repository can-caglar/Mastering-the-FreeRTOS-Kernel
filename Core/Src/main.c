#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

// Tasks
void vTask1(void* vParam);
void vTaskSendToQueue(void* vParam);
void vTaskReceiveFromQueue(void* vParam);

// Generic Data
const int MyNumbers[] = {100, 200};
const char* const MyCharacters[] = {"hello", "my", "old", "friend"};

typedef enum TaskId_e
{
  TASK1_DATA,
  TASK2_DATA
} TaskId_e;

struct Data_s
{
  TaskId_e id;
  void* value;
} structData[2] =
{
  {TASK1_DATA, (void*)&MyNumbers[0]},
  {TASK2_DATA, (void*)&MyCharacters[0]},
};
typedef struct Data_s Data_s;


// FreeRTOS Objects

// Queues
xQueueHandle myQueue;

void Error_Handler()
{
  printf("error!\n");
  while(1)
  {
  }
}

int main(void)
{
  printf("Program starting...\n");
  
  // Create FreeRTOS objects
  myQueue = xQueueCreate(5, sizeof(Data_s));
  
  // Task creation
  xTaskCreate(vTaskSendToQueue, "TaskSend1", 0x100, &structData[0], 2, NULL);
  xTaskCreate(vTaskSendToQueue, "TaskSend2", 0x100, &structData[1], 2, NULL);
  xTaskCreate(vTaskReceiveFromQueue, "TaskRecv", 0x100, NULL, 1, NULL);
  
  // Begin scheduler
  vTaskStartScheduler();
  
  // should never get to here
  printf("Entering forever loop!\n");
  while(1)
  {
  }
}

void vTask1(void* vParam)
{
  char* str = (char*)vParam;
  while(1)
  {
    printf("This is task: %s\n", str);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void vTaskSendToQueue(void* vParam)
{
  printf("vTaskSendToQueue - Starting sending task...\n");
  
  while(1)
  {
    BaseType_t err = xQueueSend(myQueue, vParam, pdMS_TO_TICKS(500));
    if (err != errQUEUE_FULL)
    {
      printf("vTaskSendToQueue - Queue is full\n");
    }
  }
}

void vTaskReceiveFromQueue(void* vParam)
{
  printf("vTaskReceiveFromQueue - Starting...\n");
  while(1)
  {
    // Print info
    printf("vTaskReceiveFromQueue Items in Q = %lu\n", uxQueueMessagesWaiting(myQueue));
    
    // Receive from queue
    Data_s dataReceived;
    BaseType_t err = xQueueReceive(myQueue, &dataReceived, pdMS_TO_TICKS(500));
    
    // Error checking and logging
    if (err == pdPASS)
    {
      switch (dataReceived.id)
      {
        case TASK1_DATA:
        {
          printf("vTaskReceiveFromQueue - Received from TASK1, number: %d\n", (*(int*)dataReceived.value));
        }
        break;
        case TASK2_DATA:
        {
          printf("vTaskReceiveFromQueue - Received from TASK2, string: %s\n", (*(char**)dataReceived.value));
        }
        break;
        default:
        {
          printf("vTaskReceiveFromQueue - Unhandled ID\n");
        }
        break;
      }
      
    }
    else
    {
      printf("vTaskReceiveFromQueue - Queue empty and timed out\n");
    }
  }
}
