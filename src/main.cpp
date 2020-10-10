/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32vf103.h"
#include "gd32vf103_rcu.h"
#include "systick.h"
#include "lcd.h"
#include <stdio.h>
#include <cstdint>

#define BLACK   1, 1, 1
#define RED     0, 1, 1
#define GREEN   1, 0, 1
#define BLUE    1, 1, 0
#define YELLOW  0, 0, 1
#define PURPLE  0, 1, 0
#define CYAN    1, 0, 0
#define WHITE   0, 0, 0

void init_led();
void set_red(uint_fast8_t value);
void set_green(uint_fast8_t value);
void set_blue(uint_fast8_t value);
void set_rgb(uint_fast8_t red, uint_fast8_t green, uint_fast8_t blue);
void led_all_colors();

void config_clk(){
    rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV8);
    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV8);

    //rcu_predv0_config(RCU_PREDV0SRC_HXTAL, RCU_PREDV0_DIV1);
    rcu_pll_config(RCU_PLLSRC_HXTAL, RCU_PLL_MUL10);
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
    //rcu_pll_config(RCU_PLLSRC_HXTAL, RCU_PLL_MUL12);
    
    //rcu_system_clock_source_config(RCU_CKSYSSRC_HXTAL);
    

}

int main(void){
    init_led();
    config_clk();
    
    // USING LONGAN NANO LED
    led_all_colors();
    
    lcd::init(80, 160, 26, 1);
    
    lcd::adjust(lcd::XY_EXCHANGE, 
                lcd::X_MIRROR, 
                lcd::Y_NORMAL, 
                lcd::COLOR_BGR);
    
    set_rgb(RED);
    
    lcd::fill_rect(0, 80, 0, 160, 000, 000, 120);
    
    lcd::lprintf(255, 255, 255, 000, 000, 120, "E nao eh que funciona\nHAHAHAHAH\n");
    lcd::lprintf(255, 255, 255, 000, 000, 120, "Frequencia ahb: %lukHz\nFreq. APB2:     %lukHz\n", rcu_clock_freq_get(CK_AHB)/1000, rcu_clock_freq_get(CK_APB2)/1000);
    
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x80);
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00);  // RED
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00);
    
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00);  // YELLOW
    
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0x80, 0xFF, 0x00);
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00);  // GREEN
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x80);
    
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF);  // CYAN
    
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0x00, 0x80, 0xFF);
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF);  // BLUE
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0xFF);
    
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF);  // MAGENTA

    
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
    lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    
    lcd::lputc('\n', 0xFF, 0xFF, 0xFF);

    
    set_rgb(GREEN);
    
    uint8_t iterator = 0;
    
    while(1){
        lcd::lprintf(0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80, "\rNumero: ");
        lcd::lprintf(0x9F, 0xFF, 0x9F, iterator, 0x40, 0x80, "%4hhu", iterator);
        iterator += 1;
        delay_1us(300);
    }
    
    return 0;
}

void led_all_colors(){
    
    set_rgb(BLACK);
    delay_1ms(250);
    
    set_rgb(RED);
    delay_1ms(250);
    
    set_rgb(YELLOW);
    delay_1ms(250);
    
    set_rgb(GREEN);
    delay_1ms(250);
    
    set_rgb(CYAN);
    delay_1ms(250);
    
    set_rgb(BLUE);
    delay_1ms(250);

    set_rgb(PURPLE);
    delay_1ms(250);
    
    set_rgb(WHITE);
    delay_1ms(250);
    
    set_rgb(BLACK);
    delay_1ms(250);
}

void init_led(){
    /* enable the LED clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure LED GPIO port */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1 | GPIO_PIN_2);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    
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
