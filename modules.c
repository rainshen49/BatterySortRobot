#include "modules.h"

unsigned int captureKeypad() {
    while (!PORTBbits.RB1);
    unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
    while (PORTBbits.RB1);
    INT1IF = 0; //Clear flag bit

    return keypress;
}

void interrupt keypressed(void) {
    //    printf("%d",INT1IF);
    if (INT1IF) {

        unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
        INT1IF = 0; //Clear flag bit
        printf("Emergency          ");
        stopMoving();
    }
}

void showInfo(unsigned int time, unsigned int * sorted) {
    di();
    __lcd_home();
    printf("Sorting Complete          ");
    __delay_ms(1000);
    __lcd_home();
    printf("Press a number          ");
    __lcd_newline();
    printf("For more info          ");
    int type = 0;
    while (1) {
        type = captureKeypad();
        switch (type) {
            case(0):
                __lcd_home();
                printf("Time Elapsed:          ");
                __lcd_newline();
                printf("%d seconds          ", time);
                break;
            case(1):
                __lcd_home();
                printf("Total Sorted:          ");
                __lcd_newline();
                printf("%d batteries          ", sorted[0] + sorted[1] + sorted[2] + sorted[3]);
                break;
            case(2):
                __lcd_home();
                printf("AA Sorted:          ");
                __lcd_newline();
                printf("%d          ", sorted[0]);
                break;
            case(3):
                __lcd_home();
                printf("C Sorted:          ");
                __lcd_newline();
                printf("%d          ", sorted[1]);
                break;
            case(4):
                __lcd_home();
                printf("9V Sorted:          ");
                __lcd_newline();
                printf("%d          ", sorted[2]);
                break;
            case(5):
                __lcd_home();
                printf("Uncharged:          ");
                __lcd_newline();
                printf("%d          ", sorted[3]);
                break;
            case(15):
                RESET();

            default:
                break;
        }
    }
    ei();
}

int getTime() {
    //    get time from RTC
    int time = 0;

    return time;
}

