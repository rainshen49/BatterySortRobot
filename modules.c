#include "common.h"
#include "modules.h"
#include "motors.h"
#include "I2C.h"

unsigned int HexDecToDec2(unsigned int hex) {
    //    converts fake Dec (infact hex) to actual dec
    unsigned tenth = hex / 16;
    unsigned digit = hex % 16;
    return tenth * 10 + digit;
}

unsigned int captureKeypad() {
    di();
    while (!PORTBbits.RB1);
    unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
    while (PORTBbits.RB1);
    INT1IF = 0; //Clear flag bit
    ei();
    return keypress;
}

void showInfo(unsigned int time, unsigned char *sorted) {
    line0();
    printf("Press a number          ");
    line1();
    printf("For more info          ");
    int type = 0;
    int stop = 0;
    while (!stop) {
        type = captureKeypad();
//      1:time,2:Total,3:AA,4:C,5:9V,6:uncharged
        switch (type) {
            case (0):
                line0();
                printf("Time Elapsed:          ");
                line1();
                printf("%u seconds          ", time);
                break;
            case (1):
                line0();
                printf("Total Sorted:          ");
                line1();
                printf("%u batteries          ", sorted[0] + sorted[1] + sorted[2] + sorted[3]);
                break;
            case (2):
                line0();
                printf("AA Sorted:          ");
                line1();
                printf("%u             ", sorted[0]);
                break;
            case (4):
                line0();
                printf("C Sorted:          ");
                line1();
                printf("%u             ", sorted[1]);
                break;
            case (5):
                line0();
                printf("9V Sorted:          ");
                line1();
                printf("%u             ", sorted[2]);
                break;
            case (6):
                line0();
                printf("Uncharged:          ");
                line1();
                printf("%u             ", sorted[3]);
                break;
            default:
                stop = 1;
        }
    }
}

void getTime(unsigned char * time) {
    di();
    //    get time from RTC

    //Reset RTC memory pointer 
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); //Set memory pointer to seconds
    I2C_Master_Stop(); //Stop condition

    //Read Current Time
    I2C_Master_Start();
    I2C_Master_Write(0b11010001); //7 bit RTC address + Read
    for (unsigned char i = 0; i < 0x06; i++) {
        time[i] = I2C_Master_Read(1);
    }
    time[6] = I2C_Master_Read(0); //Final Read without ack
    I2C_Master_Stop();
    ei();
}
