#include <xc.h>
#include "configBits.h"
//#include "constants.h"
#include <stdio.h>
#include "modules.h"
#include "lcd.h"
#include "macros.h"


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

int checkV(int voltage, int max) {
    //    return 0 if not exist, 1 if exist and over 85 charged, -1 if exist but uncharged
    printf("%d received",voltage);
    __lcd_home();
    __delay_ms(1000);
    if (voltage > 1023 * 4 / 5) {
        return 0;
    } else if (voltage > 0.85 * 1023 * max / 5) {
        return 1;
    } else {
        return -1;
    }
}

int checkAA(int* sorted) {
    //    input RA0(AN0)
    println("AA:");
    __delay_ms(1000);
    switch (checkV(AD(0), 1.5)) {
        case 1:
            println("charged AA");
            __delay_ms(1000);
            sorted[0]++;
            return 1;
        case -1:
            println("uncharged AA");
            __delay_ms(1000);
            sorted[3]++;
            return 0;
        case 0:
            println("no AA");
            return -1;
        default:
            return 0;
    }
    __lcd_home();
}

int checkC9V(int* sorted) {
    //    output RE1-3 corresponds to R1,R2,R3&4
    //    input RA1(AN1),RA2(AN2),RA3(AN3)
    LATE = 0b1000;
    println("V3");
    switch (checkV(AD(3), 1.5)) {
        case 0:
            println("9:");
            __delay_ms(1000);
            //            its a 9v, not C, now checking 
            int i = 1;
            for (i; i < 3; i++) {
//          first check channel 1, then check channel 2
                printf("channel %d", i);
                __lcd_home();
                __delay_ms(900);
                LATE = i == 1 ? 0 : 0b0110;
                switch (checkV(AD(i), 3.6)) {
                    case 1:
                        println("charged 9V");
                        __delay_ms(1000);
                        sorted[2]++;
                        return 2;
                    case -1:
                        println("uncharged 9V");
                        __delay_ms(1000);
                        sorted[3]++;
                        return 0;
                    default:
                        println("no 9");
                        continue;
                }
            }
            return -1;
        case 1:
            //            its charged C
            println("charged C");
            __delay_ms(1000);
            sorted[1]++;
            return 1;
        case -1:
            println("uncharged C");
            __delay_ms(1000);
            sorted[3]++;
            return 0;
        default:
            return -1;
    }
}