#include "gd32vf103.h"
#include "gd32vf103_rcu.h"
#include "systick.h"
#include "lcd.h"
#include <cstdint>

/*
 * Author:  Yago Teodoro de Mello
 * Date:    2020-10-10
 * License: MIT
 * V1.0.0
 * 
 */

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
void gamma_test_step();
void gamma_test_linear_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white, uint8_t step = 3, uint8_t count = 22);
void lcd_color_test();

void config_clk(){
    
    //rcu_pll_config(RCU_PLLSRC_HXTAL, RCU_PLL_MUL12);
    
    //rcu_system_clock_source_config(RCU_CKSYSSRC_HXTAL);
    rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV8);
    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV8);
    /*
    //rcu_predv0_config(RCU_PREDV0SRC_HXTAL, RCU_PREDV0_DIV1);
    rcu_predv0_config(RCU_PREDV0SRC_HXTAL, RCU_PREDV0_DIV2);
    rcu_osci_on(RCU_PLL_CK);
    rcu_pll_config(RCU_PLLSRC_HXTAL, RCU_PLL_MUL6);
    rcu_osci_stab_wait(RCU_PLL_CK);
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
    */
    rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV2);
}

int main(void){
    SystemInit();
    
    init_led();
    config_clk();
    
    // USING LONGAN NANO LED
    led_all_colors();
    
    lcd::load_defaults_longan();
    
    set_rgb(RED);
    
    lcd::fill_rect(0, 80, 0, 160, LCD_DARKBLUE);
    
    lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "LC-Display test ");
    if(rcu_system_clock_source_get() != RCU_SCSS_PLL)
        lcd::lprintf(LCD_RED, LCD_YELLOW, "CKSYS FAIL\n");
    else
        lcd::lprintf(LCD_WHITE, LCD_DARKGREEN, "CKSYS OK\n");
    
    lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "Frequency AHB:  %7lukHz\nFrequency APB2: %7lukHz\n", rcu_clock_freq_get(CK_AHB)/1000, rcu_clock_freq_get(CK_APB2)/1000);
    lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "Frequency CKSYS:%7lukHz\n", rcu_clock_freq_get(CK_SYS)/1000);
    
    lcd_color_test();
    
    gamma_test_linear_color(true, true, true, true);
    
    set_rgb(GREEN);
    
    while(1){
        gamma_test_step();
        
        delay_1ms(20);
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

void gamma_test_step(){
    static uint8_t iterator = 0;
    lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\rNumber: ");
    lcd::lprintf(0x9F, 0xFF, 0x9F, 4 * iterator, 0x40, 0x80, "%4hhu", iterator % 64);
        
    if(iterator % 64 == 0){
        lcd::send_gamset(iterator / 64);
        lcd::lprintf(0x7F, 0x7F, 0x7F, LCD_DARKBLUE, "   gamma: %3hhu", iterator / 64);
    }
    
    iterator += 1;
}

void gamma_test_linear_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white, uint8_t step, uint8_t count){
    uint8_t iterator;
    
    iterator = 0;
    if(red){
        lcd::lputs("R: ", LCD_WHITE, LCD_DARKBLUE);
        while(iterator < count){
            lcd::lputc(' ', 0xFF, 0xFF, 0xFF, (step * iterator) << 2, 0, 0);
            iterator++;
        }
        lcd::lputc('\n', 0xFF, 0xFF, 0xFF);
    }
    
    iterator = 0;
    if(green){
        lcd::lputs("G: ", LCD_WHITE, LCD_DARKBLUE);
        while(iterator < count){
            lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0, (step * iterator) << 2, 0);
            iterator++;
        }
        lcd::lputc('\n', 0xFF, 0xFF, 0xFF);
    }
    
    iterator = 0;
    if(blue){
        lcd::lputs("B: ", LCD_WHITE, LCD_DARKBLUE);
        while(iterator < count){
            lcd::lputc(' ', 0xFF, 0xFF, 0xFF, 0, 0, (step * iterator) << 2);
            iterator++;
        }
        lcd::lputc('\n', 0xFF, 0xFF, 0xFF);
    }
    
    iterator = 0;
    if(white){
        lcd::lputs("W: ", LCD_WHITE, LCD_DARKBLUE);
        while(iterator < count){
            lcd::lputc(' ', 0xFF, 0xFF, 0xFF, (step * iterator) << 2, (step * iterator) << 2, (step * iterator) << 2);
            iterator++;
        }
        lcd::lputc('\n', 0xFF, 0xFF, 0xFF);
    }
}

void lcd_color_test(){
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
    lcd::lputs(" \n", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
}




