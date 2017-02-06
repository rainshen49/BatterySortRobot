#include <xc.h>
#include "configBits.h"
//#include "constants.h"
#include <stdio.h>

int AD(int pin){
    // detect the voltage on a pin and return the value if charged or uncharged
}

void println(char* data) {
    int i = 0;
    char output[] = "                                        ";
    while (data[i] != '\0') {
        output[i] = data[i++];
    }
    printf("%s", output);
}

unsigned int captureKeypad() {
    while (!PORTBbits.RB1);
    unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
    while (PORTBbits.RB1);
    return keypress;
}

void interrupt keypressed(void) {
    if (INT1IF) {
        unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
        INT1IF = 0; //Clear flag bit
        if (keypress == 15) {
            //            stopMoving();
        }
    }
}

void showInfo(unsigned int type) {
    //    di();
    while (1) {
        switch (type) {
            case(-1):
                println("Sorting Complete");
                println(" ");
                __delay_ms(1000);
                println("Press a number");
                println("For more info");
                break;
            case(0):
                println("Time Elapsed:");
                println("15.62s");
                break;
            case(1):
                println("Total Sorted:");
                println("15");
                break;
            case(2):
                println("AA Sorted:");
                println("5");
                break;
            case(3):
                println("C Sorted:");
                println("5");
                break;
            case(4):
                println("9V Sorted:");
                println("5");
                break;
            case(5):
                println("Uncharged:");
                println("0");
                break;
            case(15):
                RESET();
            default:
                break;
        }
        type = captureKeypad();
    }
    //    ei();
}

int getTime() {
    //    get time from RTC
    int time = 0;
    return time;
}

void moveXMotors(int which) {
    //    move the X motors by 90 degree
}

void moveBigNose(int * prev, int next) {
    //    move the big nose to the next location, 0 uncharged, 1 C, 2 9V
    //    determine how to move based on prev and next and then move the motors
    *prev = next;
}

void moveSmallNose(int * prev, int next) {
    //    move the big nose to the next location, 0 uncharged, 1AA
    //    determine how to move based on prev and next and then move the motors
    *prev = next;
}

void stopMoving() {
    di();
    println("Emergency");
    LATC = 0;
    while (captureKeypad() != 15);
    println("Resume");
    ei();
}

void checkAA() {
//    int charged = AD(0);
    //    if (charged) {
    //        return 1;
    //    }
    //    return 0;
}

void checkC9V() {
//    int Ccharged = AD(1);
//    int 9Vcharged = AD(2);    
    //    if (Ccharged) {
    //        return 1;
    //    } else if (9Vcharged) {
    //        return 2;
    //    }
    //    return 0;
}