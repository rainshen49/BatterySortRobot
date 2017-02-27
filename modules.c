#include "common.h"
#include "modules.h"
#include "motors.h"
#include "I2C.h"

unsigned int captureKeypad() {
    di();
    while (!PORTBbits.RB1)
        ;
    unsigned char keypress = (PORTB & 0xF0) >> 4; // Read the 4 bit character code
    while (PORTBbits.RB1)
        ;
    INT1IF = 0; //Clear flag bit
    ei();
    return keypress;
}

void showInfo(unsigned int time, unsigned char *sorted) {
    __lcd_home();
    printf("Press a number          ");
    __lcd_newline();
    printf("For more info          ");
    int type = 0;
    int stop = 0;
    while (!stop) {
        type = captureKeypad();
        switch (type) {
            case (0):
                __lcd_home();
                printf("Time Elapsed:          ");
                __lcd_newline();
                printf("%u seconds          ", time);
                break;
            case (1):
                __lcd_home();
                printf("Total Sorted:          ");
                __lcd_newline();
                printf("%d batteries          ", sorted[0] + sorted[1] + sorted[2] + sorted[3]);
                break;
            case (2):
                __lcd_home();
                printf("AA Sorted:          ");
                __lcd_newline();
                printf("%d             ", sorted[0]);
                break;
            case (3):
                __lcd_home();
                printf("C Sorted:          ");
                __lcd_newline();
                printf("%d             ", sorted[1]);
                break;
            case (4):
                __lcd_home();
                printf("9V Sorted:          ");
                __lcd_newline();
                printf("%d             ", sorted[2]);
                break;
            case (5):
                __lcd_home();
                printf("Uncharged:          ");
                __lcd_newline();
                printf("%d             ", sorted[3]);
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
