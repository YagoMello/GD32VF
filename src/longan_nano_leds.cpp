#include "longan_nano_leds.h"

#include "gd32vf103.h"
#include "gd32vf103_rcu.h"


/*
 * Author:  Yago Teodoro de Mello
 * Date:    2020-10-11
 * License: MIT
 * V1.0.0
 * 
 */


void led_all_colors(){
    
    set_rgb(LED_BLACK);
    delay_1ms(250);
    
    set_rgb(LED_RED);
    delay_1ms(250);
    
    set_rgb(LED_YELLOW);
    delay_1ms(250);
    
    set_rgb(LED_GREEN);
    delay_1ms(250);
    
    set_rgb(LED_CYAN);
    delay_1ms(250);
    
    set_rgb(LED_BLUE);
    delay_1ms(250);

    set_rgb(LED_PURPLE);
    delay_1ms(250);
    
    set_rgb(LED_WHITE);
    delay_1ms(250);
    
    set_rgb(LED_BLACK);
    delay_1ms(250);
}

void init_led(){
    /* enable the LED clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure LED GPIO port */
    gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_1 | GPIO_PIN_2);
    gpio_init(GPIOC, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    
    gpio_bit_reset(GPIOA, GPIO_PIN_1 | GPIO_PIN_2);
    gpio_bit_reset(GPIOC, GPIO_PIN_13);
}

void set_red(uint_fast8_t value){
    if(value){
        gpio_bit_set(GPIOC, GPIO_PIN_13);
    }
    else{
        gpio_bit_reset(GPIOC, GPIO_PIN_13);
    }
}

void set_green(uint_fast8_t value){
    if(value){
        gpio_bit_set(GPIOA, GPIO_PIN_1);
    }
    else{
        gpio_bit_reset(GPIOA, GPIO_PIN_1);
    }
}

void set_blue(uint_fast8_t value){
    if(value){
        gpio_bit_set(GPIOA, GPIO_PIN_2);
    }
    else{
        gpio_bit_reset(GPIOA, GPIO_PIN_2);
    }
}

void set_rgb(uint_fast8_t red, uint_fast8_t green, uint_fast8_t blue){
    set_red(red);
    set_green(green);
    set_blue(blue);
} 
