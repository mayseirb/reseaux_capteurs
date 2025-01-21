/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


using namespace std::chrono;



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
////////////////////////////////////////////////////

Timer t;
int delta = 0;


void flip1()
{
   t.reset();
   t.start();
   led = !led;
}

void flip2()
{
   t.stop();
   led = !led;
   delta = duration_cast<milliseconds>(t.elapsed_time()).count();
  // printf("Durée de l'appui : %d \n", delta);
}

int main()
{
    button.rise(&flip1);  // attach the address of the flip function to the rising edge
    button.fall(&flip2);  // attach the address of the flip function to the rising edge

    while (true) {
    
    printf("Durée de l'appui : %d \n", delta);
    }
}
