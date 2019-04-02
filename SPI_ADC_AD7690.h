/*! \file  SPI_ADC_AD7690.h
    \brief Header for SPI ADC
    \author Anthony Marshall

    Assumes 2 ADC's being used
*/
#ifndef _SPI_ADC_H
#define _SPI_ADC_H

#include <stdint.h>
#include <stdbool.h>

// Pin defines - Change to suit
#define AD7690_CNV_GPIO_PORT    GPIOA
#define AD7690_CNV_PIN          GPIO_PIN_1
#define AD7690_CS1_GPIO_PORT    GPIOA
#define AD7690_CS1_PIN          GPIO_PIN_1
#define AD7690_CS2_GPIO_PORT    GPIOA
#define AD7690_CS2_PIN          GPIO_PIN_1

typedef struct
{
  uint32_t adc1;
  uint32_t adc2;
} S_ADC_READINGS, *P_ADC_READINGS;

typedef enum
{
  E_ADC1,
  E_ADC2
} E_ADC;

S_ADC_READINGS AD7690_GetValues(void);

void AD7690_Initialise_SPI(void);
void AD7690_Initialise_GPIO(void);

#endif
