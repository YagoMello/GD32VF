#include <cstdint>
#include "systick.h"
#include "ds18b20.h"

ds18b20_t::ds18b20_t(uint8_t precision_bits_inp){
    set_precision(precision_bits_inp);
}

double ds18b20_t::get_temp(uint8_t * buffer){
    uint16_t temp;
    temp  = buffer[0];
    temp |= (uint16_t(buffer[1])) << 8;
    
    int8_t powe = 7;
    
    double value = 0;
    while(powe > (8 - precision_bits)){
        powe--;
        
        if((powe >= 0) && (temp & (1ULL << (powe + 4)))){
            value += static_cast<double>(1ULL << powe);
        }
        else if((powe < 0) && (temp & (1ULL << (powe + 4)))){
            value += 1.0/static_cast<double>(1ULL << (-powe));
        }
    }
    if(temp & 0xF000){
        value = -value;
    }
    
    return value;
}

double ds18b20_t::get_tempf(uint8_t * buffer, int8_t precision){
    
    if(precision < 9){
        precision = 9;
    }
    if(precision > 12){
        precision = 12;
    }
    
    uint16_t temp;
    temp  = buffer[0];
    temp |= (uint16_t(buffer[1])) << 8;
    
    int8_t powe = 7;
    
    double value = 0;
    while(powe > (8 - precision)){
        powe--;
        
        if((powe >= 0) && (temp & (1ULL << (powe + 4)))){
            value += static_cast<double>(1ULL << powe);
        }
        else if((powe < 0) && (temp & (1ULL << (powe + 4)))){
            value += 1.0/static_cast<double>(1ULL << (-powe));
        }
    }
    if(temp & 0xF000){
        value = -value;
    }
    
    return value;
}

uint8_t * ds18b20_t::set_precision(const uint8_t value){
    precision_seq[3] = 0x0F | ((value - 9) << 5);
    precision_bits = value;
    return precision_seq;
}

uint8_t * ds18b20_t::get_precision(){
    //precision_seq[3] = 0xFF | ((precision_bits - 9) << 5);
    return precision_seq;
}

uint32_t ds18b20_t::delay_value_milisseconds(){
    uint32_t value;
    
    if(precision_bits == 12){
        value = 750;
    }
    else if(precision_bits == 11){
        value = 375;
    }
    else if(precision_bits == 10){
        value = 188;
    }
    else if(precision_bits == 9){
        value = 94;
    }
    else{
        value = 750;
    }
    
    return value;
}

void ds18b20_t::delay_conversion(){
    delay_1ms(delay_value_milisseconds());
}

