#include "schv4.h" 
#include "gd32vf103.h"

uint64_t sch::ticks_from_us(uint64_t us){
    return us * rcu_clock_freq_get(CK_AHB) / (1000000ULL * 4);
}

uint64_t sch::get_current_time(){
    return get_timer_value();
}
