#ifndef LCD_TEST_FUNCTIONS
#define LCD_TEST_FUNCTIONS

/*
 * Author:  Yago Teodoro de Mello
 * Date:    2020-10-11
 * License: MIT
 * V1.0.0
 * 
 * This is a header only "library"
 * 
 */

// Declarations

void gamma_test_step();
void gamma_test_linear_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white, uint8_t step = 3, uint8_t count = 22);
void lcd_color_test();


// Definitions

inline void gamma_test_step(){
    static uint8_t iterator = 0;
    lcd::lprintf(LCD_WHITE, LCD_DARKBLUE, "\rNumber: ");
    lcd::lprintf(0x9F, 0xFF, 0x9F, 4 * iterator, 0x40, 0x80, "%4hhu", iterator % 64);
        
    if(iterator % 64 == 0){
        lcd::send_gamset(iterator / 64);
        lcd::lprintf(0x7F, 0x7F, 0x7F, LCD_DARKBLUE, "   gamma: %3hhu", iterator / 64);
    }
    
    iterator += 1;
}

inline void gamma_test_linear_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white, uint8_t step, uint8_t count){
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

inline void lcd_color_test(){
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

#endif // LCD_TEST_FUNCTIONS
