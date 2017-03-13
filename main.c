/* 
 * File:   main.c
 * Author: Rain.S
 *
 * Created on February 3, 2017, 12:23 AM
 */
//#include <stdio.h>
#include "common.h"
#include "modules.h"
#include "sensors.h"
#include "motors.h"
#include "eeprom.h"
#include "I2C.h"

int mode = 0; // 0 standby 1 running 2 result 3 emergency
unsigned char time[7];

void initialize() {
    TRISA = 0xFF; // Set Port A as all input
    TRISB = 0xFF; //All input mode
    TRISC = 0x00; //All output mode
    TRISD = 0x00;
    TRISE = 0x00;
    //    config all ports input or output
    RBPU = 0;
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
    __delay_ms(50);
    I2C_Master_Init(10000); //Initialize I2C Master with 100KHz clock
    //    __lcd_home();
    __delay_ms(50);
    initLatest();
}

void mainloop(int *bigNose, int *smallNose) {
    line0();
    printf("Mainloop");
}

void simulate() {
    unsigned int now[] = {0, 0}, start[] = {0, 0};
    unsigned char sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    unsigned int period = 0;
    int stop = 0;
    int AA, C, V9;
    line0();
    printf("Simulating...          ");
    start[0] = time[0]; //0 is seconds, 1 is hour
    start[1] = time[1];
    LATC5 = 1; // start DC
    INT2IE = 1; // enable the emergency stop
    INT2IF = 0;
    while (!stop) {
        //        captureKeypad();
        line0();
        AA = checkAA(sorted);
        //        captureKeypad();
        C = checkC(sorted);
        //        captureKeypad();
        V9 = check9(sorted);
        printf("    ");

        //        captureKeypad();
        int portCW[] = {0, 0, 0};
        int portCCW[] = {0, 0, 0};

        line1();
        printf("AA:");
        if (!AA) {
            portCCW[0] = 1;
            printf(" -");
        } else if (AA == 1) {
            portCW[0] = 1;
            printf(" +");
        } else {
            printf(" 0");
        }
        //        captureKeypad();
        printf(" C:");

        if (!C) {
            portCCW[1] = 1;
            printf(" -");
        } else if (C == 1) {
            portCW[1] = 1;
            printf(" +");
        } else {
            printf(" 0");
        }
        //        captureKeypad();
        printf(" 9V:");

        if (!V9) {
            portCCW[2] = 1;
            printf(" -");

        } else if (V9 == 1) {
            portCW[2] = 1;
            printf(" +");
        } else {
            printf(" 0");
        }
        //        captureKeypad();

        CCW90(portCCW);
        CW90(portCW);
        LATC0 = 0;
        LATC1 = 0;
        LATC2 = 0;
        //        captureKeypad();

        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 15) {
            //            stop = 1;
        }
        getTime(time);
        now[0] = time[0]; //0 is seconds, 1 is hour
        now[1] = time[1];
        line0();
        printf("%02x:%02x to %02x:%02x  ", start[1], start[0], now[1], now[0]);
        period = (HexDecToDec2(now[1]) - HexDecToDec2(start[1]))*60 + HexDecToDec2(now[0]) - HexDecToDec2(start[0]);
        line1();
        printf("Period: %u        ", period);
        if (period > 20) {
            //            stop = 1;
        }
        //        captureKeypad();
    }
    //    interfacing with EEPROM
    mode = 2;
    INT2IE = 0;
    stopMoving(0);
    line1();
    printf("Sorting Complete          ");
    WriteRun(sorted, period);
    __delay_ms(700);
    showInfo(period, sorted);
    mode = 0;
}

void showRTC() {
    getTime(time);
    line0();
    printf("    %02x/%02x/%02x      ", time[6], time[5], time[4]); //Print date in YY/MM/DD
    line1();
    printf("    %02x:%02x:%02x      ", time[2], time[1], time[0]); //HH:MM:SS
    __delay_ms(250);
}

void interrupt intrpt(void) {
    //    test case:
    //    when standby: start runing
    //    when running: emergency
    //    when result: show info (do nothing)
    //    when emergency: nothing
    di();
    if (INT1IF) {
        unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
        INT1IF = 0; //Clear flag bit
        switch (mode) {
            case 0:
                if (keypress < 4) {
                    mode = 2;
                    PermLog(keypress);
                    mode = 0;
                } else {
                    mode = 1;
                    ei();
                    simulate();
                    di();
                }
                break;
            default:
                break;
        }
        while (PORTBbits.RB1); //wait for release
    }
    if (INT2IF) {
        INT2IF = 0; //Clear flag bit
        switch (mode) {
            case 1:
                mode = 3;
                stopMoving(1);
                break;
            default:
                break;
        }
        while (PORTBbits.RB2); //wait for release
    }
    ei();
}

//void testPWM() {
//    while (1) {
//        int portCW[] = {1, 0, 0};
//        CCW90(portCW);
//        captureKeypad();
//    }
//}

int main() {
    initialize(); //Initialize LCD and PORTs
    // Enter Standby mode
    line0();
    printf("Welcome!          ");
    //    pause();
    __delay_ms(500);
    //    testPWM();
    INT1IE = 1;
    INT1IF = 0;
    INT2IE = 0;
    ei(); //Enable all interrupts
    mode = 0;

    while (1)showRTC();
    RESET();
}