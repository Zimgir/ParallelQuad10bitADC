
#ifndef ADC_FUNC_H
#define	ADC_FUNC_H

#define SYSTEM_CLOCK 79227500UL
#define FCY SYSTEM_CLOCK/2

#include <xc.h>
#include <libpic30.h>


void initSystemClock();
void initIOPorts();
void initInterrupts();
void initADC();
void outputSample(char channel);

#endif	/* ADC_FUNC_H */

