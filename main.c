/* 
 * File:   main.c
 * Author: Rain.S
 *
 * Created on February 3, 2017, 12:23 AM
 */

#include "common.h"
#include "modules.h"
#include "sensors.h"
#include "motors.h"
#include "eeprom.h"
#include "I2C.h"

// interface: 
// standby: ABC: adjust servo #: Permlog, D: Start
// permlog: 1234: runs, into log: 123456: info, * back
// running: not touchable
// show result: 123456: info, * back
int mode = 0; // 0 standby 1 running 2 result 3 emergency
unsigned char time[7];

void initialize() {
    TRISA = 0xFF; // Set Port A as all input
    TRISB = 0xFF; //All input mode
    TRISC = 0x00; //All output mode
    TRISD = 0x00;
    TRISE = 0x0F;
    //    config all ports input or output
    initLCD();

    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    //    initialize all pins to be low
    ADCON0 = 0x00; //Disable ADC
    ADCON1 = 0x09; //Set AN0-5 to be analog input
    CVRCON = 0x00; // Disable CCP reference voltage output
    ADFM = 1;
    //    set digital and analog
    __lcd_clear();
    //    __delay_ms(100);
    I2C_Master_Init(10000); //Initialize I2C Master with 100KHz clock
    //    __lcd_home();
    //    __delay_ms(50);
    initLatest();
    INTEDG2 = 0;
}

void mainloop() {
    line0();
    printf("Mainloop");
}

void simulate() {
    unsigned int now[] = {0, 0}, start[] = {0, 0};
    unsigned char sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    unsigned int period = 0;
    int AA, C, V9;
    unsigned char countDC = 0;
    line0();
    printf("Simulating...          ");
    start[0] = time[0]; //0 is seconds, 1 is hour
    start[1] = time[1];
    LATC6 = 1; // start DC
    INT2IE = 1; // enable the emergency stop
    INT2IF = 0;
    while (mode == 1) {
        if (countDC++ > 50) {
            LATC6 = 0;
            countDC = 0;
        }
        unsigned char DCmoving = LATC6;
        line0();
        LATC6 = 0;
        V9 = check9(sorted);
        C = checkC(sorted);
        AA = checkAA(sorted);
        printf("    ");

        int portCW[] = {0, 0, 0};
        int portCCW[] = {0, 0, 0};

        //        1 charged 0 uncharged -1 DNE
        line1();
        printf("9V:");
        if (!V9) {
            portCCW[0] = 1;
            printf(" -");

            captureKeypad();

        } else if (V9 == 1) {
            portCW[0] = 1;
            printf(" +");

            captureKeypad();

        } else {
            printf(" 0");
        }
        //        captureKeypad();
        printf(" C:");

        if (!C) {
            portCCW[1] = 1;
            printf(" -");

            captureKeypad();

        } else if (C == 1) {
            portCW[1] = 1;
            printf(" +");

            captureKeypad();

        } else {
            printf(" 0");
        }
        //        captureKeypad();
        printf(" AA:");

        if (!AA) {
            portCCW[2] = 1;
            printf(" -");

            captureKeypad();

        } else if (AA == 1) {
            portCW[2] = 1;
            printf(" +");

            captureKeypad();

        } else {
            printf(" 0");
        }
        //        captureKeypad();
        CCW90(portCCW);
        CW90(portCW);
        LATC6 = DCmoving;
        int portShake[];
        portShake[0] = portCCW[0] == portCW[0];
        portShake[1] = portCCW[1] == portCW[1];
        portShake[2] = portCCW[2] == portCW[2];
        shake(portShake);
        LATC0 = 0;
        LATC1 = 0;
        LATC2 = 0;
        //        captureKeypad();

        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 15) {
            //            mode = 2;
        }
        getTime(time);
        now[0] = time[0]; //0 is seconds, 1 is hour
        now[1] = time[1];
        line0();
        //        printf("%02x:%02x to %02x:%02x  ", start[1], start[0], now[1], now[0]);
        period = (HexDecToDec2(now[1]) - HexDecToDec2(start[1]))*60 + HexDecToDec2(now[0]) - HexDecToDec2(start[0]);
        line1();
        //        printf("Period: %u        ", period);
        if (period > 60) {
            //            mode = 2;
        }
        if (countDC == 25) {
            LATC6 = 1;
        }
    }
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
    di();
    if (INT1IF) {
        int port[] = {0, 0, 0};
        unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
        INT1IF = 0; //Clear flag bit
        while (PORTBbits.RB1); //wait for release
        switch (mode) {
            case 0:
                switch (keypress) {
                    case 3:
                        //                        move 9V servo a bit
                        port[2] = 1;
                        PWMC(400, 6, port);
                        break;
                    case 7:
                        //                        move C servo a bit
                        port[1] = 1;
                        PWMC(400, 6, port);
                        break;
                    case 8:
                        LATC6 = ~LATC6;
                        break;
                    case 11:
                        //                        move AA servo a bit
                        port[0] = 1;
                        line1();
                        PWMC(400, 6, port);
                        break;
                    case 12:
                        return;
                    case 14:
                        mode = 2;
                        PermLog();
                        mode = 0;
                        break;
                    case 15:
                        mode = 1;
                        ei();
                        simulate();
                        di();
                        break;
                }
                break;
            case 1:
                if (keypress == 13) {
                    mode = 2;
                    return;
                }
            default:
                break;
        }
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
        while (!PORTBbits.RB2); //wait for release
    }
    ei();
}

//void testServo() {
//    while (1) {
//        int port[] = {1, 1, 1};
//        CCW90(port);
//        __delay_ms(500);
//        CW90(port);
//        __delay_ms(500);
//    }
//}

//void testAD() {
//    unsigned char sorted[] = {0, 0, 0, 0};
//    line0();
//    printf("A %d,C %d,9 %d   ", AD(2), AD(0), AD(5));
//    __delay_ms(100);
//}

int main() {
    initialize(); //Initialize LCD and PORTs
    // Enter Standby mode
    line0();
    printf("Welcome!          ");
    //    pause();
    __delay_ms(500);
    //    while (1)testServo();
    //    testPWM();
    INT1IE = 1;
    INT1IF = 0;
    INT2IE = 0;
    ei(); //Enable all interrupts
    mode = 0;

    while (1)showRTC();
    RESET();
}