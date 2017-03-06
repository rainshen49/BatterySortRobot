#include "common.h"
#include "sensors.h"

int AD(char channel) {
    // Select A2D channel to read
    ADCON0 = ((channel << 2));
    ADON = 1;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO_NOT_DONE);
    return (ADRESH << 8) | ADRESL;
}

int checkV(int voltage, float max) {
    //    return 0 if not exist, 1 if exist and over 85 charged, -1 if exist but uncharged
    println(1,"%f02 received    ", (float) voltage / 1024 * MAXV);
    if (voltage < 50) {
        return 0;
    } else if (voltage > 0.85 * 1023 * max / MAXV) {
        return 1;
    } else {
        return -1;
    }
}

int checkAA(unsigned char *sorted) {
    //    input RA0(AN0)
    println(0,"AA:          ");
    switch (checkV(AD(0), 1.5)) {
        case 1:
            println(0,"charged AA          ");
            sorted[0]++;
            return 1;
        case -1:
            println(0,"uncharged AA          ");
            sorted[3]++;
            return 0;
        case 0:
            println(0,"no AA          ");
            return -1;
        default:
            return 0;
    }
}

int checkC(unsigned char *sorted) {
    //    input RA1(AN1)
    println(0,"C:          ");
    switch (checkV(AD(1), 1.5)) {
        case 1:
            println(0,"charged C          ");
            sorted[1]++;
            return 1;
        case -1:
            println(0,"uncharged C          ");
            sorted[3]++;
            return 0;
        case 0:
            println(0,"no C          ");
            return -1;
        default:
            return 0;
    }
}

int check9(unsigned char *sorted) {
    //    input RA2(AN2)
    println(0,"9V:          ");
    switch (checkV(AD(2), 4)) {
        case 1:
            println(0,"charged 9V          ");
            sorted[2]++;
            return 1;
        case -1:
            println(0,"uncharged 9V          ");
            sorted[3]++;
            return 0;
        case 0:
            println(0,"no 9V          ");
            return -1;
        default:
            return 0;
    }
}