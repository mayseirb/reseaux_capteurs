/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


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

void flip()
{
   //printf("etat du bouton est : %d \n", int(button));
   led = !led;
}

int main()
{
    button.rise(&flip);  // attach the address of the flip function to the rising edge
    button.fall(&flip);  // attach the address of the flip function to the rising edge

    while (true) {
        
    }
}
