/*	Author: Mark Alan Vincent II
 *  Partner(s) Name: NA
 *	Lab Section: A01
 *	Assignment: Lab #11  Exercise #4
 *	Youtube:  https://youtu.be/2fJT-23mLr8
 *  Exercise Description:
 *
 *  I need to draw a 4x4 square on the LED matrix.
 *  And I need to allow it to move.
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
unsigned char row_level = 1;  // current row level on matrix
unsigned char col_level = 5;  // current col level on matrix
unsigned char row_val = 0x00; // (x, 7 - 0)
unsigned char row_sel = 0x00; // (y, 0 - 4)

// Pattern for square to be displayed
unsigned char row_valSquare[] = {0x00, 0x3C, 0x24, 0x24, 0x3C};
unsigned char row_selSquare[] = {0x1E, 0x1D, 0x1B, 0x17, 0x0F};

// Flags for direction of shift.
enum shiftDirection { noDir, up, down, left, right } shiftDirection;

// Our concurrent synchSMs
enum SM1_States { SM1_Start, SM1_Off, SM1_b1, SM1_b2, SM1_b3, SM1_b4 } sm1_state;
int TickFct_Button(int state) {

    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;

    // Transitions
    switch(state) {

        case SM1_Start:
            // init SM (Should illuminate top row)
            row_level = 1;
            col_level = 5;

            // For col shifting:
            // row_val = 0x01;
            // row_sel = 0x00;

            // For row shifting:
            // row_val = 0xFF;
            // row_sel = 0x1E;

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
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM1_b3;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM1_b4;
            } else {
                // Bad input, do nothing.
                state = SM1_Off;
            }
            break;

        case SM1_b1:
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
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM1_b3;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM1_b4;
            } else {
                // Bad input, do nothing.
                state = SM1_Off;
            }
            break;

        case SM1_b2:
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
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM1_b3;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM1_b4;
            } else {
                // Bad input, do nothing.
                state = SM1_Off;
            }
            break;

        case SM1_b3:
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
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM1_b3;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM1_b4;
            } else {
                // Bad input, do nothing.
                state = SM1_Off;
            }
            break;

        case SM1_b4:
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
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM1_b3;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM1_b4;
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

        case SM1_b3:
            // move right
            shiftDirection = right;
            break;

        case SM1_b4:
            // move left
            shiftDirection = left;
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
                  // subs 1 from row level and shifts square up 1 row.
                  row_level = row_level - 1;
                  // Shifts over y axis
                  unsigned char tmp = row_valSquare[0];
                  row_valSquare[0] = row_valSquare[1];
                  row_valSquare[1] = row_valSquare[2];
                  row_valSquare[2] = row_valSquare[3];
                  row_valSquare[3] = row_valSquare[4];
                  row_valSquare[4] = tmp;

              }

            } else if (shiftDirection == down) {
              // shift down
              if ( (row_level + 3) < 4) {
                  row_level = row_level + 1;
                  // Shifts over y axis
                  unsigned char tmp = row_valSquare[0];
                  unsigned char tmp1 = row_valSquare[1];
                  row_valSquare[0] = row_valSquare[4];
                  row_valSquare[1] = row_valSquare[3];
                  row_valSquare[2] = row_valSquare[2];
                  row_valSquare[3] = tmp1;
                  row_valSquare[4] = tmp;
              }

            } else if (shiftDirection == right) {
              // shift right
              if ( (col_level - 3) > 0) {
                  col_level = col_level - 1;
                  // Shifts over x axis
                  row_valSquare[0] = (row_valSquare[0] >> 1);
                  row_valSquare[1] = (row_valSquare[1] >> 1);
                  row_valSquare[2] = (row_valSquare[2] >> 1);
                  row_valSquare[3] = (row_valSquare[3] >> 1);
                  row_valSquare[4] = (row_valSquare[4] >> 1);
              }

            } else if (shiftDirection == left) {
              // shift left
              if (col_level < 7) {
                  col_level = col_level + 1;
                  row_valSquare[0] = (row_valSquare[0] << 1);
                  row_valSquare[1] = (row_valSquare[1] << 1);
                  row_valSquare[2] = (row_valSquare[2] << 1);
                  row_valSquare[3] = (row_valSquare[3] << 1);
                  row_valSquare[4] = (row_valSquare[4] << 1);
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
            // turn row_level off:
            row_level = 10;

            // turn col_level off:
            // col_level = 10;

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
              default:
                  break;
            }

            // Gather new position of LEDs from col_level
            switch (col_level) {
              case 0:
                  row_val = 0x01;
                  break;
              case 1:
                  row_val = 0x02;
                  break;
              case 2:
                  row_val = 0x04;
                  break;
              case 3:
                  row_val = 0x08;
                  break;
              case 4:
                  row_val = 0x10;
                  break;
              case 5:
                  row_val = 0x20;
                  break;
              case 6:
                  row_val = 0x40;
                  break;
              case 7:
                  row_val = 0x80;
                  break;
              default:
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

enum SM4_States { SM4_Start, SM4_Draw } sm4_state;
int TickFct_DrawSquare(int state) {

    // counter for current frame to draw (row in this case).
    static unsigned char drawFrame = 0;

    // Transitions
    switch(state) {

        case SM4_Start:
            // init vars
            drawFrame = 0;
            // Go to draw state
            state = SM4_Draw;
            break;

        case SM4_Draw:
            // Stay drawing g
            state = SM4_Draw;
            break;

        default:
            // Default
            state = SM4_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM4_Start:
            break;

        case SM4_Draw:
            //unsigned char row_valSquare[] = {0x00, 0x3C, 0x24, 0x24, 0x3C};
            //unsigned char row_selSquare[] = {0x1F, 0x1D, 0x1B, 0x17, 0x0F};
            if (drawFrame == 4) {
              // draw last frame and reset
              PORTC = row_valSquare[drawFrame];
              PORTD = row_selSquare[drawFrame];
              drawFrame = 0;
            } else {
              // draw and increment frame
              PORTC = row_valSquare[drawFrame];
              PORTD = row_selSquare[drawFrame];
              drawFrame = drawFrame + 1;
            }
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
	  tasks[i].period = 100;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Button;
	  i++;

    // SM2
    tasks[i].state = SM2_Start;
	  tasks[i].period = 100;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Shift;
	  i++;

    // SM4 (Draw Square)
    tasks[i].state = SM4_Start;
	  tasks[i].period = 1;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_DrawSquare;

    // Setup System Period & Timer to ON.
    TimerSet(tasksGCD);
    TimerOn();
    while (1) {}
    return 1;
}
