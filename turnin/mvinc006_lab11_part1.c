/*	Author: Mark Alan Vincent II
 *  Partner(s) Name: NA
 *	Lab Section: A01
 *	Assignment: Lab #11  Exercise #1
 *	Youtube:  https://youtu.be/EzlpJXfqTLw
 *  Exercise Description:
 *
 *  I need to create a system that allows for the shifting of entire rows
 *  on the LED matrix in terms of 1 row of lit LEDs at a time. They cannnot
 *  go out of bounds.
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

// Method for handling writing to register.
void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
   	 PORTC = 0x08;
   	 // set SER = next bit of data to be sent.
   	 PORTC |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTC |= 0x02;
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTC |= 0x04;
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}

// globals (shared varibles for the SMs)
unsigned char tmpA = 0x00;
unsigned char row_level = 0;  // current row level on matrix
unsigned char row_val = 0x00; // (x, 7 - 0)
unsigned char row_sel = 0x00; // (y, 0 - 4)

// Flags for direction of shift.
enum shiftDirection { noDir, up, down } shiftDirection;

// Our concurrent synchSMs
enum SM1_States { SM1_Start, SM1_Off, SM1_b1, SM1_b2 } sm1_state;
int TickFct_Button(int state) {

    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;

    // Transitions
    switch(state) {

        case SM1_Start:
            // init SM (Should illuminate top row)
            row_level = 0;
            row_val = 0xFF;
            row_sel = 0x1E;
            shiftDirection = noDir;
            state = SM1_Off;
            break;

        case SM1_Off:
            // Check for new state
            if (tmpA == 0x00) {
                // Stay in SM1_Off
                state = SM1_Off;
            } else if (tmpA == 0x01) {
                // A0 activated
                state = SM1_b1;
            } else if (tmpA == 0x02) {
                // A1 activated
                state = SM1_b2;
            } else {
                // Bad input, do nothing.
                state = SM1_Off;
            }
            break;

        case SM1_b1:
            // Check for new state
            if (tmpA == 0x00) {
                // go to SM1_Off
                state = SM1_Off;
            } else if (tmpA == 0x01) {
                // A0 activated
                state = SM1_b1;
            } else if (tmpA == 0x02) {
                // A1 activated
                state = SM1_b2;
            } else {
                // Bad input, do nothing.
                state = SM1_Off;
            }
            break;

        case SM1_b2:
            // Check for new state
            if (tmpA == 0x00) {
                // go to SM1_Off
                state = SM1_Off;
            } else if (tmpA == 0x01) {
                // A0 activated
                state = SM1_b1;
            } else if (tmpA == 0x02) {
                // A1 activated
                state = SM1_b2;
            } else {
                // Bad input, do nothing.
                state = SM1_Off;
            }
            break;

        default:
            // Default
            state = SM1_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM1_Start:
            break;

        case SM1_Off:
            // dont move
            shiftDirection = noDir;
            break;

        case SM1_b1:
            // move up
            shiftDirection = up;
            break;

        case SM1_b2:
            // move down
            shiftDirection = down;
            break;

        default:
            break;

    }

    // End of Tick Function SM1
    return state;
}

enum SM2_States { SM2_Start, SM2_Shift } sm2_state;
int TickFct_Shift(int state) {

    // Transitions
    switch(state) {

        case SM2_Start:
            state = SM2_Shift;
            break;

        case SM2_Shift:
            // keep trying to shift
            state = SM2_Shift;
            break;

        default:
            // Default
            state = SM2_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM2_Start:
            break;

        case SM2_Shift:
            // check type of shift
            if (shiftDirection == noDir) {
              // no change
            } else if (shiftDirection == up) {
              // shift up
              if (row_level > 0) {
                  row_level = row_level - 1;
              }
            } else if (shiftDirection == down) {
              // shift down
              if (row_level < 4) {
                  row_level = row_level + 1;
              }
            } else {
              // no change
            }
            break;

        default:
            break;

    }

    // End of Tick Function SM1
    return state;
}

enum SM3_States { SM3_Start, SM3_Write } sm3_state;
int TickFct_Write(int state) {

    // Transitions
    switch(state) {

        case SM3_Start:
            // Go to write state
            state = SM3_Write;
            break;

        case SM3_Write:
            // Stay writing g
            state = SM3_Write;
            break;

        default:
            // Default
            state = SM3_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM3_Start:
            break;

        case SM3_Write:
            // Gather new position of LEDs from row_level
            switch (row_level) {
              case 0:
                  row_sel = 0x1E;
                  break;
              case 1:
                  row_sel = 0x1D;
                  break;
              case 2:
                  row_sel = 0x1B;
                  break;
              case 3:
                  row_sel = 0x17;
                  break;
              case 4:
                  row_sel = 0x0F;
                  break;
            }
            // Write to PORTC and PORTD using Globals
            PORTC = row_val;
            PORTD = row_sel;
            break;

        default:
            break;

    }

    // End of Tick Function SM3
    return state;
}

// Main Funcntion
int main(void) {

    // PORTS
    DDRA = 0x00; PORTA = 0xFF; // PortA as input
    DDRC = 0xFF; PORTC = 0x00; // PortC as output
    DDRD = 0xFF; PORTD = 0x00; // PortD as output

    // Setup Task List
    unsigned char i = 0;

    // SM1
    tasks[i].state = SM1_Start;
	  tasks[i].period = 50;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Button;
	  i++;

    // SM2
    tasks[i].state = SM1_Start;
	  tasks[i].period = 50;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Shift;
	  i++;

    // SM3
    tasks[i].state = SM1_Start;
	  tasks[i].period = 50;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Write;

    // Setup System Period & Timer to ON.
    TimerSet(tasksGCD);
    TimerOn();
    while (1) {}
    return 1;
}
