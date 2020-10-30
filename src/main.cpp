#include "gd32vf103.h"
#include "gd32vf103_rcu.h"
#include "systick.h"
#include "lcd.h"
#include "lcd_test_functions.hpp"
#include "longan_nano_leds.h"
#include "one_wire.h"
#include "ds18b20.h"
#include "servo_test.h"
#include "schv4.h"
#include <cstdint>
#include <array>

/*
 * Author:  Yago Teodoro de Mello
 * Date:    2020-10-29
 * License: MIT
 * V1.8.0
 * 
 */

#define ROW_AHB         0,  0
#define ROW_APB1        1,  0
#define ROW_CKSYS       2,  0
#define ROW_READING     3,  0
#define ROW_LINE_ERROR  3, 14
#define ROW_TEMP        4,  0
#define ROW_DEVICE_ID   5,  5
#define ROW_DEVICES     5,  0
#define ROW_TIMERS      7,  0

/*
namespace cst {
    template <typename T, size_t N>
    class array {
        T values[N];
    public:
        T & operator[] (size_t index){
            return values[index];
        }
        T * data(){
            return values;
        }
    };
}
*/
double get_pwm_frequency();
double get_pwm_duty();

struct read_ds_data_t {
    one_wire_t * wireptr;
    ds18b20_t  * ds;
    std::array<std::array<uint8_t, 9>, 8> buf;
    uint8_t    print_counter;
};

void ds_print_value(proc_t * this_proc, void * payload){
    one_wire_t * wire      = reinterpret_cast<read_ds_data_t *>(payload)->wireptr;
    ds18b20_t  * ds        = reinterpret_cast<read_ds_data_t *>(payload)->ds;
    std::array<std::array<uint8_t, 9>, 8> * buffer = &reinterpret_cast<read_ds_data_t *>(payload)->buf;
    uint8_t    * printctr = &reinterpret_cast<read_ds_data_t *>(payload)->print_counter;
    
    if(*printctr){
        (*printctr)--;
        lcd::lpprintf(ROW_TEMP, LCD_WHITE, LCD_DARKBLUE, "Temp: %2.3f%cC  ", ds[*printctr].get_temp(buffer[0][*printctr].data()), char(248));
        lcd::lpprintf(ROW_DEVICE_ID, LCD_WHITE, LCD_DARKBLUE, "%3hhu] %llx", *printctr + 1, wire->get_id(*printctr));
        if(printctr){
            sch::proc_add(this_proc);
        }
    }
}

void ds_read_value(proc_t *, void * payload){
    one_wire_t * wire      = reinterpret_cast<read_ds_data_t *>(payload)->wireptr;
    ds18b20_t  * ds        = reinterpret_cast<read_ds_data_t *>(payload)->ds;
    std::array<std::array<uint8_t, 9>, 8> * buffer = &reinterpret_cast<read_ds_data_t *>(payload)->buf;
    uint8_t    * printctr = &reinterpret_cast<read_ds_data_t *>(payload)->print_counter;
    
    static proc_t ds_print(ds_print_value, 
                           sch::ticks_from_us(800'000), // BUG 800'000+ crashes the program
                           proc_t::SINGLE,
                           payload);
    
    *printctr = wire->get_device_count();
    
    uint8_t ds_iterator = wire->get_device_count();
    while(ds_iterator--){
        wire->request(ds18b20_t::read_scratchpad, 1, buffer[0][ds_iterator].data(), 9, wire->get_id(ds_iterator));
    }
    
    sch::proc_add(&ds_print);
}

void ds_begin_conversion(proc_t *, void * payload){
    one_wire_t * wire   = reinterpret_cast<read_ds_data_t *>(payload)->wireptr;
    ds18b20_t  * ds     = reinterpret_cast<read_ds_data_t *>(payload)->ds;
    std::array<std::array<uint8_t, 9>, 8> * buffer = &reinterpret_cast<read_ds_data_t *>(payload)->buf;
    uint8_t    * printctr = &reinterpret_cast<read_ds_data_t *>(payload)->print_counter;
    
    if(!*printctr){
        uint8_t ds_iterator = wire->get_device_count();
        while(ds_iterator--){
            wire->send(ds[ds_iterator].set_precision(9), 4, wire->get_id(ds_iterator));
        }
        
        lcd::clear_line(ROW_READING, LCD_DARKBLUE);
        lcd::lpprintf(ROW_READING, LCD_WHITE, LCD_DARKBLUE, "Reading");
        
        static proc_t ds_read(ds_read_value, 
                              sch::ticks_from_us(1000 * ds[0].delay_value_milisseconds()),
                              proc_t::SINGLE,
                              payload);
        
        if(!wire->send_reset()){
            lcd::lpprintf(ROW_LINE_ERROR, LCD_WHITE, LCD_DARKBLUE, "Line Error  ");
        }
        
        ds_iterator = wire->get_device_count();
        while(ds_iterator--){
            wire->send(ds18b20_t::begin_conversion, 1, wire->get_id(ds_iterator));
        }
        
        sch::proc_add(&ds_read);
    }
}

void find_devices_f(proc_t *, void * wire){
    reinterpret_cast<one_wire_t *>(wire)->find_devices();
    lcd::lpprintf(ROW_DEVICES, LCD_WHITE, LCD_DARKBLUE, "[%3hhu:", reinterpret_cast<one_wire_t *>(wire)->get_device_count());
}

class led_blink_t : public exec_base_t {
    uint8_t state = 0b111;
public:
    virtual void func(proc_t *) override {
        set_rgb(state & 0b001, 
                state & 0b010, 
                state & 0b100);
        
        state--;
        if(state == 0b000){
            state = 0b111;
        }
    }
};

void led_blink_f(proc_t * this_proc, void *){
    static uint8_t max = 21;
    static uint8_t state = 0b111;
    
    set_rgb(state & 0b001, 
            state & 0b010, 
            state & 0b100);
    
    state--;
    if(state == 0b000){
        state = 0b111;
    }
    
    lcd::lpprintf(1, 0, LCD_WHITE, LCD_DARKBLUE, "Led remaining: %2hhu", max);
    if(max == 0){
        sch::proc_kill(this_proc);
    }
    max--;
}

void get_cksys(proc_t *, void *){
    lcd::clear_line(ROW_CKSYS, LCD_DARKBLUE);
    lcd::lpprintf(ROW_CKSYS, LCD_WHITE, LCD_DARKBLUE, "Frequency CKSYS:%7lukHz", rcu_clock_freq_get(CK_SYS)/1000);
}

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
    rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
}
/*
void sch::idle(){
    //lcd::lpprintf(ROW_TIMERS, LCD_WHITE, LCD_DARKBLUE, "Timer value: %5hu [%llu]\nB: %llu", timer_counter_read(TIMER1), sch::proc_count(), get_timer_value());
}
*/
void pulse(proc_t *, void * data){
    uint8_t * color = reinterpret_cast<uint8_t *>(data);
    set_rgb(color[0], color[1], color[2]);
    delay_1ms(100);
    set_rgb(LED_BLACK);
    delay_1ms(50);
}

void increment(proc_t *, void * data_raw){
    uint32_t * data = reinterpret_cast<uint32_t *>(data_raw);
    (*data)++;
}

void print5(proc_t *, void * data_raw){
    uint32_t * data = reinterpret_cast<uint32_t *>(data_raw);
    lcd::lpprintf(0, 0, LCD_WHITE, LCD_DARKBLUE, "%8lu", data[0]);
    lcd::lpprintf(1, 0, LCD_WHITE, LCD_DARKBLUE, "%8lu", data[1]);
    lcd::lpprintf(2, 0, LCD_WHITE, LCD_DARKBLUE, "%8lu", data[2]);
    lcd::lpprintf(3, 0, LCD_WHITE, LCD_DARKBLUE, "%8lu", data[3]);
    lcd::lpprintf(4, 0, LCD_WHITE, LCD_DARKBLUE, "%8lu", data[4]);
}

int main(void){
    SystemInit();
    
    init_led();
    config_clk();
    //init_servo();
    //gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    
    // USING LONGAN NANO LED
    led_all_colors();
    
    lcd::load_defaults_longan();
    
    set_rgb(LED_RED);
    
    lcd::fill_rect(0, 80, 0, 160, LCD_DARKBLUE);
    
    set_rgb(LED_WHITE);
    
    //lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "LC-Display test ");
    //lcd::lpprintf(ROW_AHB, LCD_WHITE, LCD_DARKBLUE, "Frequency AHB:  %7lukHz\nFrequency APB1: %7lukHz", rcu_clock_freq_get(CK_AHB)/1000, rcu_clock_freq_get(CK_APB1)/1000);
    //lcd::lpprintf(ROW_CKSYS, LCD_WHITE, LCD_DARKBLUE, "Frequency CKSYS:%7lukHz", rcu_clock_freq_get(CK_SYS)/1000);
    
    rcu_periph_clock_enable(RCU_GPIOB);
    uint64_t devices[8];
    
    one_wire_t wire(GPIOB, 
                    GPIO_PIN_6, 
                    8, 
                    devices);
    
    ds18b20_t ds[8];
    
    set_rgb(LED_CYAN);
    
    wire.find_devices();
    lcd::lpprintf(ROW_DEVICES, LCD_WHITE, LCD_DARKBLUE, "[%3hhu:", wire.get_device_count());
    
    set_rgb(LED_GREEN);
    
    set_rgb(LED_BLUE);
    
    read_ds_data_t ds_data;
    ds_data.wireptr = &wire;
    ds_data.ds      = ds;
    ds_data.print_counter = 0;
    
    proc_t ds_proc(ds_begin_conversion, 
                   sch::ticks_from_us(1'000'000), 
                   proc_t::REPEAT, 
                   &ds_data);
    
    proc_t wire_proc(find_devices_f, 
                     sch::ticks_from_us(10'000'000), 
                     proc_t::REPEAT, 
                     &wire);
    
    //led_blink_t led_blink;
    proc_t led_blink_p(led_blink_f, 
                       sch::ticks_from_us(800'000), 
                       proc_t::REPEAT);
    
    proc_t get_cksys_proc(get_cksys, 
                          sch::ticks_from_us(2'000'000), 
                          proc_t::REPEAT);
    
    set_rgb(LED_CYAN);
    
    sch::proc_add(&ds_proc);
    sch::proc_add(&wire_proc);
    sch::proc_add(&led_blink_p);
    sch::proc_add(&get_cksys_proc);
    
    /*
    uint32_t cnt[5] = {0UL, 0UL, 0UL, 0UL, 0UL};
    
    proc_t p0(increment,  sch::ticks_from_us( 5000), proc_t::REPEAT, cnt + 0);
    proc_t p1(increment,  sch::ticks_from_us(10000), proc_t::REPEAT, cnt + 1);
    proc_t p2(increment,  sch::ticks_from_us(20000), proc_t::REPEAT, cnt + 2);
    proc_t p3(increment,  sch::ticks_from_us(40000), proc_t::REPEAT, cnt + 3);
    proc_t p4(increment,  sch::ticks_from_us(80000), proc_t::REPEAT, cnt + 4);

    sch::proc_add(&p0);
    sch::proc_add(&p1);
    sch::proc_add(&p2);
    sch::proc_add(&p3);
    sch::proc_add(&p4);
    
    proc_t printer(print5, sch::ticks_from_us(50000), proc_t::REPEAT, cnt);
    sch::proc_add(&printer);
    */
    set_rgb(LED_BLACK);
    
    sch::capture();
    
    set_rgb(LED_RED);
    
    return 0;
}






 
