#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <math.h>
#include "adc.h"

// ADC Sample bits
const char ADSMP[] = {25, 12, 16, 20, 21, 26, 19, 13, 6, 5};

// Vibration motor channel pins
const char PWMCTL[] = {7, 8, 9, 10};
// ADC control pins
enum {ADCONV = 17, ADCH0 = 23, ADCH1 = 24, ADBUSY = 27};


/*
int main(int argc, char **argv)
{
			
	// Failed to init ADC card hardware
	if(adcInit() == -1)
		return -1;
	
	
	printf("ADC program running...\n");
	
	// Row size in matrix buffer
	int size = 1000;
	
	// Create matrix buffer size x 4
	int frame[4*size];    
	

    while(1) {	

		// Fill buffer matrix with samples frame
		adcFillFrame(frame,size);
		
		// Print the samples frame
		adcPrintFrame(frame,size);
		
		// Vibrate motor at channel 0 with 20% power
		adcVibrate(0,25);
		
		delay(500);
		
		// Disable motor vibration at channel 0
		adcVibrate(0,0);
		
		delay(500);
		

    }


    return 0;
}
*/
int adcInit() {
	
	// Failed to init GPIO control
    if(wiringPiSetupGpio() == -1)
		return -1;	
	
	int i;
	
	// Init GPIO digital pins
	for(i = 0; i < 10; i++)
		pinMode(ADSMP[i],INPUT);
		
	pinMode(ADBUSY,INPUT);
	
	pinMode(ADCONV,OUTPUT);
	
	pinMode(ADCH0,OUTPUT);
	
	pinMode(ADCH1,OUTPUT);
	
	// Init PWM pins with default period of 100*100us
	for (i = 0; i < 4; i++) {
		
		// Failed to init PWM pin
		if(softPwmCreate(PWMCTL[i],0,100))
			return -1;
	}
	piHiPri(99);
	return 0;
		
}

int adcVibrate(int channel, int power) {
	
	// This function must not be used during sampling!
	// Motor vibration creates a large noise signal
	
	// Wrong channel selection error
	if(channel < 0 || channel >= 4)
		return -1;
	
	// Limit power range	
	if(power > 99)
		power = 99;
		
	else if(power < 0)
		power = 0;
		
	softPwmWrite(PWMCTL[channel],power);
	
	return 0;
	
}

void adcFillFrame(int * frame, int size) {

	int i;
	
	for(i = 0; i < size; i++)
		adcFillSample(frame + 4*i);
		
}

void adcFillSample(int * sample) {
		
	digitalWrite(ADCH0,0);
	digitalWrite(ADCH1,0);
	
	digitalWrite(ADCONV,1);
	
	while(digitalRead(ADBUSY));

	digitalWrite(ADCONV,0);
				
	sample[0] = adcReadSample();
		
	digitalWrite(ADCH0,1);
	digitalWrite(ADCH1,0);
	delayMicroseconds(1);
	
	sample[1] = adcReadSample();
	
	digitalWrite(ADCH0,0);
	digitalWrite(ADCH1,1);
	delayMicroseconds(1);
	
	sample[2]= adcReadSample();
	
	
	// Another Sample request is enabled here when channel select is 3
	digitalWrite(ADCH0,1);
	digitalWrite(ADCH1,1);
	delayMicroseconds(1);
	
	sample[3] = adcReadSample();	
			
}

int adcReadSample() {
	
	int i, sample = 0;
	
	for(i = 0; i < 10; i++)
		sample += digitalRead(ADSMP[i]) << i;
					
	return sample;	
}

void adcPrintFrame(int * frame , int size) {
	
	
	int i;
	
	for(i = 0; i < size; i++)
		printf("%04d %04d %04d %04d\n",*(frame + 4*i),*(frame + 4*i + 1),*(frame + 4*i + 2),*(frame + 4*i + 3));
	
}
int adcPrintint(int * frame,int loc) {
	return frame[loc];
}

int adcrms(int * frame,int size, int mic,int shift)
{
	float temp=0;
	int i=0;
	frame+=shift*4;
	for(i=0;i<size;i++)
	{
		temp+=(frame[4*i+mic]-512)*(frame[4*i+mic]-512);
	}
	temp/=size;
	return (int)sqrt(temp)*1000;
}