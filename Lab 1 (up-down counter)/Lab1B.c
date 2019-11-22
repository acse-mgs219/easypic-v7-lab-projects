// One Hz up counter using a multiplexed 7-segment display

#include        <p18cxxx.h>
#include	<BCDlib.h>

char digits[3];
unsigned char counter, Qstates, IntCounter, i;
char SSCodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void Setup(void);

void main(void)
{
	Setup();                             // system initialization
	while (1);                           // wait for interrupt
}

#pragma code ISR = 0x0008
#pragma interrupt ISR

void ISR(void)
{
    INTCONbits.T0IF = 0;	    // acknowledge interrupt
    TMR0L = 256 - 125;	            // 32us * 125 = 4ms
        
    PORTD = 0x00;	            // turn digit off before selection
    PORTA = Qstates;                // turn on appropriate transistor
    PORTD = SSCodes[digits[i++]];   // send data to 7-segment display
    Qstates >>= 1;	            // get ready for next digit

    if (i == 3){
	i = 0;                      // point to digits[0]
	Qstates = 0b00000100;	    // start with MSD
    }

    if (--IntCounter == 0){
	IntCounter = 250;	        // 1 sec has elapsed, start anew
	Bin2Bcd(++counter, digits);	// increment counter & convert to BCD
    }
}

void Setup(void)
{
    ANSELD = 0x00;                              // PORTD: digital I/O
    ANSELA &= 0xF0;                             // PORTA<3:0>: digital I/O, RA5: analog input

    TRISA &= 0xF8;                              // RA2..RA0 are output pins
    TRISD = 0x00;                               // PORTD pins are output

    counter = 0;	// start with a count of 0
    Bin2Bcd(counter, digits);	// convert to BCD

    i = 0;	// point to digits[0]

    Qstates = 0b00000100;	// start with MSD

    T0CON =  0b11010101;	// divide clock by 64, 8-bit mode

    TMR0L = 256 - 125;	// 32us * 125 = 4ms
    IntCounter = 250;	// 4ms * 250 = 1 sec

    INTCONbits.T0IE = 1;	// enable real-time interrupt
    INTCONbits.GIE = 1;
}