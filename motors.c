#include "modules.h"

void PWMC0(int us,int cycle) {
    us-=2;
    int i = 0;
    LATC0 = 1 ^ LATC0;
    for (i = 0; i < us; i++) {
//        __delay_us(1);
    }
    us -= 2;
    LATC0 = 1 ^ LATC0;
    for (i = 0; i < cycle - us; i++) {
//        __delay_us(1);
    }
}

void step(int wait) {
    //    __lcd_newline();
    //    printf("step          ");
    switch (PORTC & 0x0F) {
        case(1):
            LATC0 = 0;
            LATC1 = 1;
            break;
        case(2):
            LATC1 = 0;
            LATC2 = 1;
            break;
        case(4):
            LATC2 = 0;
            LATC3 = 1;
            break;
        case(8):
            LATC3 = 0;
            LATC0 = 1;
            break;
        default:
            LATC0 = 1;
            LATC1 = 0;
            LATC2 = 0;
            LATC3 = 0;
            break;
    }
    for (wait; wait > 0; wait--) {
        __delay_ms(10);
    }
}
//

void moveXMotors() {
    //    move the X motors by 90 degree
    __lcd_home();
    printf("Rotating Xs          ");
    runPWM1(256, 3100); //duty, freq
    __delay_ms(1000);
    PWM1_Stop();
}

void moveBigNose(int * prev, int next) {
    //    move the big nose to the next location, 0 uncharged, 1 C, 2 9V
    //    determine how to move based on prev and next and then move the motors
    //    simulation
    __lcd_home();
    printf("bigNose to          ");
    __lcd_newline();
    switch (next) {
        case 0:
            printf("uncharged          ");
            break;
        case 1:
            printf("C          ");
            break;
        case 2:
            printf("9V          ");
            break;
        default:
            printf("uncharged          ");
    }
    //    end simulation
    if (*prev == next)return;
    while (*prev < next) {
        //        move the motor a certain degree
        *prev++;
    }
    while (*prev > next) {
        //        move the motor a certain degree inverse

        *prev--;
    }
}

void moveSmallNose(int * prev, int next) {
    //    move the big nose to the next location, 0 uncharged, 1AA
    //    determine how to move based on prev and next and then move the motors
    //    simulation
    __lcd_home();
    printf("smallNose to          ");
    __lcd_newline();
    if (next) {
        printf("AA          ");
    } else {
        printf("uncharged          ");
    }

    //    end simulation
    if (*prev == next)return;
    while (*prev < next) {
        //        move the motor a certain degree
        *prev++;
    }
    while (*prev > next) {
        //        move the motor a certain degree inverse

        *prev--;
    }
}

void stopMoving() {
    di();
    __lcd_home();
    printf("Stopped          ");
    pause();
    //    ei();
}