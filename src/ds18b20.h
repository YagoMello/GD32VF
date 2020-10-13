#ifndef DS18B20_H
#define DS18B20_H

class ds18b20_t {
private:
    uint8_t precision_bits;
    uint8_t precision_seq[4] = {0x4E, 0xFF, 0x00, 0x7F};
    
public:
    ds18b20_t(uint8_t precision_bits_inp = 12);
    
    double get_temp(uint8_t * buffer);
    static double get_tempf(uint8_t * buffer, int8_t precision);
    
    uint8_t * set_precision(const uint8_t value);
    uint8_t * get_precision();
    
    uint32_t delay_value_milisseconds();
    void delay_conversion();
    
    constexpr static uint8_t begin_conversion[1] = {0x44};
    constexpr static uint8_t read_scratchpad[1]  = {0xBE};
}; 

#endif // DS18B20_H
