/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


// using namespace std::chrono;


int freq = 1;
/////////////////////////////////////////////////
// Blinking rate in milliseconds
#define BLINKING_RATE     250ms

    // Initialise the digital pin LED1 as an output
#ifdef LED1
    DigitalOut led(LED1);
#else
    bool led;
#endif

    // Initialise the digital pin LED1 as an output
#ifdef BUTTON1
    InterruptIn button(BUTTON1);
#else
    bool button;
#endif
////////////////////////////////////////////////////*/

Ticker flipper;


void flip()
{
   led = !led;
}

void inter()
{

freq = freq + 1 ; 
if(freq > 6 )
 {freq = 1;}

}

void inter2()
{

flipper.attach(&flip, freq); // the address of the function to be attached (flip) and the interval (2 seconds)
}


int main()
{
    button.rise(&inter);
    button.fall(&inter2);
    flipper.attach(&flip, freq); // the address of the function to be attached (flip) and the interval (2 seconds)
    
    while (true) {
    printf("freq =  %d\n", freq);
    
    }
}
