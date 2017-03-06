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
    println(0,"Press a number          ");
    println(1,"For more info          ");
    int type = 0;
    int stop = 0;
    while (!stop) {
        type = captureKeypad();
        switch (type) {
            case (0):
                println(0,"Time Elapsed:          ");
                println(1,"%u seconds          ", time);
                break;
            case (1):
                println(0,"Total Sorted:          ");
                println(1,"%d batteries          ", sorted[0] + sorted[1] + sorted[2] + sorted[3]);
                break;
            case (2):
                println(0,"AA Sorted:          ");
                println(1,"%d             ", sorted[0]);
                break;
            case (3):
                println(0,"C Sorted:          ");
                println(1,"%d             ", sorted[1]);
                break;
            case (4):
                println(0,"9V Sorted:          ");
                println(1,"%d             ", sorted[2]);
                break;
            case (5):
                println(0,"Uncharged:          ");
                println(1,"%d             ", sorted[3]);
                break;
            case (15):
                RESET();
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
