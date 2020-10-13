#include "gd32vf103.h"
#include "gd32vf103_rcu.h"
#include "systick.h"
#include "lcd.h"
#include "lcd_test_functions.hpp"
#include "longan_nano_leds.h"
#include "one_wire.h"
#include "ds18b20.h"
#include <cstdint>

/*
 * Author:  Yago Teodoro de Mello
 * Date:    2020-10-13
 * License: MIT
 * V1.6.0
 * 
 */

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
    
    set_rgb(LED_RED);
    
    lcd::fill_rect(0, 80, 0, 160, LCD_DARKBLUE);
    
    //lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "LC-Display test ");
    //lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "Frequency AHB:  %7lukHz\nFrequency APB2: %7lukHz\n", rcu_clock_freq_get(CK_AHB)/1000, rcu_clock_freq_get(CK_APB2)/1000);
    lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "Frequency CKSYS:%7lukHz\n", rcu_clock_freq_get(CK_SYS)/1000);
    
    rcu_periph_clock_enable(RCU_GPIOB);
    uint64_t devices[8];
    
    one_wire_t wire(GPIOB, 
                    GPIO_PIN_6, 
                    8, 
                    devices);
    
    ds18b20_t ds[8];
    
    set_rgb(LED_YELLOW);
    
    wire.find_devices();
    
    set_rgb(LED_GREEN);
    
    uint8_t buffer[8][9];
    while(1){
        uint8_t ds_iterator = wire.get_device_count();
        while(ds_iterator--){
            wire.send(ds[ds_iterator].set_precision(11), 4, wire.get_id(ds_iterator));
        }
        
        lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "                ");
        lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\rReading");
        uint8_t point_count = 4;
        while(point_count--){
            delay_1ms(50);
            lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, ".");
        }
        lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\n");
        
        if(!wire.send_reset()){
            lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\bLine Error  \n");
        }
        
        ds_iterator = wire.get_device_count();
        while(ds_iterator--){
            wire.send(ds18b20_t::begin_conversion, 1, wire.get_id(ds_iterator));
        }
        
        ds[0].delay_conversion();
        
        ds_iterator = wire.get_device_count();
        while(ds_iterator--){
            wire.request(ds18b20_t::read_scratchpad, 1, buffer[ds_iterator], 9, wire.get_id(ds_iterator));
        }
        
        ds_iterator = wire.get_device_count();
        while(ds_iterator--){
            lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "Temp: %2.3f    \n[0]: %2hhx [1]: %2hhx ", ds[ds_iterator].get_temp(buffer[ds_iterator]), buffer[ds_iterator][0], buffer[ds_iterator][1]);
            lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "[2]: %2hhx\n[3]: %2hhx [4]: %2hhx [5]: %2hhx\n", buffer[ds_iterator][2], buffer[ds_iterator][3], buffer[ds_iterator][4], buffer[ds_iterator][5]);
            lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "[6]: %2hhx [7]: %2hhx [8]: %2hhx\n", buffer[ds_iterator][6], buffer[ds_iterator][7], buffer[ds_iterator][8]);
            lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\r%3hhu: %llx", ds_iterator, wire.get_id(ds_iterator));
            delay_1ms(1000);
            lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\b\b\b\b");
        }
        
        lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\n\n\n\n");
        wire.find_devices();
        lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "Found %3hhu devices.    ", wire.get_device_count());
        lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\b\b\b\b");
        
        delay_1ms(500);
        
        lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\b");
        
        delay_1ms(10);
    }
    
    return 0;
}
















// Lcd Test 





