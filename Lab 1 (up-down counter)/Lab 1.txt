// One Hz up counter using a multiplexed 7-segment display

#include <p18cxxx.h>
#include <BCDlib.h>

char digits[5];
unsigned char Qstates, IntCounter, i;
unsigned int counter;

char SSCodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // 0 to 9 kirmel tdawe l SSD

void Setup(void);

void main(void) {
    Setup(); 			// system initialization
    while (1); 		       // wait for interrupt
}

#pragma code ISR = 0x0008
#pragma interrupt ISR

void ISR(void) {
    INTCONbits.T0IF = 0;    	        // acknowledge interrupt
    TMR0L = 256 - 250; 		       // 32us * 125 = 4ms

    PORTD = 0x00; 	             // turn digit off before selection
    PORTA = Qstates;                // turn on appropriate transistor
    PORTD = SSCodes[digits[i++]];  // send data to 7-segment display

    Qstates >>= 1;                // get ready for next digit
    if (i == 5) {
        i = 1;                  // point to digits[1]
        Qstates = 0b00001000;  // start with MSD
    }

    if (--IntCounter == 0) {
        IntCounter = 12;    // 1 sec has elapsed, start anew
        counter++;
        if (counter == 10000)
            counter = 0;
        
        Bin2BcdE(counter, digits); // increment counter & convert to BCD
    }
}

void Setup(void) {
    ANSELD = 0x00;   		// PORTD: digital I/O
    ANSELA &= 0xF0; 		// PORTA<3:0>: digital I/O, RA5: analog input
    
    TRISA &= 0xF0; 		// RA3..RA0 are output pins //1s are not outputs
    TRISD = 0x00; 		// PORTD pins are output

    counter = 0; 		// start with a count of 0
    Bin2BcdE(counter, digits);  // convert to BCD //Extended

    i = 1; // point to digits[1]
    Qstates = 0b00001000;	// start with MSD

    T0CON = 0b11010100; 	// divide clock by 256, 8-bit mode
    TMR0L = 256 - 250; 		// 32us * 125 = 4ms

    IntCounter = 12; 		// 4ms * 250 = 1 sec

    INTCONbits.T0IE = 1; 	// enable real-time interrupt
    INTCONbits.GIE = 1;

}