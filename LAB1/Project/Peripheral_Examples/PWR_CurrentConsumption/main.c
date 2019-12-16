/**
  ******************************************************************************
  * @file    PWR_CurrentConsumption/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f0_discovery.h"
#include "stm32f0xx_lp_modes.h"
#include "stm32f0xx_usart.h"
#include <stdio.h>

/** @addtogroup STM32F0_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup PWR_CurrentConsumption
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef					 USART_InitStructure;
GPIO_InitTypeDef					 GPIO_InitStructure;
__IO uint32_t i = 0;
/* Private function prototypes -----------------------------------------------*/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_getchar(int ch)
#else
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
	
/* Private functions ---------------------------------------------------------*/
void led_modes(uint8_t mode, uint8_t times);
void UART2_init(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	UART2_init();
	printf("RESET\n\r");
	printf("Press the user button to enter in low power states.\n\r");
  /*!< At this stage the microcontroller clock setting is already configured, 
    this is done through SystemInit() function which is called from startup
    file (startup_stm32f0xx.s) before to branch to application main.
    To reconfigure the default setting of SystemInit() function, refer to
    system_stm32f0xx.c file
    */
    /* Configure User Button */
    STM_EVAL_PBInit(BUTTON_USER,BUTTON_MODE_GPIO);
    
    /* Loop while User button is maintained pressed */
    while(STM_EVAL_PBGetState(BUTTON_USER) != RESET)
    {
    }
    
    /* Enable PWR APB1 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    
    /* Allow access to Backup */
    PWR_BackupAccessCmd(ENABLE);
    
    /* Reset RTC Domain */
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);
    
    /* Loop while User button is maintained pressed */
    while(STM_EVAL_PBGetState(BUTTON_USER) == RESET)
    {
    }
    /* Loop while User button is maintained pressed */
    while(STM_EVAL_PBGetState(BUTTON_USER) != RESET)
    {
    }
	printf("STOP Mode Entry.\n\r");
  led_modes(3, 4);
  /* STOP Mode Entry 
      - RTC Clocked by LSI
      - Regulator in LP mode
      - HSI, HSE OFF and LSI OFF if not used as RTC Clock source  
      - No IWDG
      - FLASH in deep power down mode
      - Automatic Wakeup using RTC clocked by LSI 
   */
  StopMode_Measure();
		
	UART2_init();
	printf("Sleep Mode Entry.\n\r");
  led_modes(2, 4);
  /* Sleep Mode Entry 
      - System Running at PLL (48MHz)
      - Flash 3 wait state
      - Prefetch and Cache enabled
      - Code running from Internal FLASH
      - All peripherals disabled.
      - Wakeup using EXTI Line (User Button PA.00)
   */  
  SleepMode_Measure();
		
	UART2_init();
	printf("STANDBY Mode Entry.\n\r");
  led_modes(0, 4);
  /* STANDBY Mode Entry 
      - RTC OFF
      - IWDG and LSI OFF
      - Wakeup using WakeUp Pin (PA.0)
   */
  StandbyMode_Measure(); //here it blocks and resets if user button pressed.
	
	/*following code will never reached:*/
	UART2_init();
	printf("Exiting all of the modalities.\n\r");
  led_modes(1, 4);
	while(1);
}

void led_modes(uint8_t mode, uint8_t times)	{
	uint8_t a;
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDOff(LED3);
	STM_EVAL_LEDOff(LED4);
	switch(mode)	{
		case 0:
			for(a = 0; a < times; a++)	{
				STM_EVAL_LEDToggle(LED3);
				for(i = 0; i < 0x7FFFF; i++);
				STM_EVAL_LEDToggle(LED3);
				for(i = 0; i < 0x7FFFF; i++);
			}
			break;
		case 1:
			for(a = 0; a < times; a++)	{
				STM_EVAL_LEDToggle(LED4);
				for(i = 0; i < 0x7FFFF; i++);
				STM_EVAL_LEDToggle(LED4);
				for(i = 0; i < 0x7FFFF; i++);
			}
			break;
		case 2:
			for(a = 0; a < times; a++)	{
				STM_EVAL_LEDToggle(LED3);
				STM_EVAL_LEDToggle(LED4);
				for(i = 0; i < 0x7FFFF; i++);
				STM_EVAL_LEDToggle(LED3);
				STM_EVAL_LEDToggle(LED4);
				for(i = 0; i < 0x7FFFF; i++);
			}
			break;
		case 3:
				STM_EVAL_LEDToggle(LED3);
			for(a = 0; a < times; a++)	{
				STM_EVAL_LEDToggle(LED3);
				STM_EVAL_LEDToggle(LED4);
				for(i = 0; i < 0x7FFFF; i++);
				STM_EVAL_LEDToggle(LED3);
				STM_EVAL_LEDToggle(LED4);
				for(i = 0; i < 0x7FFFF; i++);
			}
			break;
	}
	STM_EVAL_LEDOff(LED3);
	STM_EVAL_LEDOff(LED4);
}

void UART2_init(void)	{
	// USART periferial initialization settings 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
  //Configure USART2 pins: Rx (PA2) and Tx (PA3)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //Configure USART2 setting: ----------------------------
  USART_InitStructure.USART_BaudRate = 19200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2,ENABLE);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* fputc implementation: */
  /* write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);
  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
  {}
  return ch;
}

GETCHAR_PROTOTYPE
{
	FILE *fd;
	char ch;
  /* fgetc implementation: */
  /* Loop until RXNE = 1 */
  while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
  {}
  /* read a character to the USART */
	ch = USART_ReceiveData(USART2);
	if(ch == '\r' || ch == '\n')	printf("\n\r");
	else fputc(ch, fd);
  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
