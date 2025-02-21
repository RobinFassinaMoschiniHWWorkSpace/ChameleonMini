/*
 * AntennaLevel.h
 *
 *  Created on: 24.11.2013
 *      Author: skuser
 */

#ifndef ANTENNALEVEL_H_
#define ANTENNALEVEL_H_

#include "Common.h"

#define ANTENNA_LEVEL_R1	47E3
#define ANTENNA_LEVEL_R2	1E3
#define ANTENNA_LEVEL_VREF	1.0
#define ANTENNA_LEVEL_RES	4096
#define ANTENNA_LEVEL_OFFSET	190 /* LSB */

#define ANTENNA_LEVEL_MILLIVOLT 1E3
#define ANTENNA_LEVEL_FACTOR	(ANTENNA_LEVEL_VREF * (ANTENNA_LEVEL_R1 + ANTENNA_LEVEL_R2) / (ANTENNA_LEVEL_RES * ANTENNA_LEVEL_R2) )
#define ANTENNA_LEVEL_SCALE		((uint32_t) 1<<16)
#define ANTENNA_LEVEL_NUMERATOR	((uint32_t) (ANTENNA_LEVEL_MILLIVOLT * ANTENNA_LEVEL_FACTOR * ANTENNA_LEVEL_SCALE + .5))
#define ANTENNA_LEVEL_DENOMINATOR (ANTENNA_LEVEL_SCALE)

static inline
void AntennaLevelInit(void) {
    ADCA.CAL = (PRODSIGNATURES_ADCACAL1 << 8) | PRODSIGNATURES_ADCACAL0; /* Load calibration data, source: https://www.avrfreaks.net/comment/2080211#comment-2080211 */
    ADCA.CTRLA = ADC_ENABLE_bm;
    ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc;
    ADCA.REFCTRL = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm;
    ADCA.PRESCALER = ADC_PRESCALER_DIV32_gc;
    ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc;

}

static inline
uint16_t AntennaLevelGet(void) {
    ADCA.CH0.CTRL |= ADC_CH_START_bm;
    while (!(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm));

    ADCA.CH0.INTFLAGS = ADC_CH_CHIF_bm;

    int16_t Result = ADCA.CH0RES - ANTENNA_LEVEL_OFFSET;
    if (Result < 0) Result = 0;

    return (uint16_t)(((uint32_t) Result * ANTENNA_LEVEL_NUMERATOR) / ANTENNA_LEVEL_DENOMINATOR);
}

void AntennaLevelTick(void);
void AntennaLevelResetMaxRssi(void);

#endif /* ANTENNALEVEL_H_ */
