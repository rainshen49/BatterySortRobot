/* 
 * File:   newfile.h
 * Author: Rain.S
 *
 * Created on February 3, 2017, 4:52 PM
 */

#include <stdio.h>
#include <stdlib.h>
//#include <xc.h>
#include "pic18f4620.h"
#include "pic18.h"
#include "configBits.h"
#include "constants.h"
#include "lcd.h"
#include "macros.h"
#include "motors.h"
#include "sensors.h"

void showInfo(unsigned int time, unsigned int * sorted);
void interrupt keypressed(void);
unsigned int captureKeypad();
int getTime();
#define pause(); while(1);

