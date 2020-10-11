#include "printf.h"
#include "lcd.h" 

/*
 * Author:  Yago Teodoro de Mello
 * Date:    2020-10-10
 * License: MIT
 * V1.0.0
 */

void lcd::init_clock(){
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_SPI0);
}

void lcd::init_pins(){
    gpio_init(GPIOA, GPIO_MODE_AF_PP,  GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
    
    gpio_bit_reset(GPIOB, GPIO_PIN_0);
    gpio_bit_set(  GPIOB, GPIO_PIN_1 | GPIO_PIN_2);
}

void lcd::init_comm(){
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
    spi_bidirectional_transfer_config(SPI0, SPI_BIDIRECTIONAL_TRANSMIT);
    spi_enable(SPI0);
}

void lcd::dcx(const uint8_t value){
    if(gpio_output_bit_get(GPIOB, GPIO_PIN_0) != value){
        if(value){
            gpio_bit_set(GPIOB, GPIO_PIN_0);
        }
        else{
            gpio_bit_reset(GPIOB, GPIO_PIN_0);
        }
    }
}


void lcd::transmission_begin(){
    gpio_bit_reset(GPIOB, GPIO_PIN_2);
}

void lcd::transmission_end(){
    gpio_bit_set(GPIOB, GPIO_PIN_2);
}

uint8_t lcd::get_byte(const uint64_t data, const uint8_t byte){
    return static_cast<uint8_t>(data >> (byte << 3));
}

void lcd::cmd(const uint8_t value){
    dcx(0);
    transmission_begin();
    
    spi_i2s_data_transmit(SPI0, value);
    while(spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS) == SET);
    
    transmission_end();
}

void lcd::data(const uint8_t value){
    dcx(1);
    transmission_begin();
    
    spi_i2s_data_transmit(SPI0, value);
    while(spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS) == SET);
    
    transmission_end();
}

void lcd::send_nop(){
    cmd(0x00);
}

void lcd::send_sleepin(){
    cmd(0x10);
    delay_1ms(200);
}

void lcd::send_sleepout(){
    cmd(0x11);
    delay_1ms(200);
}

void lcd::send_invoff(){
    cmd(0x20);
    delay_1ms(200);
}

void lcd::send_invon(){
    cmd(0x21);
    delay_1ms(200);
}

void lcd::send_gamset(const uint8_t curve_id){
    cmd(0x26);
    data(0x01 << curve_id);
    delay_1ms(200);
}

void lcd::send_dispoff(){
    cmd(0x28);
    delay_1ms(500);
}

void lcd::send_dispon(){
    cmd(0x29);
    delay_1ms(500);
}

void lcd::send_caset(const uint16_t start, const uint16_t end){
    cmd(0x2A);
    
    data(get_byte(start, 1));
    data(get_byte(start, 0));
    
    data(get_byte(end - 1, 1));
    data(get_byte(end - 1, 0));
}

void lcd::send_raset(const uint16_t start, const uint16_t end){
    cmd(0x2B);
    
    data(get_byte(start, 1));
    data(get_byte(start, 0));
    
    data(get_byte(end - 1, 1));
    data(get_byte(end - 1, 0));
}

void lcd::send_ramwr(const uint8_t * payload, const uint32_t size){
    uint32_t iterator = 0;
    cmd(0x2C);
    
    while(iterator < size){
        data(payload[iterator]);
        iterator++;
    }
}

void lcd::send_madctl(const uint8_t reg){
    cmd(0x36);
    data(reg);
}

void lcd::send_colmod(const uint8_t ipf){
    /*
     * ipf:
     * 0x03 - 12bpp
     * 0x05 - 16bpp
     * 0x06 - 18bpp
     */
    cmd(0x3A);
    data(ipf);
}

uint16_t lcd::screen_rows = 64;
uint16_t lcd::screen_cols = 64;

uint16_t lcd::screen_rows_offset = 0;
uint16_t lcd::screen_cols_offset = 0;

uint16_t lcd::cursor_row;
uint16_t lcd::cursor_col;

void lcd::init(
    const uint16_t rows, 
    const uint16_t cols,
    const uint16_t rows_offset,
    const uint16_t cols_offset
){
    screen_rows = rows;
    screen_cols = cols;
    
    screen_rows_offset = rows_offset;
    screen_cols_offset = cols_offset;
    
    cursor_row = 0;
    cursor_col = 0;
    
    init_clock();
    init_pins();
    init_comm();
    
    send_sleepout();
    send_dispon();
}

void lcd::load_defaults_longan(){
    lcd::init(80, 160, 26, 1);
    
    lcd::adjust(lcd::XY_EXCHANGE, 
                lcd::X_MIRROR, 
                lcd::Y_NORMAL, 
                lcd::COLOR_BGR);
    
    send_invon();
    
    send_gamset(1);
    
}

void lcd::write(const uint8_t * payload, const uint16_t dim_rows, const uint16_t dim_cols){
    
    send_caset(cursor_col + screen_cols_offset, 
               cursor_col + dim_cols + screen_cols_offset);
    
    send_raset(cursor_row + screen_rows_offset, 
               cursor_row + dim_rows + screen_rows_offset);
    
    send_ramwr(payload, dim_rows * dim_cols * 3);
    
    if(cursor_col + dim_cols > screen_cols){
        if(cursor_row + dim_rows > screen_rows){
            cursor_row = 0;
            cursor_col = 0;
        }
        else{
            cursor_row += dim_rows;
            cursor_col = 0;
        }
    }
    else{
        cursor_col += dim_cols;
    }
    
}


void lcd::fill_rect(
    const uint16_t row_start,
    const uint16_t row_end,
    const uint16_t col_start,
    const uint16_t col_end,
    const uint8_t red,
    const uint8_t green,
    const uint8_t blue
){
    uint32_t iterator = 0;
    
    send_caset(col_start + screen_cols_offset, col_end + screen_cols_offset);
    send_raset(row_start + screen_rows_offset, row_end + screen_rows_offset);
    
    cmd(0x2C);
    
    while(iterator < static_cast<uint32_t>((col_end - col_start) * (row_end - row_start))){
        data(red);
        data(green);
        data(blue);
        iterator++;
    }
}

void lcd::fill(const uint8_t red, const uint8_t green, const uint8_t blue){
    fill_rect(0, screen_rows, 0, screen_cols, red, green, blue);
}

void lcd::adjust(
    const uint8_t exchange_xy,
    const uint8_t mirror_x,
    const uint8_t mirror_y,
    const uint8_t color
){
    cmd(0x36);
    data(exchange_xy | mirror_x | mirror_y | color);
}

#define FONT_ROWS 8
#define FONT_COLS 6
#define MAX_R 63
#define MAX_G 63
#define MAX_B 63

static void decompose(
    const uint8_t id, 
    const uint8_t red_font, 
    const uint8_t green_font, 
    const uint8_t blue_font,
    const uint8_t red_background, 
    const uint8_t green_background, 
    const uint8_t blue_background, 
    uint8_t * buffer
){
    uint16_t iterator_rows;
    uint16_t iterator_cols;
    uint8_t bits;
    
    iterator_rows = 0;
    while(iterator_rows < FONT_ROWS){
        iterator_cols = 0;
        
        bits = font[id][iterator_rows] << 3; // allign left
        
        while(iterator_cols < FONT_COLS){
            
            if(bits & 0x80){
                buffer[iterator_rows * FONT_COLS * 3 + iterator_cols * 3 + 0] = red_font;
                buffer[iterator_rows * FONT_COLS * 3 + iterator_cols * 3 + 1] = green_font;
                buffer[iterator_rows * FONT_COLS * 3 + iterator_cols * 3 + 2] = blue_font;
            }
            else{
                buffer[iterator_rows * FONT_COLS * 3 + iterator_cols * 3 + 0] = red_background;
                buffer[iterator_rows * FONT_COLS * 3 + iterator_cols * 3 + 1] = green_background;
                buffer[iterator_rows * FONT_COLS * 3 + iterator_cols * 3 + 2] = blue_background;
            }
            
            bits = bits << 1;
            iterator_cols++;
        }
        
        iterator_rows++;
    }
    
    
}

void lcd::lputc(
    const char id, 
    const uint8_t red_font , 
    const uint8_t green_font, 
    const uint8_t blue_font, 
    const uint8_t red_background, 
    const uint8_t green_background, 
    const uint8_t blue_background
){
    uint8_t buffer[FONT_ROWS * FONT_COLS * 3];
    
    switch(id){
        case '\n':
            cursor_row += FONT_ROWS;
            cursor_col = 0;
            break;
        
        case '\r':
            cursor_col = 0;
            break;
            
        default:
            decompose(uint8_t(id),
                      red_font, 
                      green_font, 
                      blue_font, 
                      red_background, 
                      green_background, 
                      blue_background,
                      buffer);
            
            write(buffer, FONT_ROWS, FONT_COLS);
            break;
        
    }
    
}

void lcd::lputs(
    const char * buffer,
    const uint8_t red_font , 
    const uint8_t green_font, 
    const uint8_t blue_font, 
    const uint8_t red_background, 
    const uint8_t green_background, 
    const uint8_t blue_background
){
    uint16_t iterator = 0;
    while(buffer[iterator]){
        
        lcd::lputc(buffer[iterator], 
                   red_font, 
                   green_font, 
                   blue_font, 
                   red_background, 
                   green_background, 
                   blue_background);
        
        iterator++;
    }
}

void lcd::lprintf(
    const uint8_t red_font , 
    const uint8_t green_font, 
    const uint8_t blue_font, 
    const uint8_t red_background, 
    const uint8_t green_background, 
    const uint8_t blue_background,
    const char * format,
    ...
){
    va_list args;
    char result[64];
    
    //printf_buffer.clear();
    
    va_start(args, format);
    vsnprintf(result, 64, format, args);
    va_end(args);
    
    lcd::lputs(result,
               red_font, 
               green_font, 
               blue_font, 
               red_background, 
               green_background, 
               blue_background);
    
}

/*
void lcd::pmstring_t::clear(){
    length = 0;
    data[0] = 0;
}

void lcd::pmstring_t::putchar(char input){
    data[length] = input;
    length++;
}

lcd::pmstring_t lcd::sprintf_buffer;
*/
