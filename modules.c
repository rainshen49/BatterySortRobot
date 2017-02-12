#include <xc.h>
#include "configBits.h"
//#include "constants.h"
#include <stdio.h>
#include "modules.h"

int AD(char channel) {
    // Select A2D channel to read
    ADCON0 = ((channel << 2));
    ADON = 1;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO_NOT_DONE) {
        __delay_ms(10);
    }
    return (ADRESH << 8) | ADRESL;
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
    INT1IF = 0; //Clear flag bit
    return keypress;
}

void interrupt keypressed(void) {
//    printf("%d",INT1IF);
    if (INT1IF) {
        unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
        INT1IF = 0; //Clear flag bit
        stopMoving();
    }
}

void showInfo(unsigned int time, unsigned int * sorted) {
    di();
    println("Sorting Complete");
    println(" ");
    __delay_ms(1000);
    println("Press a number");
    println("For more info");
    int type = 0;
    while (1) {
        type = captureKeypad;
        switch (type) {
            case(0):
                println("Time Elapsed:");
                printf("%d seconds", time);
                break;
            case(1):
                println("Total Sorted:");
                printf("%d batteries", sorted[0] + sorted[1] + sorted[2] + sorted[3]);
                break;
            case(2):
                println("AA Sorted:");
                printf("%d", sorted[0]);
                break;
            case(3):
                println("C Sorted:");
                printf("%d", sorted[1]);
                break;
            case(4):
                println("9V Sorted:");
                printf("%d", sorted[2]);
                break;
            case(5):
                println("Uncharged:");
                printf("%d", sorted[3]);
                break;
            case(15):
                RESET();
            default:
                break;
        }
    }
    ei();
}

int getTime() {
    //    get time from RTC
    int time = 0;
    return time;
}

void moveXMotors() {
    //    move the X motors by 90 degree
}

void moveBigNose(int * prev, int next) {
    //    move the big nose to the next location, 0 uncharged, 1 C, 2 9V
    //    determine how to move based on prev and next and then move the motors
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
    println("Emergency");
    pause();
    //    ei();
}

int checkAA() {
    return AD(0); //check AA X if charged
}

int checkC9V() {
    int chargedC = AD(1);
    int charged9 = AD(2);
    if (chargedC) {
        return 1;
    } else if (charged9) {
        return 2;
    }
    return 0;
}