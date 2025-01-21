/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


// Blinking rate in milliseconds
#define BLINKING_RATE     50ms


int main()
{
    // Initialise the digital pin LED1 as an output
#ifdef LED1
    DigitalOut led(LED1);
#else
    bool led;
#endif

    // Initialise the digital pin LED1 as an output
#ifdef BUTTON1
    DigitalIn button(BUTTON1);
#else
    bool button;
#endif

    while (true) {
        printf("etat du bouton est : %d \n", int(button));
        led = button;
        ThisThread::sleep_for(BLINKING_RATE);
    }
}
