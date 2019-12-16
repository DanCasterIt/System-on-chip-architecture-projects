/**
  ******************************************************************************
  * @file    DAC_SignalsGeneration/main.c 
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
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_lp_modes.h"
#include "waveform.h"
#include <stdio.h>

/** @addtogroup STM32F0_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup DAC_Signals_Generation
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DAC_DHR12R1_ADDRESS      0x40007408
#define DAC_DHR8R1_ADDRESS       0x40007410

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
DAC_InitTypeDef            DAC_InitStructure;
DMA_InitTypeDef            DMA_InitStructure;
USART_InitTypeDef					 USART_InitStructure;
GPIO_InitTypeDef					 GPIO_InitStructure;

const uint8_t Escalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};
uint8_t Idx = 0;
__IO uint8_t  SelectedWavesForm = 1;
__IO uint8_t WaveChange = 1; 

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
void DAC_Config(void);
void led_modes(uint8_t mode, uint8_t times);
void USART2_init(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	USART2_init();
	printf("RESET\n\r");
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */ 

  /* Preconfiguration before using DAC----------------------------------------*/
  DAC_Config();
  
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 0xFF;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
  
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
  
  /* Configures Button GPIO and EXTI Line */
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Infinite loop */
  while (1)
  {
    /* If the wave form is changed */
    if (WaveChange == 1)
    {  
      /* Switch the selected waves forms according the Button status */
      if (SelectedWavesForm == 1)
      {
					led_modes(3, 4);
					#ifdef SQUARE
          printf("SQUARE WAVEFORM\n\r");
					#else
          printf("SINEWAVE\n\r");
					#endif
          /* The sine wave has been selected */
          /* Sine Wave generator ---------------------------------------------*/
          DAC_DeInit(); 
          
          /* DAC channel1 Configuration */
          DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
          
          /* DMA channel3 Configuration */
          DMA_DeInit(DMA1_Channel3); 
          DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
					#ifdef SQUARE
          DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Square12bit;
					#else
          DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
					#endif
          DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
          DMA_InitStructure.DMA_BufferSize = 1024;
          DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
          DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
          DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
          DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
          DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
          DMA_InitStructure.DMA_Priority = DMA_Priority_High;
          DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
          DMA_Init(DMA1_Channel3, &DMA_InitStructure);

          /* Enable DMA1 Channel3 */
          DMA_Cmd(DMA1_Channel3, ENABLE);

          /* DAC Channel1 Init */
          DAC_Init(DAC_Channel_1, &DAC_InitStructure);

          /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
             automatically connected to the DAC converter. */
          DAC_Cmd(DAC_Channel_1, ENABLE);

          /* Enable DMA for DAC Channel1 */
          DAC_DMACmd(DAC_Channel_1, ENABLE);
         
      }
          /* The Escalator wave has been selected */
       else
       {
					led_modes(2, 4);
				  printf("ESCALATOR\n\r");
         
          /* Escalator Wave generator -----------------------------------------*/
          DAC_DeInit();
          
          /* DAC channel1 Configuration */
          DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
          
  
          /* DMA1 channel2 configuration */
          DMA_DeInit(DMA1_Channel3);

          DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_ADDRESS;
          DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Escalator8bit;
          DMA_InitStructure.DMA_BufferSize = 6;
          DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
          DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
          DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    
          /* Enable DMA1 Channel2 */
          DMA_Cmd(DMA1_Channel3, ENABLE);
    
          /* DAC channel1 Configuration */
          DAC_Init(DAC_Channel_1, &DAC_InitStructure);

          /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
             automatically connected to the DAC converter. */
          DAC_Cmd(DAC_Channel_1, ENABLE);

          /* Enable DMA for DAC Channel1 */
          DAC_DMACmd(DAC_Channel_1, ENABLE);
      }
      WaveChange = !WaveChange;
    }
		else
		{
			printf("Entered in StopMode\n\r");
			StopMode_Measure();
			USART2_init();
			printf("Exited from StopMode\n\r");
		}
  }
}


/**
  * @brief  PrecConfiguration: configure PA4 in analog,
  *                           enable DAC clock, enable DMA1 clock
  * @param  None
  * @retval None
  */
void DAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* DMA1 clock enable (to be used with DAC) */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* Configure PA.04 (DAC_OUT1) as analog */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void led_modes(uint8_t mode, uint8_t times)	{
	uint8_t a;
	uint32_t i;
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

void USART2_init(void)	{
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
