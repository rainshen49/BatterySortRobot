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
    println(0,"Mainloop");
}

void simulate() {
    unsigned int now[] = {0, 0}, start[] = {0, 0};
    unsigned char sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    unsigned int period = 0;
    int stop = 0;
    int AA, C, V9;
    println(0,"Simulating...          ");
    start[0] = time[0]; //0 is seconds, 1 is hour
    start[1] = time[1];
    LATC5 = 1; // start DC
    while (!stop) {
        //        captureKeypad();
        AA = checkAA(sorted);
        //        captureKeypad();
        C = checkC(sorted);
        //        captureKeypad();
        V9 = check9(sorted);
        //        captureKeypad();

        int portCW[] = {0, 0, 0};
        int portCCW[] = {0, 0, 0};
        __lcd_clear();
        if (!AA) {
            portCCW[0] = 1;
            println(0,"AAServoLeft         ");
        } else if (AA == 1) {
            portCW[0] = 1;
            println(0,"AAServoRight         ");
        } else {
            println(0,"AAServoStays         ");
        }
        //        captureKeypad();

        if (!C) {
            portCCW[1] = 1;
            println(1,"CServoLeft         ");
        } else if (C == 1) {
            portCW[1] = 1;
            println(1,"CServoRight         ");
        } else {
            println(1,"CServoStays         ");
        }
        //        captureKeypad();

        if (!V9) {
            portCCW[2] = 1;
            println(0,"9ServoLeft         ");
        } else if (V9 == 1) {
            portCW[2] = 1;
            println(0,"9ServoRight         ");
        } else {
            println(0,"9ServoStays         ");
        }
        //        captureKeypad();

        CCW90(portCCW);
        CW90(portCW);
        LATC0 = 0;
        LATC1 = 0;
        LATC2 = 0;
        //        captureKeypad();

        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 10) {
            stop = 1;
        }
        getTime(time);
        now[0] = time[0]; //0 is seconds, 1 is hour
        now[1] = time[1];
        println(0,"start %02x:%02x     ", start[1], start[0]);
        println(1,"now %02x:%02x       ", now[1], now[0]);
        period = (HexDecToDec2(now[1]) - HexDecToDec2(start[1]))*60 + HexDecToDec2(now[0]) - HexDecToDec2(start[0]);
        println(1,"Period: %u      ", period);
        if (period > 20) {
            stop = 1;
        }
    }
    //    interfacing with EEPROM
    mode = 2;
    stopMoving(0);
    println(1,"Sorting Complete          ");
    showInfo(period, sorted);
    WriteRun(sorted, period);
    mode = 0;
}

void showRTC() {
    getTime(time);
    __lcd_home();
    printf("%02x/%02x/%02x         ", time[6], time[5], time[4]); //Print date in YY/MM/DD
    __lcd_newline();
    printf("%02x:%02x:%02x         ", time[2], time[1], time[0]); //HH:MM:SS
    __delay_ms(250);
}

void testADC() {
    unsigned char sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    while (1)checkAA(sorted);
}

void testMotor() {
    LATC5 = 1;
    pause();
}

void interrupt keypressed(void) {
    //    test case:
    //    when standby: start runing
    //    when running: emergency
    //    when result: show info (do nothing)
    //    when emergency: nothing
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

int main(int argc, char **argv) {
    initialize(); //Initiallize LCD and PORTs
    // Enter Standby mode
    println(1, "Welcome!          ");
    INT1IE = 1;
    ei(); //Enable all interrupts
    mode = 0;
    //        testADC();
    //    testMotor();
    while (1)showRTC();
    //    pause();
    RESET();
}