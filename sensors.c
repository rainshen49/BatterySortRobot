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
    //    captureKeypad();
    int voltage = AD(channel);
//    __delay_ms(5);
    int voltage2 = AD(channel);
//    __delay_ms(5);
    int voltage3 = AD(channel);
    //    __delay_ms(5);
    //    int voltage4 = AD(channel);

//    printf("%d,%d,%d      ", voltage, voltage2, voltage3);
//    line1();
    
    if (voltage == voltage2 && voltage2 == voltage3) {
        //        detect on stable reading:
        if (voltage > 1.1 * threshhold) {
//            printf("over");
        } else if (voltage > 0.85 * threshhold) {
//            printf("charged");
            return 1;
        } else if (voltage < 10) {
//            printf("below");
        } else {
//            printf("uncharged");
            return 0;
        }
    } else {
//        printf("fluctuate");
    }
    return -1;
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
            //            line0();
            sorted[0]++;
        case 0:
            //            line0();
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
            //            line0();
            sorted[1]++;
        case 0:
            //            line0();
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
            //            line0();
            sorted[2]++;
        case 0:
            //            line0();
            sorted[3]++;
    }
    return V;
}