
#include "ADCconfig.h"
#include "ADCfunc.h"

int main(void) {

    // Initialization routines
    initSystemClock();

    initIOPorts();

    initADC();

    initInterrupts();

    // Main loop
    while (1) {

        // If not busy output samples by channel select pins
        if (!PORTBbits.RB11) {

            if (!PORTAbits.RA4 && !PORTAbits.RA3)
                outputSample(0);

            else if (!PORTAbits.RA4 && PORTAbits.RA3)
                outputSample(1);

            else if (PORTAbits.RA4 && !PORTAbits.RA3)
                outputSample(2);

            // Last Channel elect enables another sample request
            else if (PORTAbits.RA4 && PORTAbits.RA3) {
                
                outputSample(3);
                
                IEC1bits.INT1IE = 1; // Enable another sample request
                
            }
                
        }

    }

    return 0;
}

// Conversion of last pair complete interrupt

void __attribute__((__interrupt__, no_auto_psv)) _ADCP1Interrupt(void) {

    // Clear BUSY flag
    LATBbits.LATB11 = 0;

    IFS6bits.ADCP1IF = 0; // Clear ADC Pair 1 Interrupt Flag
}

// SAMPLE interrupt pin

void __attribute__((__interrupt__, no_auto_psv)) _INT1Interrupt(void) {

    // Disable external pin interrupts to prevent bouncing
    IEC1bits.INT1IE = 0;

    LATBbits.LATB11 = 1; // Set BUSY flag

    // Set global software trigger to convert 2 pairs of inputs
    ADCONbits.GSWTRG = 1;

    ADCONbits.GSWTRG = 0; // Clear global software trigger

    IFS1bits.INT1IF = 0; // Clear INT1 interrupt flag      
}

// CHANNEL interrupt pin

void __attribute__((__interrupt__, no_auto_psv)) _INT2Interrupt(void) {

    // Unimplemented
    
    IFS1bits.INT2IF = 0; // Clear INT2 interrupt flag
}
