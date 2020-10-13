#ifndef ONE_WIRE_H
#define ONE_WIRE_H

// for debugging purposes im using the lcd
#include "lcd.h"
#include <cstdint>

//#include "lcd.h"
//#include <systick.h>

class one_wire_t {
private:
    const uint32_t port;
    const uint32_t pin;
    const uint8_t  device_max;
          uint8_t  device_count = 0;
          uint8_t  buffer_full = false;
          uint8_t  locked = false;
    
    void high();
    void low();
    uint8_t getv();
    
    uint8_t voltage_now;
    uint8_t voltage_before;
    uint8_t waveform_list[70];
    uint8_t waveform_count = 0;
    
    uint64_t * device;
    
    void send_search_rom();
    void send_read_rom();
    void send_match_rom();
    void send_skip_rom();
    
    void match_rom(uint64_t id);
    
public:
    uint8_t send_reset();
    void    send_bit(const uint8_t data);
    uint8_t read_bit();
    void    send_byte(const uint8_t data);
    uint8_t read_byte();
    
    uint8_t lock();
    void    unlock();
    
    one_wire_t(const uint32_t port_inp, 
               const uint32_t pin_inp,
               const uint8_t  device_max_inp,
               uint64_t * const device_list_inp);
    
    uint8_t  find_devices();
    uint8_t  get_device_count();
    uint64_t get_id(uint8_t list_index);
    
    uint32_t send(const uint8_t * data, uint8_t count, const uint64_t id = 0);
    uint32_t read(uint8_t * data, uint8_t count, const uint64_t id = 0);
    uint32_t read_rom(uint8_t * data, uint8_t count, const uint64_t id = 0);
    uint32_t request(const uint8_t * data_send, 
                     uint8_t count_send, 
                     uint8_t * data_read, 
                     uint8_t count_read, 
                     const uint64_t id = 0);
    
    void waveform_capture();
    void waveform_print();
};
/*
 * Attenmpt to make a RAII transmission
 * but the concept has too many flaws
class transmission_t {
    uint8_t local_lock;
    one_wire_t * const one_wire;
    const uint64_t uid;
    
public:
    
    transmission_t(one_wire_t * const one_wire_inp, const uint64_t uid_inp = 0);
    ~transmission_t();
    
    void release();
    
    transmission_t() = delete;
};

transmission_t::transmission_t(
    one_wire_t * const handler_inp, 
    const uint64_t uid_inp
): 
    handler(hanbdler_inp), 
    uid(uid_inp)
{
    local_lock = one_wire->lock();
}

transmission_t::~transmission_t(){
    if(local_lock){
        one_wire->unlock();
        local_lock = false;
    }
}

void transmission_t::release(){
    if(local_lock){
        one_wire->unlock();
        local_lock = false;
    }
}
*/

#endif // ONE_WIRE_H
