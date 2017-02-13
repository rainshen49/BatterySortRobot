/* 
 * File:   main.c
 * Author: Rain.S
 *
 * Created on February 3, 2017, 12:23 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "configBits.h"
#include "constants.h"
#include "lcd.h"
#include "modules.h"
#include "macros.h"

//const char keys[] = "123A456B789C*0#D";

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
}

//void simulateSort(int * stop) {
//    unsigned int keypad = captureKeypad(); // for demonstration purpse use the keypad to control the flow
//    switch (keypad) {
//        case(15):
//            println("Emergency! All");
//            LATC = 0;
//            println("Motors Stop");
//            LATC = 0;
//            pause();
//            *stop = 1;
//            break;
//        case(11):
//            //            Normal exit
//            *stop = 1;
//            break;
//        default:
//            break;
//    }
//}

void mainloop(int * bigNose, int* smallNose) {
    println("Sorting...");
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
        AA = AA==-1?*smallNose:AA;
        C9 = checkC9V(sorted);
        C9 = C9==-1?*bigNose:C9;
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

void simulate(){
//    simulate battery detection
    println("Sorting...");
    int stop = 0;
    int sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
//    int startTime = getTime();
    while (!stop) {
        captureKeypad();
//        checkAA(sorted);
        checkC9V(sorted);
        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 5) {
            stop = 1;
        }
    }
    printf("sorted %d,%d,%d,%d",sorted[0],sorted[1],sorted[2],sorted[3]);
    pause();
}

int main(int argc, char** argv) {
    initialize(); //Initiallize LCD and PORTs
    // Enter Standby mode
    println((unsigned char *) "Welcome!");
    //        displays RTC
    // put all noses to the uncharged bin position
    int bigNose = 0, smallNose = 0;
    moveBigNose(&bigNose, 0);
    moveSmallNose(&smallNose, 0);

    // once D is pressed enter mainloop();
    while (captureKeypad() != 15);
//    mainloop(&bigNose, &smallNose);
    simulate();
    RESET();
}

