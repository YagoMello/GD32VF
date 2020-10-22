#ifndef SCHV4_H
#define SCHV4_H

#include <cstdint>

class proc_t;

namespace sch {

    extern proc_t * proc_list;
    extern uint64_t time_last_call;

    void     capture();
    void     idle();
    
    uint64_t ticks_from_us(uint64_t us);
    uint64_t get_current_time();
    
    void     remove_item_from_list(proc_t * proc);
    void     overflow_cleanup();
    
    void     proc_add(proc_t * proc);
    void     proc_kill(proc_t * proc);
    
    uint32_t proc_count();
    
}

class exec_base_t {
public:
    virtual void func(proc_t *) = 0;
};

class proc_t {
public:
    using callback_t = void (*)(proc_t *, void *);
    
private:
    
    enum : uint8_t {
        UNDEFINED,
        CALLBACK,
        POLYMORPHIC
    };
    
    union target_t {
        callback_t    callback;
        exec_base_t * polymorphic;
    };
    
    uint8_t  target_type = UNDEFINED;
    target_t target;
     
    void *   data;
    
    uint64_t time;
    uint64_t interval;
    
    uint8_t  mode;
    
    proc_t * prev = nullptr;
    proc_t * next = nullptr;
    
public:
    
    enum : uint8_t {
        SINGLE,
        REPEAT
    };
    
    proc_t(callback_t func, 
           uint64_t interval_inp, 
           uint8_t mode_inp  = REPEAT, 
           void * data_inp   = nullptr);
    
    proc_t(exec_base_t * object, 
           uint64_t interval_inp, 
           uint8_t mode_inp = REPEAT);
    
    ~proc_t();
    
    void exec();
    
    void terminate();
    void set_mode(uint8_t);
    
    friend void sch::capture();
    
    friend void sch::remove_item_from_list(proc_t * proc);
    friend void sch::overflow_cleanup();
    
    friend void sch::proc_add(proc_t * proc);
    friend void sch::proc_kill(proc_t * proc);
    
    friend uint32_t sch::proc_count();
    
};



#endif // SCHV4_H
