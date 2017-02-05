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

//const char keys[] = "123A456B789C*0#D";
/*
 * 
 */
//void pointBigNoseTo(int Bin){
//    
//}

void initialize() {
    TRISC = 0x00;
    TRISD = 0x00; //All output mode
    TRISB = 0xFF; //All input mode
    LATB = 0x00;
    LATC = 0x00;
    ADCON0 = 0x00; //Disable ADC
    ADCON1 = 0xFF; //Set PORTB to be digital instead of analog default  
    initLCD();
}

void simulateSort(int * stop) {
    unsigned int keypad = captureKeypad(); // for demonstration purpse use the keypad to control the flow
    switch (keypad) {
        case(15):
            println("Emergency! All");
            LATC = 0;
            println("Motors Stop");
            *stop = 1;
            break;
        case(11):
            //            Normal exit
            *stop = 1;
            break;
        default:
            break;
    }
}

void mainloop(int * bigNose, int* smallNose) {
    println("Sorting...");
    int stop = 0;
    int sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    int startTime = 0;
    int AA = 0;
    int C9 = 0;
    //    ei(); //Enable all interrupts
    while (!stop) {
        //        moveSmallNose(smallNose, AA);
        //        moveBigNose(bigNose, C9);
        //    moveXMotors(int which);
        LATC = LATC | 0b011001111; // running all X motors for demonstration purpose
        simulateSort(&stop);

        //        AA = checkAA();
        //        C9 = checkC9V();
        //        sorted[0] += AA;
        //        sorted[1] += C9==1;
        //        sorted[2] += C9==2;
        //        sorted[3] += AA==0 && C9==0;
        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] == 15) {
            stop = 1;
        }
        if (getTime() - startTime >= 180) {
            stop = 1;
        }
    }
    //    di(); //disable all interrupts
    showInfo(-1);
    //    stopMoving();
    LATC = 0; // clear all LATC for demonstration purpose
    return;
}

int main(int argc, char** argv) {
    while (1) {
        initialize(); //Initiallize LCD and PORTs
        // Enter Standby mode
        println((unsigned char *) "Standby...");
        // put all noses to the uncharged bin position
        int bigNose = 0, smallNose = 0;
        //    moveBigNose(&bigNose,0);
        //    moveSmallNose(&smallNose,0);

        // once D is pressed enter mainloop();
        if (captureKeypad() == 15)mainloop(&bigNose, &smallNose);
    }
    return (EXIT_SUCCESS);
}

