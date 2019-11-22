// before physical measure

// One Hz up counter using a multiplexed 7-segment display

#include <p18cxxx.h>
#include <BCDlib.h>
#include <delays.h>

unsigned char counter, Qstates, IntCounter, i;

enum {
    degree = 10, C, F
};
char SSCodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x63, 0x39, 0x71};
char digits[5] = {0, 0, 0, degree, C};

float T;

void Setup(void);

void main(void) {
    Setup(); // system initialization
    while (1) {

        PORTBbits.RB7 = 0;
        PORTBbits.RB6 = 0;
        PORTBbits.RB5 = 0;
        
        ADCON0bits.GO = 1;
        while (ADCON0bits.GO);


        T = ((unsigned short long) 100 * ADRESH) / 256;
        
        if ((0 <= T) & (T < 20))
                PORTBbits.RB5 = 1;
        else if ((20 <= T) & (T < 30))
                PORTBbits.RB6 = 1;
        else if (30 <= T)
                PORTBbits.RB7 = 1;

        if (INTCONbits.INT0F) {
            INTCONbits.INT0IF = 0;
            digits[4] = (digits[4] == C) ? F : C;
        }
        if (digits[4] == F) {
            T = T * 1.8 + 32;
        }

        Bin2Bcd((unsigned char) (T + 0.5), digits);
    }


} // wait for interrupt




#pragma code ISR = 0x0008
#pragma interrupt ISR

void ISR(void) {
    INTCONbits.T0IF = 0; // acknowledge interrupt
    TMR0L = 256 - 125; // 32us * 125 = 4ms

    PORTD = 0x00; // turn digit off before selection
    PORTA = Qstates; // turn on appropriate transistor

    PORTD = SSCodes[digits[i++]]; // send data to 7-segment display


    Qstates >>= 1; // get ready for next digit
    if (i == 5) {
        i = 1; // point to digits[0]
        Qstates = 0b00001000; // start with MSD
    }

}

void Setup(void) {
    ANSELD = 0x00; // PORTD: digital I/O
    ANSELA = 0xF0; // PORTA<3:0>: digital I/O, RA5: analog input
    TRISA = 0xF0; // RA2..RA0 are output pins
    TRISD = 0x00; // PORTD pins are output

    ANSELB = 0x00; // Port B is digital I/O
    TRISBbits.RB7 = 0;
    TRISBbits.RB6 = 0;
    TRISBbits.RB5 = 0;

    counter = 0; // start with a count of 0
    //Bin2Bcd(counter, digits); // convert to BCD
    i = 1; // point to digits[0]
    Qstates = 0b00001000; // start with MSD
    T0CON = 0b11010101; // divide clock by 64, 8-bit mode
    TMR0L = 256 - 125; // 32us * 125 = 4ms
    IntCounter = 250; // 4ms * 250 = 1 sec
    INTCONbits.T0IE = 1; // enable real-time interrupt
    INTCONbits.GIE = 1;




    ADCON0 = 0b00010001;
    ADCON1 = 0b00000000;
    ADCON2 = 0b00100101;
}
