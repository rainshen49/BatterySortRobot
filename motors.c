#include "modules.h"

//PWM

int set_PWM_freq(long fre) {

    PR2 = (_XTAL_FREQ / (fre * 4 * TMR2PRESCALE)) - 1;
    //    freq = fre;
    return (_XTAL_FREQ / (fre * TMR2PRESCALE));
}

void PWM1_Start() {
    //Configure CCP1CON, single output mode, all active high

    P1M1 = 0;
    P1M0 = 0;
    CCP1M3 = 1;
    CCP1M2 = 1;
    CCP1M1 = 0;
    CCP1M0 = 0;

    //Configure prescale values for Timer2, according to TMR2PRESCALAR
#if TMR2PRESCALAR == 1
    T2CKPS0 = 0;
    T2CKPS1 = 0;
#elif TMR2PRESCALAR == 4
    T2CKPS0 = 1;
    T2CKPS1 = 0;
#elif TMR2PRESCALAR == 16
    T2CKPS0 = 1;
    T2CKPS1 = 1;
#endif

    // Enable timer 2
    TMR2ON = 1;

    // Enable PWM output pins
    //    TRISCbits.TRISC2 = 0;
}

void PWM2_Start() {
    //Configure CCP2CON, enter PWM mode

    CCP1M3 = 1;
    CCP1M2 = 1;

    //Configure prescale values for Timer2, according to TMR2PRESCALAR
#if TMR2PRESCALAR == 1
    T2CKPS0 = 0;
    T2CKPS1 = 0;
#elif TMR2PRESCALAR == 4
    T2CKPS0 = 1;
    T2CKPS1 = 0;
#elif TMR2PRESCALAR == 16
    T2CKPS0 = 1;
    T2CKPS1 = 1;
#endif

    // Enable timer 2
    TMR2ON = 1;

    // Enable PWM output pins
    TRISCbits.TRISC1 = 0;
}

void set_PWM1_duty(unsigned int duty, int Max_Duty) {
    if (duty < 1024) {

        duty = ((float) duty / 1023) * Max_Duty;
        CCP1X = duty & 2; // Set the 2 lest significant bit in CCP1CON register
        CCP1Y = duty & 1;
        CCPR1L = duty >> 2; // Set rest of the duty cycle bits in CCPR1L
    }
}

void set_PWM2_duty(unsigned int duty, int Max_Duty) {
    if (duty < 1024) {

        duty = ((float) duty / 1023) * Max_Duty;
        CCP2X = duty & 2; // Set the 2 lest significant bit in CCP2CON register
        CCP2Y = duty & 1;
        CCPR2L = duty >> 2; // Set rest of the duty cycle bits in CCPR2L
    }
}

void PWM1_Stop() {

    CCP1M3 = 0;
    CCP1M2 = 0;
    CCP1M1 = 0;
    CCP1M0 = 0;
}

void PWM2_Stop() {

    CCP2M3 = 0;
    CCP2M2 = 0;
    CCP2M1 = 0;
    CCP2M0 = 0;
}

void runPWM1(unsigned int duty, long freq) {
    PWM1_Start();
    int max_duty = set_PWM_freq(freq);
    set_PWM1_duty(duty, max_duty);
}

void runPWM2(unsigned int duty, long freq) {
    PWM2_Start();
    int max_duty = set_PWM_freq(freq);
    set_PWM2_duty(duty, max_duty);
}
//end PWM

//Stepper Driving on RC0-3

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
    switch(next){
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