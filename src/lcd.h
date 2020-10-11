#ifndef LCD_H
#define LCD_H

#include <cstdarg>
#include <cstdint>

#include "systick.h"
#include "basti79_font.h"
#include "gd32vf103.h"
#include "lcd_colors.h"

/*
 * Author:  Yago Teodoro de Mello
 * Date:    2020-10-10
 * License: MIT
 * V1.0.0
 * 
 */

namespace lcd {
    // PB0 - RS (DCX)
    // PB1 - NRST
    // PB2 - NCS
    
    enum color_modes_t : uint8_t {
        COLOR_RGB = 0x00,
        COLOR_BGR = 0x08
    };
    
    enum rotation_modes_t : uint8_t {
        X_NORMAL    = 0x00,
        X_MIRROR    = 0x40,
        
        Y_NORMAL    = 0x00,
        Y_MIRROR    = 0x80,
        
        XY_NORMAL   = 0x00,
        XY_EXCHANGE = 0x20
    };
    
    void init_clock();
    void init_pins();
    void init_comm();
    
    void dcx(const uint8_t value);
    
    void transmission_begin();
    void transmission_end();
    
    uint8_t get_byte(const uint64_t data, const uint8_t byte);
    
    void cmd(const uint8_t value);
    void data(const uint8_t value);
    
    void send_nop();
    void send_sleepin();
    void send_sleepout();
    void send_invoff();
    void send_invon();
    void send_gamset(const uint8_t curve_id);
    void send_dispoff();
    void send_dispon();
    void send_caset(const uint16_t start, const uint16_t end);
    void send_raset(const uint16_t start, const uint16_t end);
    void send_ramwr(const uint8_t * payload, const uint32_t size);
    void send_madctl(const uint8_t reg);
    void send_colmod(const uint8_t ipf);
    
    extern uint16_t screen_rows;
    extern uint16_t screen_cols;
    
    extern uint16_t screen_rows_offset;
    extern uint16_t screen_cols_offset;
    
    extern uint16_t cursor_row;
    extern uint16_t cursor_col;
    
    void init(const uint16_t rows, 
              const uint16_t cols, 
              const uint16_t rows_offset = 0, 
              const uint16_t cols_offset = 0);
    
    void load_defaults_longan();
    
    void write(const uint8_t * payload, const uint16_t dim_rows, const uint16_t dim_cols);
    
    void fill_rect(const uint16_t row_start, 
                   const uint16_t row_end, 
                   const uint16_t col_start, 
                   const uint16_t col_end, 
                   const uint8_t red, 
                   const uint8_t green, 
                   const uint8_t blue);
    
    void fill(const uint8_t red, const uint8_t green, const uint8_t blue);
    
    void adjust(const uint8_t mirror_x,
                const uint8_t mirror_y,
                const uint8_t exchange_xy,
                const uint8_t color);
    
    void lputc(char id, 
               uint8_t red_font, 
               uint8_t green_font, 
               uint8_t blue_font, 
               uint8_t red_background = 0, 
               uint8_t green_background = 0, 
               uint8_t blue_background = 0);
    
    void lputs(const char * buffer,
               uint8_t red_font, 
               uint8_t green_font, 
               uint8_t blue_font, 
               uint8_t red_background = 0, 
               uint8_t green_background = 0, 
               uint8_t blue_background = 0);
    
    void lprintf(const uint8_t red_font, 
                 const uint8_t green_font, 
                 const uint8_t blue_font, 
                 const uint8_t red_background, 
                 const uint8_t green_background, 
                 const uint8_t blue_background,
                 const char * format, 
                 ...);
    
    /*
    // poor mans std::string
    class pmstring_t {
    public:
        uint16_t length;
        char data[64];
        
        void clear();
        void putchar(char input);
    };
    
    extern pmstring_t sprintf_buffer;
    */
    
}

#endif //LCD_H
