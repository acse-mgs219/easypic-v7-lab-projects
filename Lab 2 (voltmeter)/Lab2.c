// One Hz up counter using a multiplexed 7-segment display

#include <p18cxxx.h>
#include <BCDlib.h>
#include <delays.h>

char digits[5];
unsigned char counter, Qstates, IntCounter, i;
char SSCodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
unsigned int V;

void Setup(void);

void main(void) {
    Setup(); // system initialization
    while (1) {

        ADCON0bits.GO = 1;
        while (ADCON0bits.GO);
        Delay10KTCYx(1);
        V = ((unsigned short long) 125 * ADRES + 65.5) / 256;
        Bin2BcdE(V, digits);
        

    } // wait for interrupt
}

#pragma code ISR = 0x0008
#pragma interrupt ISR

void ISR(void) {
    INTCONbits.T0IF = 0; // acknowledge interrupt
    TMR0L = 256 - 125; // 32us * 125 = 4ms

    PORTD = 0x00; // turn digit off before selection
    PORTA = Qstates; // turn on appropriate transistor

    if (i == 2)
        PORTD = SSCodes[digits[i++]] | 0x80;
    else
        PORTD = SSCodes[digits[i++]]; // send data to 7-segment display
    
    
    Qstates >>= 1; // get ready for next digit
    if (i == 5) {
        i = 2; // point to digits[0]
        Qstates = 0b00000100; // start with MSD
    }

}

void Setup(void) {
    ANSELD = 0x00; // PORTD: digital I/O
    ANSELA = 0xF0; // PORTA<3:0>: digital I/O, RA5: analog input
    TRISA = 0xF0; // RA2..RA0 are output pins
    TRISD = 0x00; // PORTD pins are output

    counter = 0; // start with a count of 0
    //Bin2Bcd(counter, digits); // convert to BCD
    i = 2; // point to digits[0]
    Qstates = 0b00000100; // start with MSD
    T0CON = 0b11010101; // divide clock by 64, 8-bit mode
    TMR0L = 256 - 125; // 32us * 125 = 4ms
    IntCounter = 250; // 4ms * 250 = 1 sec
    INTCONbits.T0IE = 1; // enable real-time interrupt
    INTCONbits.GIE = 1;


    ADCON0 = 0b00010001;
    ADCON1 = 0b00000000;
    ADCON2 = 0b10100101;
}