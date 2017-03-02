#include "common.h"
#include "motors.h"

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

void CW90(int * port) {
    __lcd_home();
    printf("move clockwise  ");
    PWMC(3000, 96, port);
}

void CCW90(int * port) {
    __lcd_newline();
    printf("move cclockwise    ");
    PWMC(400, 700, port);
}

void stopMoving(char em) {
    di();
    __lcd_home();
    LATC = 0;
    printf(em ? "Emergency          " : "Stopped          ");
    __delay_ms(700);
    if (em)
        pause();
}