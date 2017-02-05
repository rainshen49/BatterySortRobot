/* 
 * File:   lcd.h
 * Author: Administrator
 *
 * Created on August 12, 2016, 4:24 PM
 */

#ifndef LCD_H
#define	LCD_H

void lcdInst(char data);
void lcdNibble(char data);
void initLCD(void);
void println(unsigned char* data);
void showInfo(unsigned int type);

#endif	/* LCD_H */

