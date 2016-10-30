

// Initialize ADC card interface
int adcInit();

// Get frame of quad samples in a matrix buffer
void adcFillFrame(int * frame, int size);

// Get a quad chanel sample array from the ADC in an array buffer
void adcFillSample(int * sample);

// Read sample bits function
int adcReadSample();

// Vibrate a motor at 'chanel' with 'power'
int adcVibrate(int channel, int power);

// Print frame function
void adcPrintFrame();
int adcPrintint(int * frame,int loc);
int adcrms(int * frame,int size, int mic,int shift);
