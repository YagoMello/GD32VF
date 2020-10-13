 #ifndef LONGAN_NANO_LEDS_H
#define LONGAN_NANO_LEDS_H

#include "systick.h"


/*
 * Author:  Yago Teodoro de Mello
 * Date:    2020-10-11
 * License: MIT
 * V1.0.0
 * 
 */

#define LED_BLACK   1, 1, 1
#define LED_RED     0, 1, 1
#define LED_GREEN   1, 0, 1
#define LED_BLUE    1, 1, 0
#define LED_YELLOW  0, 0, 1
#define LED_PURPLE  0, 1, 0
#define LED_CYAN    1, 0, 0
#define LED_WHITE   0, 0, 0

void init_led();
void set_red(uint_fast8_t value);
void set_green(uint_fast8_t value);
void set_blue(uint_fast8_t value);
void set_rgb(uint_fast8_t red, uint_fast8_t green, uint_fast8_t blue);
void led_all_colors();

#endif // LONGAN_NANO_LEDS_H
