#include "common.h"
#include "motors.h"
char shake[] = {0, 0, 0};

void PWMC(int widthus, int cycles, int * port) {
    widthus /= 10;
    widthus -= 1;
    int countdown = widthus;
    for (cycles; cycles > 0; cycles--) {
        countdown = widthus;
        LATC0 = (1 ^ LATC0) & port[0];
        LATC1 = (1 ^ LATC1) & port[1];
        LATC2 = (1 ^ LATC2) & port[2];
        for (countdown; countdown > 0; countdown--) {
            __delay_us(5);
        }
    }
}

void V9Spin(char mode) {
    //    mode: 1 charged CCW, 0 uncharged CW
    int port[] = {0, 0, 1};
    LATC = 0;
    switch (mode) {
        case 1:
            PWMC(400, 1000, port);
            break;
        case 0:
            PWMC(3000, 180, port);
            break;
    }
    __delay_ms(20);
    if (shake[2]) {
        PWMC(450, 20, port); // ccw
    } else {
        PWMC(5000, 20, port); //cw
    }
}

void CSpin(char mode) {
    //    mode: 1 charged CCW, 0 uncharged CW
    int port[] = {0, 1, 0};
    LATC = 0;

    switch (mode) {
        case 1:
            PWMC(400, 800, port);
            break;
        case 0:
            PWMC(3000, 180, port);
            break;
    }
    __delay_ms(20);

    if (shake[1]) {
        PWMC(450, 20, port); // ccw
    } else {
        PWMC(5000, 20, port); //cw
    }
}

void AASpin(char mode) {
    //    mode: 1 charged CW, 0 uncharged CCW
    int port[] = {1, 0, 0};
    LATC = 0;

    switch (mode) {
        case 1:
            PWMC(3000, 180, port);
            break;
        case 0:
            PWMC(400, 800, port);
            break;
    }
    __delay_ms(20);

    if (shake[0]) {
        PWMC(450, 20, port); // ccw
    } else {
        PWMC(5000, 20, port); //cw
    }
}

//void CW90(int * port) {
//    //    line0();printf("move clockwise  ");
//    if (port[0] + port[1] + port[2] == 0)return;
//    PWMC(3000, 120, port);
//    PWMC(400, 2, port);
//}
//
//void CCW90(int * port) {
//    //    line1();printf("move cclockwise    ");
//    if (port[0] + port[1] + port[2] == 0)return;
//    PWMC(400, 1000, port);
//    PWMC(3000, 2, port);
//}
//
//void shake(int * port) {
//    if (port[0] + port[1] + port[2] == 0)return;
//    PWMC(450, 160, port); // ccw
//    PWMC(5000, 2, port); //cw
//    __delay_ms(400);
//    PWMC(5000, 20, port); //cw
//    PWMC(450, 2, port); // ccw
//    __delay_ms(390);
//
//}

void stopMoving(char em) {
    di();
    LATC = 0;
    line0();
    printf(em ? "Emergency          " : "Stopped          ");
    if (em) pause();
}

void openGate() {
    LATC7 = 0;
    LATC5 = 1;
    __delay_ms(GATETIME);
    LATC5 = 0;
}

void closeGate() {
    LATC5 = 0;
    LATC7 = 1;
    __delay_ms(GATETIME);
    LATC7 = 0;
}