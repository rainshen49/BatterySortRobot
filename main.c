/* 
 * File:   main.c
 * Author: Rain.S
 *
 * Created on February 3, 2017, 12:23 AM
 */
//Meeting Goal
//see if PWM drives servo
//see if step() drives step
//figure out how to position steppers
//figure out how to position servos
//#include <stdio.h>
#include "modules.h"

void initialize()
{
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
}

void mainloop(int *bigNose, int *smallNose)
{
    __lcd_newline();
    printf("Sorting...          ");
    int stop = 0;
    int sorted[] = {0, 0, 0, 0}; // [AA,C,9V,uncharged]
    int startTime = getTime();
    LATC3 = 1; // start DC
    INT1IE = 1;
    ei(); //Enable all interrupts
    while (!stop)
    {
        moveSmallNose(smallNose, AA);
        moveBigNose(bigNose, C9);
        moveXMotors();
        //        LATC = LATC | 0b011001111; // running all X motors for demonstration purpose
        //        simulateSort(&stop);
        AA = checkAA(sorted);
        AA = (AA == -1) ? *smallNose : AA;
        C9 = checkC9V(sorted);
        C9 = (C9 == -1) ? *bigNose : C9;
        if (sorted[0] + sorted[1] + sorted[2] + sorted[3] >= 15)
        {
            stop = 1;
        }
        if (getTime() - startTime >= 180)
        {
            stop = 1;
        }
    }
    di(); //disable all interrupts
    //    interfacing with EEPROM
    //    showInfo(getTime() - startTime, sorted);
    stopMoving();
    return;
}

void simulate()
{
}

int main(int argc, char **argv)
{
    initialize(); //Initiallize LCD and PORTs
    // Enter Standby mode
    printf((unsigned char *)"Welcome!          ");
    // displays RTC
    __lcd_newline();
    printf("Press Start          ");
    // once D is pressed enter mainloop();
    while (captureKeypad() != 15)
    {
    };
    //    mainloop(&bigNose, &smallNose);
    simulate();
    RESET();
}
