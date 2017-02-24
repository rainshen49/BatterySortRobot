#include "common.h"
#include "sensors.h"

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
    printf("%d received    ", voltage);
    __lcd_newline();
    __delay_1s();
    if (voltage < 50) {
        return 0;
    } else if (voltage > 0.85 * 1023 * max / 5) {
        return 1;
    } else {
        return -1;
    }
}

int checkAA(int *sorted) {
    //    input RA0(AN0)
    __lcd_home();
    printf("AA:          ");
    __lcd_newline();
    switch (checkV(AD(0), 1.5)) {
        case 1:
            printf("charged AA          ");
            sorted[0]++;
            return 1;
        case -1:
            printf("uncharged AA          ");
            sorted[3]++;
            return 0;
        case 0:
            printf("no AA          ");
            return -1;
        default:
            return 0;
    }
}

int checkC(int *sorted) {
    //    input RA1(AN1)
    __lcd_home();
    printf("C:          ");
    __lcd_newline();
    switch (checkV(AD(1), 1.5)) {
        case 1:
            printf("charged C          ");
            sorted[1]++;
            return 1;
        case -1:
            printf("uncharged C          ");
            sorted[3]++;
            return 0;
        case 0:
            printf("no C          ");
            return -1;
        default:
            return 0;
    }
}

int check9(int *sorted) {
    //    input RA2(AN2)
    __lcd_home();
    printf("9V:          ");
    __lcd_newline();
    switch (checkV(AD(2), 4)) {
        case 1:
            printf("charged 9V          ");
            sorted[2]++;
            return 1;
        case -1:
            printf("uncharged 9V          ");
            sorted[3]++;
            return 0;
        case 0:
            printf("no 9V          ");
            return -1;
        default:
            return 0;
    }
}