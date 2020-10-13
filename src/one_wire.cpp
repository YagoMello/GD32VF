#include <cstdint>
#include "one_wire.h"


void one_wire_t::high(){
    gpio_bit_set(port, pin);
}

void one_wire_t::low(){
    gpio_bit_reset(port, pin);
}

uint8_t one_wire_t::getv(){
    if(gpio_input_bit_get(port, pin))
        return 1;
    else
        return 0;
}
/*
void one_wire_t::waveform_capture(){
    if(buffer_full == false){
        delay_1ns(100);
        voltage_before = voltage_now;
        voltage_now = getv();
        
        if(waveform_count != 0)
            waveform_list[waveform_count] = (voltage_before << 1) | voltage_now;
        else
            waveform_list[waveform_count] = (voltage_now << 1) | voltage_now;
            
        waveform_count++;
        if(waveform_count >= 70){
            buffer_full = true;
        }
    }
}

void one_wire_t::waveform_print(){
    // IMG 8r x 2c
    const uint8_t transitions[4][8*2*3] = {
       {LCD_BLACK, LCD_YELLOW,//0
        LCD_BLACK, LCD_BLACK, //1
        LCD_BLACK, LCD_BLACK, //2
        LCD_BLACK, LCD_BLACK, //3
        LCD_BLACK, LCD_BLACK, //4
        LCD_BLACK, LCD_BLACK, //5
        LCD_BLACK, LCD_BLACK, //6
        LCD_WHITE, LCD_WHITE},//7
        
       {LCD_BLACK, LCD_YELLOW,//0
        LCD_WHITE, LCD_WHITE, //1
        LCD_WHITE, LCD_BLACK, //2
        LCD_WHITE, LCD_BLACK, //3
        LCD_WHITE, LCD_BLACK, //4
        LCD_WHITE, LCD_BLACK, //5
        LCD_WHITE, LCD_BLACK, //6
        LCD_WHITE, LCD_BLACK},//7
        
       {LCD_BLACK, LCD_YELLOW,//0
        LCD_WHITE, LCD_BLACK, //1
        LCD_WHITE, LCD_BLACK, //2
        LCD_WHITE, LCD_BLACK, //3
        LCD_WHITE, LCD_BLACK, //4
        LCD_WHITE, LCD_BLACK, //5
        LCD_WHITE, LCD_BLACK, //6
        LCD_WHITE, LCD_WHITE},//7
        
       {LCD_BLACK, LCD_YELLOW,//0
        LCD_WHITE, LCD_WHITE, //1
        LCD_BLACK, LCD_BLACK, //2
        LCD_BLACK, LCD_BLACK, //3
        LCD_BLACK, LCD_BLACK, //4
        LCD_BLACK, LCD_BLACK, //5
        LCD_BLACK, LCD_BLACK, //6
        LCD_BLACK, LCD_BLACK}};//7
    
    uint8_t waveform_iterator;
    // clear space
    waveform_iterator = 2 * 70 / 6 + 1;
    while(waveform_iterator--){
        lcd::lputc(' ', LCD_WHITE, LCD_GRAY);
    }
    lcd::lputc('\r', LCD_WHITE, LCD_GRAY);
    // print waveforms
    waveform_iterator = 0;
    while(waveform_iterator < waveform_count)
        lcd::write(transitions[waveform_list[waveform_iterator++]], 8, 2);
    
    waveform_count = 0;
    buffer_full = false;
}
*/
one_wire_t::one_wire_t(
    const uint32_t   port_inp, 
    const uint32_t   pin_inp,
    const uint8_t    device_max_inp,
    uint64_t * const device_list_inp
) : 
    port(port_inp),
    pin(pin_inp),
    device_max(device_max_inp),
    device(device_list_inp)
{
    gpio_init(port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, pin);
    send_reset();
}

void one_wire_t::send_bit(const uint8_t data){
    const uint32_t delay_init = 6;
    const uint32_t delay_one_end = 64;
    const uint32_t delay_zero_release = 54;
    const uint32_t delay_zero_end = 10;
    
    if(data){
        low();
        delay_1us(delay_init);
        
        high();
        delay_1us(delay_one_end);
    }
    else{
        low();
        delay_1us(delay_init);
        
        low();
        delay_1us(delay_zero_release);
        
        high();
        delay_1us(delay_zero_end);
    }
}

uint8_t one_wire_t::read_bit(){
    const uint32_t delay_init = 6;
    const uint32_t delay_sample = 9;
    const uint32_t delay_end = 55;
    
    uint8_t value;
    
    low();
    delay_1us(delay_init);
    
    high();
    delay_1us(delay_sample);
    
    value = getv();
    delay_1us(delay_end);
    
    return value;
}

void one_wire_t::send_byte(uint8_t data){
    const uint32_t delay_init = 6;
    const uint32_t delay_one_end = 64;
    const uint32_t delay_zero_release = 54;
    const uint32_t delay_zero_end = 10;
    
    uint8_t bits_remaining = 8;
    while(bits_remaining){
        bits_remaining--;
        if(data & 0x01){
            low();
            delay_1us(delay_init);
            
            high();
            delay_1us(delay_one_end);
        }
        else{
            low();
            delay_1us(delay_init);
            
            low();
            delay_1us(delay_zero_release);
            
            high();
            delay_1us(delay_zero_end);
        }
        data = data >> 1;
    }
}

uint8_t one_wire_t::read_byte(){
    const uint32_t delay_init = 6;
    const uint32_t delay_sample = 9;
    const uint32_t delay_end = 55;
    
    uint8_t value = 0;
    
    uint8_t bits_remaining = 8;
    while(bits_remaining){
        bits_remaining--;
        value = value >> 1;
        
        low();
        delay_1us(delay_init);
        high();
        
        delay_1us(delay_sample);
        
        if(getv()){
            value |= 0x80;
        }
        else{
            value |= 0x00;
        }
        
        delay_1us(delay_end);
    }
    
    return value;
}

uint8_t one_wire_t::send_reset(){
    uint8_t response;
    
    low();
    delay_1us(480);
    
    high();
    delay_1us(70);
    
    response = !getv();
    
    delay_1us(410);
    
    response &= getv();
    
    return response;
}

void one_wire_t::send_search_rom(){
    send_byte(0xF0);
}

void one_wire_t::send_read_rom(){
    send_byte(0x33);
}

void one_wire_t::send_match_rom(){
    send_byte(0x55);
}

void one_wire_t::send_skip_rom(){
    send_byte(0xCC);
}

void one_wire_t::match_rom(uint64_t id){
    const uint32_t delay_init = 6;
    const uint32_t delay_one_end = 64;
    const uint32_t delay_zero_release = 54;
    const uint32_t delay_zero_end = 10;
    
    send_match_rom();
    
    uint8_t bits_remaining = 64;
    while(bits_remaining){
        bits_remaining--;
        if(id & 0x01){
            low();
            delay_1us(delay_init);
            
            high();
            delay_1us(delay_one_end);
        }
        else{
            low();
            delay_1us(delay_init);
            
            low();
            delay_1us(delay_zero_release);
            
            high();
            delay_1us(delay_zero_end);
        }
        id = id >> 1;
    }
    
}

uint8_t one_wire_t::find_devices(){
    uint8_t response = send_reset();
    device_count = 0;
    
    uint8_t rom_bit_not_inverted;
    uint8_t rom_bit_inverted;
    
    uint8_t bits_remaining;
    uint8_t running = true;
    //                          1  2  3  4  5  6  7  8
    uint64_t tree_iterator = 0xFF'FF'FF'FF'FF'FF'FF'FF;
    
    uint64_t sequence;
    
    uint64_t tree_test;
    uint8_t  shift_done;
    
    uint8_t bad_read;
    
    // if at least one device connected
    if(response){
        // only stops when there are no branches remaining
        while(running && (device_count < device_max)){
            send_reset();
            send_search_rom();
            
            sequence = 0;           // stores the route used this iteration
            bad_read = false;       // start as a nice read
            bits_remaining = 64;    // ammount of bits to check
            while(bits_remaining && !bad_read){
                bits_remaining--;
                
                // the accumulator is 0 if the addresses of the devices on the bus have different bits here
                // and 1 if equal (or is a single device)
                rom_bit_not_inverted = read_bit();
                rom_bit_inverted     = read_bit();
                
                // if equal and 1, no device answered, bad read
                // if equal and 0, the "bitwise or" will return 1 ("true")
                // if different, will return 0
                if(rom_bit_not_inverted & rom_bit_inverted){
                    bad_read = true;
                }
                else if(rom_bit_not_inverted | rom_bit_inverted){
                    //if equal, there is only one path to go here
                    send_bit(rom_bit_not_inverted);
                    
                    // we store the sequence, from right to left (bits_remaining starts as 63)
                    // also known as lsb first...
                    if(rom_bit_not_inverted){
                        sequence |= ((0x01ULL << 63) >> bits_remaining);
                    }
                    else{
                        sequence &= ~((0x01ULL << 63) >> bits_remaining);
                    }
                    
                    tree_iterator &= ~(0x01ULL << bits_remaining);
                }
                else{
                    // if the tree_iterator == 1 here, chose the "1" path
                    // if not, chose the "0" path
                    if((tree_iterator & (0x01ULL << bits_remaining)) != 0){
                        send_bit(1);
                        sequence |= ((0x01ULL << 63) >> bits_remaining); // store the selected value
                    }
                    else{
                        send_bit(0);
                        sequence &= ~((0x01ULL << 63) >> bits_remaining);
                    }
                }
            }
            
            // store the resulting sequence as a device
            // but only it it was a good read
            if(!bad_read){
                device[device_count] = sequence;
                device_count++;
            }
            
            // if the path was everything 0, we are done
            // because there is no "1" to clear
            if(tree_iterator == 0){
                running = 0;
            }
            
            // copy the iterator for shifting purposes
            tree_test = tree_iterator;
            bits_remaining = 64;
            // while we have not found a "1", shift_done is false
            shift_done = false;
            while(bits_remaining && !shift_done && running){
                bits_remaining--;
                
                // test the position closest to the end (last tested)
                if(tree_test & 0x01ULL){
                    // if is a 1, clear the "1" and stop this search 
                    tree_iterator &= ~((0x01ULL << 63) >> bits_remaining);
                    shift_done = true;
                }
                else{
                    // if is a 0, keep shifting
                    tree_test = tree_test >> 1;
                    // and clear the path trailed
                    tree_iterator |= ((0x01ULL << 63) >> bits_remaining);
                }
            }
        }
    }
    return device_count;
}

uint8_t one_wire_t::get_device_count(){
    return device_count;
}

uint64_t one_wire_t::get_id(uint8_t list_index){
    return device[list_index];
}

uint8_t one_wire_t::lock(){
    uint8_t status;
    if(locked == false){
        locked = true;
        status = true;
    }
    else{
        status = false;
    }
    return status;
}

void one_wire_t::unlock(){
    locked = false;
}

uint32_t one_wire_t::send(const uint8_t * data, const uint8_t count, const uint64_t id){
    uint32_t iterator = 0;
    if(lock()){
        send_reset();
        if(id != 0){
            match_rom(id);
        }
        else{
            send_skip_rom();
        }
        
        while(iterator < count){
            send_byte(data[iterator]);
            iterator++;
        }
        unlock();
    }
    return iterator;
}

uint32_t one_wire_t::read(uint8_t * data, const uint8_t count, const uint64_t id){
    uint32_t iterator = 0;
    if(lock()){
        send_reset();
        if(id != 0)
            match_rom(id);
        else
            send_skip_rom();
        
        while(iterator < count){
            data[iterator] = read_byte();
            iterator++;
        }
        unlock();
    }
    return iterator;
}

uint32_t one_wire_t::read_rom(uint8_t * data, const uint8_t count, const uint64_t id){
    uint32_t iterator = 0;
    if(lock()){
        send_reset();
        if(id != 0)
            match_rom(id);
        else
            send_skip_rom();
        
        send_read_rom();
        
        send_reset();
        if(id != 0)
            match_rom(id);
        else
            send_skip_rom();
        
        while(iterator < count){
            data[iterator] = read_byte();
            iterator++;
        }
        unlock();
    }
    return iterator;
}

uint32_t one_wire_t::request(
    const uint8_t  * data_send, 
          uint8_t    count_send, 
          uint8_t  * data_read, 
          uint8_t    count_read, 
    const uint64_t   id
){
    uint32_t iterator_send = 0;
    uint32_t iterator_read = 0;
    if(lock()){
        send_reset();
        //send
        if(id != 0)
            match_rom(id);
        else
            send_skip_rom();
        
        while(iterator_send < count_send){
            send_byte(data_send[iterator_send]);
            iterator_send++;
        }
        //read
        while(iterator_read < count_read){
            data_read[iterator_read] = read_byte();
            iterator_read++;
        }
        unlock();
    }
    return iterator_read;
}
 
