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
    I2C_Master_Init(10000); //Initialize I2C Master with 100KHz clock
    initLatest();
    __delay_ms(200);
}

void mainloop(int *bigNose, int *smallNose) {
    unsigned int now, start;
    unsigned char sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    __lcd_newline();
    printf("Sorting...          ");
    int stop = 0;
    //    unsigned char * now = getTime();
    //    int startTime = now[1]*60 + now[0];
    //    unsigned char * runTime;
    int AA = -1, C = -1, V9 = -1;
    LATC5 = 1; // start DC
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
        //        runTime = getTime();
        //        if (runTime[1]*60 + runTime[0] - startTime >= 170) {
        //            stop = 1;
        //        }
    }
    di(); //disable all interrupts
    //    interfacing with EEPROM
    //    showInfo(getTime() - startTime, sorted);
    stopMoving(0);
    return;
}

void simulate() {
    unsigned int now[] = {0, 0}, start[] = {0, 0};
    unsigned char sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    unsigned int period = 0;
    start[0] = time[0]; //0 is seconds, 1 is hour
    start[1] = time[1];
    __lcd_home();
    printf("Simulating...          ");
    __delay_ms(700);
    int stop = 0;
    int AA = -1, C = -1, V9 = -1;
    LATC5 = 1; // start DC
    while (!stop) {
        //        captureKeypad();
        AA = checkAA(sorted);
        //        captureKeypad();
        __delay_ms(700);
        C = checkC(sorted);
        //        captureKeypad();
        __delay_ms(700);

        V9 = check9(sorted);
        __lcd_home();
        //        captureKeypad();

        int portCW[] = {0, 0, 0};
        int portCCW[] = {0, 0, 0};

        if (!AA) {
            portCCW[0] = 1;
            printf("AAServoLeft         ");
        } else if (AA == 1) {
            portCW[0] = 1;
            printf("AAServoRight         ");
        } else {
            printf("AAServoStays         ");
        }
        //        captureKeypad();
        __lcd_home();
        __delay_ms(700);

        if (!C) {
            portCCW[1] = 1;
            printf("CServoLeft         ");
        } else if (C == 1) {
            portCW[1] = 1;
            printf("CServoRight         ");
        } else {
            printf("CServoStays         ");
        }
        //        captureKeypad();
        __lcd_home();
        __delay_ms(700);

        if (!V9) {
            portCCW[2] = 1;
            printf("9ServoLeft         ");
        } else if (V9 == 1) {
            portCW[2] = 1;
            printf("9ServoRight         ");
        } else {
            printf("9ServoStays         ");
        }
        //        captureKeypad();

        CCW90(portCCW);
        __delay_ms(700);
        CW90(portCW);
        LATC0 = 0;
        LATC1 = 0;
        LATC2 = 0;
        //        captureKeypad();

        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 5) {
            stop = 1;
        }
        getTime(time);
        now[0] = time[0]; //0 is seconds, 1 is hour
        now[1] = time[1];
        __lcd_home();
        printf("start %02x:%02x    ", start[1], start[0]);
        __lcd_newline();
        printf("now %02x:%02x     ", now[1], now[0]);
        __delay_ms(700);
        //        captureKeypad();
        period = (now[1] - start[1])*60 + now[0] - start[0];
        __lcd_home();
        printf("Period: %u", period);
        if (period > 60) {
            stop = 1;
        }
    }
    //    interfacing with EEPROM
    mode = 2;
    stopMoving(0);
    __lcd_home();
    printf("Sorting Complete          ");
    __delay_ms(700);
    showInfo(period, sorted);
    WriteRun(sorted, period);
    mode = 0;
}

//void testPWM() {
//    __lcd_home();
//    printf("Width:700us");
//    int port[] = {1, 1, 1};
//    for (int i = 0; i < 4; i++) {
//        CCW90(port);
//        __delay_1s();
//    }
//}

//void testADC() {
//    while (1) {
//        int AA = checkAA(sorted);
//        //        printf("AA: %d",AA);
//        __delay_ms(700);
//    }
//}

//void testEEPROM() {
//    unsigned int add = 0;
//    for (unsigned char i = 0; i < 16; i++) {
//        WriteEE(add * 8, i);
//        add++;
//    }
//    while(1){
//        unsigned int key = captureKeypad();
//        __lcd_home();
//        unsigned char result = ReadEE(key*8);
//        printf("data: %u",result);
//    }
//}

void showRTC() {

    getTime(time);
    __lcd_home();
    printf("%02x/%02x/%02x         ", time[6], time[5], time[4]); //Print date in YY/MM/DD
    __lcd_newline();
    printf("%02x:%02x:%02x         ", time[2], time[1], time[0]); //HH:MM:SS
    __delay_ms(250);
}

int main(int argc, char **argv) {
    initialize(); //Initiallize LCD and PORTs
    // Enter Standby mode
    printf((unsigned char *) "Welcome!          ");
    __delay_ms(700);
    INT1IE = 1;
    ei(); //Enable all interrupts
    mode = 0;

    while (1)showRTC();
    //    pause();
    //    mainloop(&bigNose, &smallNose);
    //    testPWM();
    //    testADC();
    //    testRTC();
    //    testEEPROM();
    RESET();
}

void interrupt keypressed(void) {
    //    test case:
    //    when standby: start runing
    //    when running: emergency
    //    when result: show info
    //    when emergency: nothing
    //        printf("%d",INT1IF);
    di();
    if (INT1IF) {
        unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
        INT1IF = 0; //Clear flag bit
        while (PORTBbits.RB1); //wait for release
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
            case 1:
                mode = 3;
                stopMoving(1);
                break;
            default:
                break;
        }
    }
    ei();
}