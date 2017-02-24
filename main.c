/* 
 * File:   main.c
 * Author: Rain.S
 *
 * Created on February 3, 2017, 12:23 AM
 */
//Meeting Goal
//see if PWM drives servo
//see if step() drives step
//figure out how to position steppers
//figure out how to position servos
//#include <stdio.h>
#include "modules.h"

void initialize() {
    TRISA = 0xFF; // Set Port A as all input
    TRISB = 0xFF; //All input mode
    TRISC = 0x00; //All output mode
    TRISD = 0x00;
    TRISE = 0x00;
    //    config all ports input or output
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    //    initialize all pins to be low

    ADCON0 = 0x00; //Disable ADC
    ADCON1 = 0x0B; //Set AN0-1 to be analog input
    CVRCON = 0x00; // Disable CCP reference voltage output
    ADFM = 1;
    //    set digital and analog
    initLCD();
    //    __lcd_clear();
    __lcd_home();
}

void mainloop(int * bigNose, int* smallNose) {
    __lcd_newline();
    printf("Sorting...          ");
    int stop = 0;
    int sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    int startTime = getTime();
    int AA = 0;
    int C9 = 0;
    INT1IE = 1;
    ei(); //Enable all interrupts
    while (!stop) {
        moveSmallNose(smallNose, AA);
        moveBigNose(bigNose, C9);
        moveXMotors();
        //        LATC = LATC | 0b011001111; // running all X motors for demonstration purpose
        //        simulateSort(&stop);
        AA = checkAA(sorted);
        AA = (AA == -1) ? *smallNose : AA;
        C9 = checkC9V(sorted);
        C9 = (C9 == -1) ? *bigNose : C9;
        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 15) {
            stop = 1;
        }
        if (getTime() - startTime >= 180) {
            stop = 1;
        }
    }
    di(); //disable all interrupts
    //    interfacing with EEPROM
    //    showInfo(getTime() - startTime, sorted);
    stopMoving();
    return;
}

void simulate() {
    //    simulate battery detection
    //    println("Sorting...          ");
    //    int stop = 0;
    //    int sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    ////    int startTime = getTime();
    //    while (!stop) {
    //        captureKeypad();
    ////        checkAA(sorted);
    //        checkC9V(sorted);
    //        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 5) {
    //            stop = 1;
    //        }
    //    }
    //    printf("sorted %d,%d,%d,%d",sorted[0],sorted[1],sorted[2],sorted[3]);
    //    pause();

    //    simulate PWM

    //    PWM1_Start();
    //    while (1) {
    //        int duty = 0;
    //        int freq = 50;
    //        for (duty; duty < 1025; duty = duty+16) {
    //            captureKeypad();
    //            runPWM1(duty,freq);
    //            __lcd_home();
    //            printf("duty: %d    ", duty);
    //        }
    //    }
    //    while (1) {
    //        captureKeypad();
    //        runPWM1(256, 50);
    //        captureKeypad();
    //        PWM1_Stop();
    //    }
    //    while (1) {
    //        step(50);
    ////        captureKeypad();
    //    }
    //    __lcd_newline();
    //    printf("Sorting...          ");
    //    int stop = 0;
    //    int sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    //    int startTime = getTime();
    //    int AA = 0;
    //    int C9 = 0;
    //    INT1IE = 1;
    //    //    ei(); //Enable all interrupts
    //    while (!stop) {
    //        moveSmallNose(smallNose, AA);
    //        captureKeypad();
    //        moveBigNose(bigNose, C9);
    //        captureKeypad();
    //        moveXMotors();
    //        captureKeypad();
    //        AA = checkAA(sorted);
    //        AA = (AA == -1) ? *smallNose : AA;
    //        captureKeypad();
    //        C9 = checkC9V(sorted);
    //        C9 = (C9 == -1) ? *bigNose : C9;
    //        captureKeypad();
    //
    //        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 5) {
    //            stop = 1;
    //        }
    //    }
    //    //    di(); //disable all interrupts
    //    //    interfacing with EEPROM
    //    showInfo(179, sorted);
    //    stopMoving();
    //    return;
    while (1) {
        captureKeypad();
        int onus = 500;
        __lcd_home();
        printf("width: %d us   ", onus);
        for (int i = 0; i < 1000; i++) {
            LATC2 = 1 ^ LATC2;
            __delay_us(1000);
            LATC2 = 1 ^ LATC2;
            __delay_us(2000);
        }
    }
}

int main(int argc, char** argv) {
    initialize(); //Initiallize LCD and PORTs
    // Enter Standby mode
    //        __delay_1s();
    printf((unsigned char *) "Welcome!          ");
    //    captureKeypad();
    //    //        displays RTC
    //    // put all noses to the uncharged bin position
    //    int bigNose = 0, smallNose = 0;
    //    moveBigNose(&bigNose, 0);
    //    captureKeypad();
    //    moveSmallNose(&smallNose, 0);
    //    captureKeypad();
    //    __lcd_newline();
    printf("Press Start          ");
    // once D is pressed enter mainloop();
//    while (captureKeypad() != 15);
    //    mainloop(&bigNose, &smallNose);
    simulate();
    RESET();
}

