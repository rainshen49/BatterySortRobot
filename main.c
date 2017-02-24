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
#include "common.h"
#include "modules.h"
#include "sensors.h"
#include "motors.h"

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
    ADCON1 = 0x0C; //Set AN0-2 to be analog input
    CVRCON = 0x00; // Disable CCP reference voltage output
    ADFM = 1;
    //    set digital and analog
    initLCD();
    __lcd_clear();
    __lcd_home();
}

void mainloop(int *bigNose, int *smallNose) {
    __lcd_newline();
    printf("Sorting...          ");
    int stop = 0;
    int sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    int startTime = getTime();
    int AA = -1, C = -1, V9 = -1;
    LATC3 = 1; // start DC
    INT1IE = 1;
    ei(); //Enable all interrupts
    while (!stop) {
        AA = checkAA(sorted);
        C = checkC(sorted);
        V9 = check9(sorted);
        if (!AA) {
            // move servo left
        } else if (AA == 1) {
            // ,pve servo right
        }
        // same for 9V and C servos
        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 15) {
            stop = 1;
        }
        if (getTime() - startTime >= 180) {
            stop = 1;
        }
    }
    di(); //disable all interrupts
    //    interfacing with EEPROM
    showInfo(getTime() - startTime, sorted);
    stopMoving();
    return;
}

void simulate() {
    __lcd_home();
    printf("Simulating...          ");
    int stop = 0;
    int sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    int AA = -1, C = -1, V9 = -1;
    LATC3 = 1; // start DC
    while (!stop) {
        captureKeypad();
        AA = checkAA(sorted);
        //        captureKeypad();
        __delay_1s();
        C = checkC(sorted);
        //        captureKeypad();
        __delay_1s();

        V9 = check9(sorted);
        __lcd_home();
        captureKeypad();

        int portCW[] = {0, 0, 0};
        int portCCW[] = {0, 0, 0};

        if (!AA) {
            portCCW[0] = 1;
            printf("AAServoLeft         ");
        } else if (AA == 1) {
            portCW[0] = 1;
            printf("AAServoRight         ");
        }
        //        captureKeypad();
        __lcd_home();
        __delay_1s();

        if (!C) {
            portCCW[1] = 1;
            printf("CServoLeft         ");
        } else if (C == 1) {
            portCW[1] = 1;
            printf("CServoRight         ");
        }
        //        captureKeypad();
        __lcd_home();
        __delay_1s();

        if (!V9) {
            portCCW[2] = 1;
            printf("9ServoLeft         ");
        } else if (V9 == 1) {
            portCW[2] = 1;
            printf("9ServoRight         ");
        }
        captureKeypad();

        CCW90(portCCW);
        __delay_ms(20);
        CW90(portCW);
        LATC = 0b00001000;
        // same for 9V and C servos
        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 15) {
            stop = 1;
        }
    }
    di(); //disable all interrupts
    //    interfacing with EEPROM
    showInfo(179, sorted);
    stopMoving();
    return;
}

//void testPWM() {
//    __lcd_home();
//    printf("Width:500us");
//    int port[] = {1, 1, 1};
//    for (int i = 0; i < 4; i++) {
//        CCW90(port);
//        __delay_1s();
//    }
//    pause();
//}

void testADC() {
    int sorted[]={1,1,1,1};
    while(1){
        int AA = checkAA(sorted);
//        printf("AA: %d",AA);
        __delay_ms(500);
    }
}

int main(int argc, char **argv) {
    initialize(); //Initiallize LCD and PORTs
    // Enter Standby mode
    printf((unsigned char *) "Welcome!          ");
    // displays RTC
    __lcd_newline();
    printf("Press Start          ");
    // once D is pressed enter mainloop();
    //    while (captureKeypad() != 15)
    //    mainloop(&bigNose, &smallNose);
    simulate();
    //    testPWM();
//    testADC();
    RESET();
}
