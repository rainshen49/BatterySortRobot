/* 
 * File:   eeprom.h
 * Author: Rain.S
 *
 * Created on February 26, 2017, 9:15 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

unsigned char ReadEE(unsigned int address);
void WriteEE(unsigned int address, unsigned char data);
void ReadRun(unsigned char backruns, unsigned char * sorted, unsigned int * time);
void WriteRun(unsigned char * sorted, unsigned int time);
unsigned char back(unsigned char runs);
void PermLog();
void initLatest();

#endif	/* EEPROM_H */

