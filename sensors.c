#include "modules.h"

int AD(char channel) {
    // Select A2D channel to read
    ADCON0 = ((channel << 2));
    ADON = 1;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO_NOT_DONE) {

        __delay_ms(10);
    }
    return (ADRESH << 8) | ADRESL;
}

int checkV(int voltage, float max) {
    //    return 0 if not exist, 1 if exist and over 85 charged, -1 if exist but uncharged
    __lcd_newline();
    printf("%d received    ", voltage);
    __delay_1s();
    if (voltage > 1023 * 4 / 5) {
        return 0;
    } else if (voltage > 0.85 * 1023 * max / 5) {
        return 1;
    } else {

        return -1;
    }
}

int checkAA(int* sorted) {
    //    input RA0(AN0)
    __lcd_home();
    printf("AA:          ");
    __lcd_newline();
    __delay_ms(1000);
    switch (checkV(AD(0), 1.5)) {
        case 1:
            __lcd_home();
            printf("charged AA          ");
            __delay_ms(1000);
            sorted[0]++;
            return 1;
        case -1:
            __lcd_newline();
            printf("uncharged AA          ");
            __delay_ms(1000);
            sorted[3]++;
            return 0;
        case 0:
            __lcd_home();
            printf("no AA          ");
            return -1;
        default:
            return 0;
    }
}

int checkC9V(int* sorted) {
    //    output RE1-3 corresponds to R1,R2,R3&4
    //    input RA1(AN1),RA2(AN2),RA3(AN3)
    LATE = 0b1000;
    __lcd_home();
    printf("V3          ");
    switch (checkV(AD(3), 1.5)) {
        case 0:
            __lcd_home();
            printf("9:          ");
            __delay_ms(1000);
            //            its a 9v, not C, now checking 
            int i = 1;
            for (i; i < 3; i++) {
                //          first check channel 1, then check channel 2
                __lcd_newline();
                printf("channel %d   ", i);
                __lcd_home();
                __delay_ms(900);
                LATE = i == 1 ? 0 : 0b0110;
                switch (checkV(AD(i), 3.6)) {
                    case 1:
                        __lcd_home();
                        printf("charged 9V          ");
                        __delay_ms(1000);
                        sorted[2]++;
                        return 2;
                    case -1:
                        __lcd_newline();
                        printf("uncharged 9V          ");
                        __delay_ms(1000);
                        sorted[3]++;
                        return 0;
                    default:
                        __lcd_home();
                        printf("no 9          ");
                        continue;
                }
            }
            return -1;
        case 1:
            //            its charged C
            __lcd_home();
            printf("charged C          ");
            __delay_ms(1000);
            sorted[1]++;
            return 1;
        case -1:
            __lcd_newline();
            printf("uncharged C          ");
            __delay_ms(1000);
            sorted[3]++;
            return 0;
        default:
            return -1;
    }
}