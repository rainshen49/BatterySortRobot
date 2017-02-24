#include "common.h"
#include "motors.h"

void PWMC(int widthus, int cycles, int * port) {
    widthus /= 10;
    widthus -= 1;
    int countdown = widthus;
//    __lcd_newline();
//    printf("countdown %d", countdown);
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

void CW90(int * port) {
    PWMC(3000, 64, port);
}

void CCW90(int * port) {
    PWMC(400, 450, port);
}

void stopMoving() {
    di();
    __lcd_home();
    LATC = 0;
    printf("Stopped          ");
    pause();
    //    ei();
}