#include <p18cxxx.h>
#include <LCD4lib.h>

#include <delays.h>

void setup(void);
unsigned int startNumber;
char digits[3];
void turnOn();

void main() {
    setup();
    while (1) {

        ADCON0bits.GO = 1;
        while (ADCON0bits.GO);

        startNumber = ADRESH;
        Bin2Asc(startNumber, digits);
        DispVarStr(digits, Ln1Ch8, 3);

        
        PORTDbits.RD7 = 0;
        
        if (PORTDbits.RD2) { // up
            Bin2Asc(startNumber++, digits);
            DispVarStr(digits, Ln1Ch8, 3);
            while (PORTDbits.RD2);

        } else if (PORTDbits.RD1) { //down
            Bin2Asc(startNumber--, digits);
            DispVarStr(digits, Ln1Ch8, 3);
            while (PORTDbits.RD1);
        } else



        if (PORTDbits.RD0) {
            turnOn();
            startNumber = 0;

            while (PORTDbits.RD0);
        }
    }
}

void turnOn() {
    PORTDbits.RD7 = 1;
    do {
        Bin2Asc(startNumber, digits);
        DispVarStr(digits, Ln1Ch8, 3);
        startNumber--;
        Delay10KTCYx(200);
    } while (startNumber != 0);

}

void setup(void) {

    ANSELD = 0x00;
    TRISDbits.RD7 = 0;

    InitLCD();
    DispRomStr(Ln1Ch0, (ROM) "Start = ");
    ADCON0 = 0b00010001;
    ADCON1 = 0b00000000;
    ADCON2 = 0b00100101;

}