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

int checkV(char channel, int threshhold) {
    //    gives -1 if DNE, 0 if uncharged, 1 if charged
    int voltage = AD(channel);
    if (voltage < 20) return -1;
    __delay_ms(1);
    int tmp = AD(channel);
    while (tmp - voltage > 5) {
        voltage = tmp;
        __delay_ms(1);
        tmp = AD(channel);
    };
    if (voltage > 0.85 * threshhold)return 1;
    return 0;
}

void printV(int charged) {
    //    line0();
    //    captureKeypad();
    if (charged < 0) {
        printf("-1 ");
    } else {
        printf(" %d ", charged);
        //        captureKeypad();
    }
}

int checkAA(unsigned char *sorted) {
    printf("AA:");
    //    check voltage
    int V = checkV(2, 1.5 / MAXV * 1023);
    printV(V);
    switch (V) {
        case 1:
            sorted[0]++;
        case 0:
            sorted[3]++;
    }
    return V;
}

int checkC(unsigned char *sorted) {
    //    input RA1(AN1)
    //    line0();
    printf("C:");
    int V = checkV(0, 1.5 / MAXV * 1023);
    printV(V);
    switch (V) {
        case 1:
            sorted[1]++;
        case 0:
            sorted[3]++;
    }
    return V;
}

int check9(unsigned char *sorted) {
    //    input RA2(AN2)
    //    line0();
    printf("9V:");
    int V = checkV(5, 3.65 / MAXV * 1023);
    printV(V);
    switch (V) {
        case 1:
            sorted[2]++;
        case 0:
            sorted[3]++;
    }
    return V;
}