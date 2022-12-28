// Include the appropriate header files
#include "stm32f4xx_hal.h"
#include "cc_button.h"

#define BUTTON_PORT GPIOA
#define BUTTON_PIN  GPIO_PIN_0

void button_init(void)
{
  // Enable GPIOA clock
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  // Declare a GPIO_InitTypeDef structure to configure the button pin
  GPIO_InitTypeDef button_pin;

  // Configure the button pin
  button_pin.Pin = GPIO_PIN_0; // Set the pin to be used
  button_pin.Mode = GPIO_MODE_IT_FALLING; // Set the pin to trigger an interrupt on a falling edge
  button_pin.Pull = GPIO_NOPULL;
  button_pin.Speed = GPIO_SPEED_FREQ_LOW; // Set the speed to low frequency

  // Initialize the button pin using the HAL_GPIO_Init() function
  HAL_GPIO_Init(GPIOA, &button_pin);
  
  // Enable and set up interrupt
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// Define the interrupt service routine (ISR) for the button input
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0) // Check if the interrupt was triggered by the button pin
    {
        // Read the state of the button using the HAL_GPIO_ReadPin() function
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
        {
          // not pressed
          volatile int a = 5;
        }
        else
        {
          // pressed
          volatile int a = 5;
        }
    }
}

void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
