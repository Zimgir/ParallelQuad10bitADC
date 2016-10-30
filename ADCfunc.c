#include "ADCfunc.h"

void initSystemClock() {

    // Disable watchdog timer
    RCONbits.SWDTEN = 0;

    // Configure PLL prescaler, PLL postscaler, PLL divisor for 39.61 MIPS OPERATION

    PLLFBD = 41; // M = 43

    CLKDIVbits.PLLPOST = 0; // N2 = 2

    CLKDIVbits.PLLPRE = 0; // N1 = 2

    // Initiate Clock Switch to Internal FRC with PLL (NOSC = 0b001)
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(0x01);

    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b001);

    // Wait for PLL to lock
    while (OSCCONbits.LOCK != 1);
    
     // Configure Auxiliary PLL   
    ACLKCONbits.FRCSEL = 1;     // Internal FRC is clock source for auxiliary PLL */
    ACLKCONbits.ENAPLL = 1;     // APLL is enabled
    ACLKCONbits.SELACLK = 1;    // Auxiliary PLL provides the source clock for the */
    ACLKCONbits.APSTSCLR = 7;   // Auxiliary Clock Output Divider is Divide by 1 */
  
    //Wait for Auxiliary PLL to Lock */
    while(ACLKCONbits.APLLCK != 1); 

}

void initIOPorts() {

    // RA0 = AN0 IN, RA1 = AN1 IN, RA2 = AN2 IN, RA3 = CH0 IN, RA4 = CH1 IN
    TRISA = 0x001F;

    // RB0 = AN3 IN, RB1-RB10 = SMP0-SMP9 OUT, RB11 = BUSY OUT, RB12 = SAMPLE IN,
    // RB13-RB15 = OPTION OUT
    TRISB = 0x1001;

    // Analog pins for dual simultaneous sampling: AN0, AN1, AN2, AN3
    ADPCFG = 0x0FF0;

    // Unlock peripheral pin select
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));

    // Map SAMPLE external interrupt pin to RP12
    RPINR0bits.INT1R = 12;

    // Map CHANNEL external interrupt pin to RP13
    RPINR1bits.INT2R = 13;

    // Lock peripheral pin select
    __builtin_write_OSCCONL(OSCCON | (1 << 6));
    
    LATA = 0x0000;
    LATB = 0x0000;

}

void initADC() {
    
    // ADC Clock configuration
    ADCONbits.SLOWCLK = 1;      // Auxiliary PLL: ACLK = FRC * 16 / 1 = 117.92 MHz
    ADCONbits.ADCS = 4;         // ADC clock = ACLK/5 = 23.584 MHz
    
    // ADC Options configuration
    ADCONbits.FORM = 0;         // Integer data format
    ADCONbits.EIE = 0;          // Early Interrupt disabled
    ADCONbits.ORDER = 0;        // Convert even channel first
    ADCONbits.SEQSAMP = 0;      // Select simultaneous sampling
    ADCONbits.ASYNCSAMP = 0;    // Synchronous Sampling

    // ADC inputs configuration
    ADPCFGbits.PCFG0 = 0;       //AN0
    ADPCFGbits.PCFG1 = 0;       //AN1   
    ADPCFGbits.PCFG2 = 0;       //AN2
    ADPCFGbits.PCFG3 = 0;       //AN3

    // Select global software trigger for all pairs
    ADCPC0bits.TRGSRC0 = 2;         // AN0 AN1 pair global trigger source
    ADCPC0bits.TRGSRC1 = 2;         // AN2 AN3 pair global trigger source

    // ADC interrupts configuration interrupt of last pair conversion complete
    IPC27bits.ADCP1IP = 7;      // Set ADC Pair 1 Interrupt Priority (Level 7)
    IFS6bits.ADCP1IF = 0;       // Clear ADC Pair 1 Interrupt Flag
    IEC6bits.ADCP1IE = 1;       // Enable ADC Pair 1 Interrupt
    
    
    ADCONbits.ADON = 1; // Enable ADC Module
}

void initInterrupts() {

    // Disable nested interrupts
    INTCON1bits.NSTDIS = 1;

    // Highest priority for SAMPLE interrupt pin
    IPC5bits.INT1IP = 5;
    // Lower priority for CHANNEL interrupt pin
    IPC7bits.INT2IP = 4;

    // Clear external pin interrupt flags
    IFS1bits.INT1IF = 0;
    IFS1bits.INT2IF = 0;

    // Set external pin interrupt on rising edge
    INTCON2bits.INT1EP = 0;
    INTCON2bits.INT2EP = 0;

    // Enable external pin interrupts
    IEC1bits.INT1IE = 1; // Sample RB12 pin interrupt enabled
    IEC1bits.INT2IE = 0; // Channel RB13 pin interrupt disabled
}

void outputSample(char channel) {
    
    // Output the new channel sample
    switch (channel) {

        case 0:
        {
            LATB = (ADCBUF0 << 1);
            break;
        }

        case 1:
        {
            LATB = (ADCBUF1 << 1);
            break;
        }
        case 2:
        {
            LATB = (ADCBUF2 << 1);
            break;
        }
        case 3:
        {
            LATB = (ADCBUF3 << 1);
            break;
        }
    }
}

