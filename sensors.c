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

unsigned char fluctuate(int a, int b) {
    //    return 1 if bigger difference than 200
    int i = a - b;
    if (a < b)i = -i;
    return i > 200;
}

int checkV(int channel, float max) {
    //    return 0 if not exist, 1 if exist and over 85 charged, -1 if exist but uncharged
    //    line1();
    //    printf("%f02 V      ", (float) voltage / 1024 * MAXV);
    int voltage = AD(channel);
    int voltage2 = AD(channel);
    int voltage3 = AD(channel);
    if (voltage > 800 || voltage2 > 800 || voltage3 > 800)return 0;
    if (fluctuate(voltage, voltage2) || fluctuate(voltage2, voltage3) || fluctuate(voltage, voltage3))return 0;
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
    //    line0();
    printf("AA:");
    switch (checkV(0, 1.5)) {
        case 1:
            //            line0();
            printf(" 1 ");
            sorted[0]++;
            return 1;
        case -1:
            //            line0();
            printf(" 0 ");
            sorted[3]++;
            return 0;
        case 0:
            //            line0();
            printf("-1 ");
            return -1;
        default:
            return 0;
    }
}

int checkC(unsigned char *sorted) {
    //    input RA1(AN1)
    //    line0();
    printf("C:");
    switch (checkV(1, 1.5)) {
        case 1:
            //            line0();
            printf(" 1 ");
            sorted[1]++;
            return 1;
        case -1:
            //            line0();
            printf(" 0 ");
            sorted[3]++;
            return 0;
        case 0:
            //            line0();
            printf("-1 ");
            return -1;
        default:
            return 0;
    }
}

int check9(unsigned char *sorted) {
    //    input RA2(AN2)
    //    line0();
    printf("9V:");
    switch (checkV(2, 4)) {
        case 1:
            //            line0();
            printf(" 1 ");
            sorted[2]++;
            return 1;
        case -1:
            //            line0();
            printf(" 0 ");
            sorted[3]++;
            return 0;
        case 0:
            //            line0();
            printf("-1 ");
            return -1;
        default:
            return 0;
    }
}