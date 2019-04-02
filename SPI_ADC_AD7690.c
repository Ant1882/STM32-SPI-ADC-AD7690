/*! \file SPI_ADC_AD7690.c
    \brief Source for SPI ADC - STM32 Based
    \author Anthony Marshall.
    
    Implemented as !CS Mode, 4-wire without busy indicator, pg.19 AD760 Datasheet Rev.C.
*/

#include "SPI_ADC_AD7690.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "spi.h"

static S_ADC_READINGS adc_values;

// Internal functions
static void AD7690_Conversion_Delay(void);
static void AD7690_GetValues_SPI(void);

// Public Functions --------------------------------------------------

// Setup ADC control lines
void AD7690_Initialise_GPIO(void)
{
   HAL_GPIO_WritePin(AD7690_CNV_GPIO_PORT, AD7690_CNV_PIN, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(AD7690_CS1_GPIO_PORT, AD7690_CS1_PIN, GPIO_PIN_SET);
   HAL_GPIO_WritePin(AD7690_CS2_GPIO_PORT, AD7690_CS2_PIN, GPIO_PIN_SET);
}

// Return values from both of the ADC's
S_ADC_READINGS AD7690_GetValues(void)
{
  AD7690_GetValues_SPI();
  
  return adc_values;
}

// Private Functions --------------------------------------------------

// Delay for at least max possible conversion time (2.1us)
static void AD7690_Conversion_Delay(void)
{
  // 16 MHz CPU, 40*(1/16*10^6) = 2.5us
  for(int i=0; i<40; i++)
  {
    asm("nop");
  }
}

// SPI Functions --------------------------------------------------
static void AD7690_Dev_Select( E_ADC adc )
{
  if(adc == E_ADC1)
  {
    HAL_GPIO_WritePin(AD7690_CS1_GPIO_PORT, AD7690_CS1_PIN, GPIO_PIN_RESET);
  }
  else if(adc == E_ADC2)
  {
    HAL_GPIO_WritePin(AD7690_CS2_GPIO_PORT, AD7690_CS2_PIN, GPIO_PIN_RESET);
  }
}

static void AD7690_Dev_Deselect( E_ADC adc )
{
   if(adc == E_ADC1)
  {
    HAL_GPIO_WritePin(AD7690_CS1_GPIO_PORT, AD7690_CS1_PIN, GPIO_PIN_SET);
  }
  else if(adc == E_ADC2)
  {
    HAL_GPIO_WritePin(AD7690_CS2_GPIO_PORT, AD7690_CS2_PIN, GPIO_PIN_SET);
  }
}

// - Replace with your own setup
void AD7690_Initialise_SPI(void)
{   
  /* Create control block for SPI and enable bus
  SPI_ADC_dev = SPI_setup (SPI_ADC_AD7690,
                           SPI_ADC_AD7690_BAUD,
                           SPI_master,
                           SPI_inactive_low,
                           SPI_ck_leading,
                           SPI_8Bit,
                           SPI_ADC_AD7690_CS_PORT,
                           SPI_ADC_AD7690_CS_PIN);
   */
}

static void AD7690_GetValues_SPI(void)
{ 
  uint8_t adc1_buff[3] = {0,0,0};
  uint32_t adc1_val = 0;
  
  uint8_t adc2_buff[3] = {0,0,0};
  uint32_t adc2_val = 0;
  
  // Trigger a conversion (CNV High)
  HAL_GPIO_WritePin(AD7690_CNV_GPIO_PORT, AD7690_CNV_PIN, GPIO_PIN_SET);
  
  // Wait 2.1us (Max conversion time)
  AD7690_Conversion_Delay();
  
  // Clock out the 18 bits for ADC1
  //--------------------------------------
  AD7690_Dev_Select(E_ADC1); 
  HAL_SPI_Receive(&hspi2, &adc1_buff[0], 1, 100);
  HAL_SPI_Receive(&hspi2, &adc1_buff[1], 1, 100);
  HAL_SPI_Receive(&hspi2, &adc1_buff[2], 1, 100);
  AD7690_Dev_Deselect(E_ADC1);
  //--------------------------------------
  adc1_val = (adc1_buff[0] << 10) | (adc1_buff[1] << 2) | (adc1_buff[2] & 0x03); 
  
  // Clock out the 18 bits for ADC2
  //--------------------------------------
  AD7690_Dev_Select(E_ADC2);
  HAL_SPI_Receive(&hspi2, &adc2_buff[0], 1, 100);
  HAL_SPI_Receive(&hspi2, &adc2_buff[1], 1, 100);
  HAL_SPI_Receive(&hspi2, &adc2_buff[2], 1, 100); 
  AD7690_Dev_Deselect(E_ADC2);
  //--------------------------------------
  adc2_val = (adc2_buff[0] << 10) | (adc2_buff[1] << 2) | (adc2_buff[2] & 0x03);
  

  // Reset CNV line
  HAL_GPIO_WritePin(AD7690_CNV_GPIO_PORT, AD7690_CNV_PIN, GPIO_PIN_RESET);
  
  adc_values.adc1 = adc1_val;
  adc_values.adc2 = adc2_val;
}
