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
#define debug 0;

//calibrate:
//DC Gate, DC spinning pulse, Servo
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
    LATB2 = 1;
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
    unsigned int now[] = {0, 0}, start[] = {0, 0};
    unsigned char sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    unsigned int period = 0;
    unsigned char shakeCW = 0;
    int AA, C, V9;
    //    unsigned char countDC = 0;
    line0();
    printf("Sorting...          ");
    start[0] = time[0]; //0 is seconds, 1 is hour
    start[1] = time[1];
    //    gate is initially closed
    LATC6 = 1; // start DC
    INT2IE = 1; // enable the emergency stop
    INT2IF = 0;
    //    __delay_ms(20000);
    openGate();
    char notyet = 0;
    while (mode == 1) {
        line0();
        V9 = check9(sorted);
        C = checkC(sorted);
        AA = checkAA(sorted);
        printf("    ");
        if (V9 != -1 || C != -1 || AA != -1) {
            notyet = 0;
        }
        //        1 charged 0 uncharged -1 DNE
        line1();
        printf("9V:");
        if (!V9) {
            printf(" +");
            AA = -1;
            V9Spin(V9);

            debug;
        } else if (V9 == 1) {
            printf(" -");
            AA = -1;
            V9Spin(V9);

            debug;
        } else {
            printf(" 0");
        }
        //        debug;
        printf(" C:");

        if (!C) {
            printf(" +");
            CSpin(C);

            debug;
        } else if (C == 1) {
            printf(" -");
            CSpin(C);

            debug;
        } else {
            printf(" 0");
        }
        //        debug;
        printf(" AA:");

        if (!AA) {
            printf(" -");
            debug;
            AASpin(AA);

        } else if (AA == 1) {
            printf(" +");
            AASpin(AA);

            debug;
        } else {
            printf(" 0");
        }
        //        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 15) {
        if (sorted[0] > 0 && sorted[1] > 0 && sorted[2] > 0 && sorted[3] > 0) {
            mode = 2;
        }
        getTime(time);
        now[0] = time[0]; //0 is seconds, 1 is hour
        now[1] = time[1];
        period = (HexDecToDec2(now[1]) - HexDecToDec2(start[1]))*60 + HexDecToDec2(now[0]) - HexDecToDec2(start[0]);
        if (period > 150) {
            mode = 2;
        }
        notyet++;
        if (notyet > 200) {
//            mode = 2;
        }
    }
    INT2IE = 0;
    stopMoving(0);
    closeGate();
    line1();
    printf("Sorting Complete          ");
    WriteRun(sorted, period);
    __delay_ms(700);
    showInfo(period, sorted);
    mode = 0;
}

void simulate() {
    unsigned int now[] = {0, 0}, start[] = {0, 0};
    unsigned char sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    unsigned int period = 0;
    unsigned char shakeCW = 0;
    int AA, C, V9;
    //    unsigned char countDC = 0;
    line0();
    printf("Sorting...          ");
    start[0] = time[0]; //0 is seconds, 1 is hour
    start[1] = time[1];
    //    gate is initially closed
    LATC6 = 1; // start DC
    INT2IE = 1; // enable the emergency stop
    INT2IF = 0;
    __delay_ms(20000);
    openGate();
    char notyet = 0;
    while (mode == 1) {
        //        9V affect AA empty
        LATC6 = 0;
        line0();
        V9 = check9(sorted);
        C = checkC(sorted);
        AA = checkAA(sorted);
        printf("    ");
        if (V9 != -1 || C != -1 || AA != -1) {
            notyet = 0;
        }

        //        1 charged 0 uncharged -1 DNE
        line1();
        printf("9V:");
        if (!V9) {
            printf(" +");
            AA = -1;
            debug;
        } else if (V9 == 1) {
            printf(" -");
            AA = -1;
            debug;
        } else {
            printf(" 0");
        }
        V9Spin(V9);
        //        debug;
        printf(" C:");

        if (!C) {
            printf(" +");
            debug;
        } else if (C == 1) {
            printf(" -");
            debug;
        } else {
            printf(" 0");
        }
        CSpin(C);
        //        debug;
        printf(" AA:");

        if (!AA) {
            printf(" -");
            debug;
        } else if (AA == 1) {
            printf(" +");
            debug;
        } else {
            printf(" 0");
        }
        AASpin(AA);
        LATC6 = 1;
        //        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 15) {
        if (sorted[0] > 0 && sorted[1] > 0 && sorted[2] > 0 && sorted[3] > 0) {
            mode = 2;
        }
        getTime(time);
        now[0] = time[0]; //0 is seconds, 1 is hour
        now[1] = time[1];
        period = (HexDecToDec2(now[1]) - HexDecToDec2(start[1]))*60 + HexDecToDec2(now[0]) - HexDecToDec2(start[0]);
        if (period > 150) {
            mode = 2;
        }
        notyet++;
        if (notyet > 200) {
            mode = 2;
        }
    }
    INT2IE = 0;
    stopMoving(0);
    closeGate();
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

void testServo() {
    while (1) {
        V9Spin(1);
        AASpin(1);
        CSpin(1);
        __delay_ms(200);
        //        V9Spin(0);
        //        AASpin(0);
        //        CSpin(0);
    }
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
                    case 0:
                    {
                        testServo();
                        break;
                    }
                    case 3:
                        //                        move 9V servo a bit
                        port[2] = 1;
                        PWMC(3000, 30, port);
                        PWMC(400, 2, port);
                        break;
                    case 7:
                        //                        move C servo a bit
                        port[1] = 1;
                        PWMC(3000, 30, port);
                        PWMC(400, 2, port);
                        break;
                    case 8:
                        LATC6 = ~LATC6;
                        break;
                    case 9:
                        LATC7 = 0;
                        LATC5 = 1;
                        __delay_ms(180);
                        LATC5 = 0;
                        break;
                    case 10:
                        LATC5 = 0;
                        LATC7 = 1;
                        __delay_ms(100);
                        LATC7 = 0;
                        break;
                    case 11:
                        port[0] = 1;
                        line1();
                        PWMC(3000, 30, port);
                        PWMC(400, 2, port);
                        break;
                    case 12:
                        return;
                    case 13:
                        mode = 1;
                        ei();
                        mainloop();
                        di();
                        break;
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
            default:
                break;
        }
    }
    if (INT2IF) {
        INT2IF = 0; //Clear flag bit
        __delay_ms(100);
        if (!PORTBbits.RB2) {
            switch (mode) {
                case 1:
                    mode = 3;
                    stopMoving(1);
                    break;
                default:
                    break;
            }
        }
    }
    ei();
}

int main() {
    initialize(); //Initialize LCD and PORTs
    // Enter Standby mode
    line0();
    printf("Welcome!          ");
    //    pause();
    __delay_ms(500);
    //    while (1)testAD();
    //    testPWM();
    INT1IE = 1;
    INT1IF = 0;
    INT2IE = 0;
    ei(); //Enable all interrupts
    mode = 0;

    while (1)showRTC();
    RESET();
}