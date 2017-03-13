#include "common.h"
#include "eeprom.h"
#include "modules.h"
// 0,1,2,3 latest = 3
// 4,1,2,3 latest = 0
// 4,5,2,3 latest = 1
unsigned char Latest = 0;

void initLatest() {
    Latest = ReadEE(160);
    if (Latest > 3) {
        Latest = 0;
    }
}

unsigned char ReadEE(unsigned int address) {

    // Set address registers
    EEADRH = (unsigned char) (address >> 8);
    EEADR = (unsigned char) address;

    EECON1bits.EEPGD = 0; // Select EEPROM Data Memory
    EECON1bits.CFGS = 0; // Access flash/EEPROM NOT config. registers
    EECON1bits.RD = 1; // Start a read cycle

    // A read should only take one cycle, and then the hardware will clear
    // the RD bit
    while (EECON1bits.RD == 1);

    return EEDATA; // Return data

}

void WriteEE(unsigned int address, unsigned char data) {
    // Set address registers
    EEADRH = (unsigned char) (address >> 8);
    EEADR = (unsigned char) address;

    EEDATA = data; // Write data we want to write to SFR
    EECON1bits.EEPGD = 0; // Select EEPROM data memory
    EECON1bits.CFGS = 0; // Access flash/EEPROM NOT config. registers
    EECON1bits.WREN = 1; // Enable writing of EEPROM (this is disabled again after the write completes)

    // The next three lines of code perform the required operations to
    // initiate a EEPROM write
    EECON2 = 0x55; // Part of required sequence for write to internal EEPROM
    EECON2 = 0xAA; // Part of required sequence for write to internal EEPROM
    EECON1bits.WR = 1; // Part of required sequence for write to internal EEPROM

    // Loop until write operation is complete
    while (!PIR2bits.EEIF);

    PIR2bits.EEIF = 0; //Clearing EEIF bit (this MUST be cleared in software after each write)
    EECON1bits.WREN = 0; // Disable write (for safety, it is re-enabled next time a EEPROM write is performed)
}

unsigned char back(unsigned char runs) {
    //    back 0 runs means to read latest
    unsigned char result = Latest + 4 - runs;
    if (result > 4) {
        result -= 4;
    }
    return result;
}

void ReadRun(unsigned char backruns, unsigned char * sorted, unsigned int * time) {
    unsigned int add = back(backruns) * 5;
    *time = ReadEE(add++ *8);
    sorted[0] = ReadEE(add++ *8);
    sorted[1] = ReadEE(add++ *8);
    sorted[2] = ReadEE(add++ *8);
    sorted[3] = ReadEE(add++ *8);
}

void WriteRun(unsigned char * sorted, unsigned int time) {
    unsigned int add = ++Latest;
    if (add > 3) {
        add -= 4;
        Latest = add;
    }
    add *= 5;
    WriteEE(add++ *8, time);
    WriteEE(add++ *8, sorted[0]);
    WriteEE(add++ *8, sorted[1]);
    WriteEE(add++ *8, sorted[2]);
    WriteEE(add++ *8, sorted[3]);
    WriteEE(160, Latest); // this place stores the Latest location of the eeprom
}

void PermLog(unsigned char backruns) {
    unsigned int time;
    unsigned char sorted[] = {0, 0, 0, 0};
    ReadRun(backruns, sorted, &time);
    line0();
    printf("showing %u      ", backruns);
    line1();
    printf("runs before     ");
    __delay_ms(700);
    showInfo(time, sorted);
}