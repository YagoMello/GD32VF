#include "schv4.h"
#include "lcd.h"

// PROC DEFINITIONS-------------------------------------------

proc_t::proc_t(
    callback_t callback_inp, 
    uint64_t   interval_inp, 
    uint8_t    mode_inp, 
    void *     data_inp
){
    time = 0;
    
    interval = interval_inp;
    mode = mode_inp;
    data = data_inp;
    
    target.callback = callback_inp;
    target_type     = CALLBACK;
}
    
proc_t::proc_t(
    exec_base_t * object, 
    uint64_t      interval_inp, 
    uint8_t       mode_inp
){
    time = 0;
    
    interval = interval_inp;
    mode = mode_inp;
    
    target.polymorphic = object;
    target_type        = POLYMORPHIC;
}
    
proc_t::~proc_t(){
    mode = UNDEFINED;
    terminate();
}

void proc_t::exec(){
    switch(target_type){
        
        case UNDEFINED:
            break;
        
        case CALLBACK:
            target.callback(this, data);
            break;
        
        case POLYMORPHIC:
            target.polymorphic->func(this);
            break;
        
    }
}

void proc_t::terminate(){
    sch::proc_kill(this);
}

void proc_t::set_mode(uint8_t arg){
    // we update the mode
    mode = arg;
    
    // find which type of transition
    // single to repeat, repeat to single, etc.
    if((arg == proc_t::REPEAT) && (mode != proc_t::REPEAT)){
        // the mode is changing to REPEAT
        
        // find if the time base is correct
        // test if the proc is not active
        if(sch::proc_list != this && prev == nullptr && next == nullptr){
            // lets clear the previous time to prevent its usage
            // as time base
            time = 0;
        }
        // else: we will use its previous time as base
        
        // and add the proc
        sch::proc_add(this);
    }
    else if((arg == proc_t::SINGLE) && (mode != proc_t::SINGLE)){
        // the mode is changing to SINGLE
        
        // we just kill this proc
        terminate();
    }
}


// SCH DEFINITIONS -------------------------------------------


proc_t * sch::proc_list      = nullptr;
uint64_t sch::time_last_call = 0;

void sch::remove_item_from_list(proc_t * proc){
    
    // we cannot remove a null pointer
    if(proc != nullptr){
        
        // lets find our place in the list
        if(
            proc->prev == nullptr && 
            proc->next == nullptr &&
            proc_list  != proc
        ){
            // we are not in the list
            // nothing to do
        }
        else if(
            proc->prev == nullptr && 
            proc->next == nullptr
        ){
            // we are at the beginning and the end of the list, simultaneously
            proc_list = nullptr;
        }
        else if(proc->prev == nullptr){
            // we are at the beginning of the list
            
            // make the next proc the list entry point
            proc_list = proc->next;
            
            // and remove this proc reference from it
            proc_list->prev = nullptr;
        }
        else if(proc->next != nullptr){
            // we are in the middle of the list
            proc->prev->next = proc->next;
            proc->next->prev = proc->prev;
        }
        else{
            // we are at the end of the list
            
            // make the prev entry the end of the list
            proc->prev->next = nullptr;
        }
        
        // and remove the list from this proc
        proc->prev = nullptr;
        proc->next = nullptr;
    }
    
}

void sch::overflow_cleanup(){
    proc_t * proc_curr;
    
    // lets call every function from the list that has
    // the time set to after the last call time
    
    // while functions remain
    do{
        // we start from the beginning
        proc_curr = proc_list;
        
        // and iterate until we find a proc after the last call
        // or we dont find anything
        while(
            proc_curr       != nullptr        && 
            proc_curr->time <  time_last_call
        ){
            proc_curr = proc_curr->next;
        }
        
        if(proc_curr != nullptr){
            // then we do our normal call convention
            
            // removing this item from the list
            sch::remove_item_from_list(proc_curr);
            
            // and if the proc is in repeat mode
            if(proc_curr->mode == proc_t::REPEAT){
                // add it again
                proc_add(proc_curr);
            }
            
            // lets run the proc
            proc_curr->exec();
        }
        
    }while(proc_curr != nullptr);
    
}

void sch::capture(){
    proc_t * proc_curr;
    uint64_t time_now;
    
    while(true){
        time_now = sch::get_current_time();
        
        // get the first proc from the list
        proc_curr = proc_list;
        
        
        if( // test for overflow
            time_last_call >  time_now        &&
            time_last_call <  proc_curr->time &&
            proc_curr      != nullptr
        ){
            // seems like we had an overflow
            
            // lets fix this
            sch::overflow_cleanup();
            
            // the next one might happen in a few thousand years...
        }
        else if( // test if its time to run
            time_now  >= proc_curr->time &&
            proc_curr != nullptr
        ){
            // reached the time to run the proc
            
            // set the next event as ready list entry point
            proc_list = proc_curr->next;
            
            // and temove this proc from the list
            proc_list->prev = nullptr;
            
            // and remove the list from this proc
            proc_curr->prev = nullptr;
            proc_curr->next = nullptr;
            
            // and if the proc is in repeat mode
            if(proc_curr->mode == proc_t::REPEAT){
                // add it again
                proc_add(proc_curr);
            }
            
            // lets run the proc
            proc_curr->exec();
        }
        else{
            // if there is no proc ready to run
            // call the idle function
            sch::idle();
        }
        
        // update the last call time
        time_last_call = time_now;
    }
    
}

void __attribute__((weak)) sch::idle(){
    
}

uint32_t sch::proc_count(){
    uint32_t count = 0;
    proc_t * iterator = proc_list;
    
    while(iterator != nullptr){
        iterator = iterator->next;
        count++;
    }
    
    return count;
}

void sch::proc_add(proc_t * proc){
    proc_t * proc_iterator          = proc_list;
    uint8_t  not_done               = true;
    
    // just a null pointer protection
    if(proc != nullptr){
        
        // check if the proc is already on the list
        if( // if the proc does not have any proc reference
            proc->prev    == nullptr && 
            proc->next    == nullptr && 
            // and is not the first and unique element
            proc_iterator != proc
        ){
            // this proc is not on the list, good
            
            // lets update the proc time from the interval
            if(proc->mode == proc_t::REPEAT){
                // the proc is repeating
                
                // we are going to use the previous time as reference
                // lets check if it has a previous time
                if(proc->time != 0){
                    // it has
                    // this will give a perfect time sync
                    proc->time += proc->interval;
                }
                else{
                    // lets use the current time as the starting point
                    proc->time = sch::get_current_time() + proc->interval;
                }
                    
            }
            else{
                // just a single proc_add
                // we dont have a reference
                // lets use the current time as the starting point
                proc->time = sch::get_current_time() + proc->interval;
            }
            
            // if the waiting list is empty
            if(proc_iterator == nullptr){
                // add the process as the first waiting
                proc_list = proc;
                
                // no one is before or after
                proc->prev = nullptr;
                proc->next = nullptr;
            }
            else{
                // scan trough the list
                while(not_done){
                    
                    // lets find our place in the list
                    if((proc_iterator->prev == nullptr) && (proc_iterator->next == nullptr)){
                        // we are at the beginning and the end of the list, simultaneously
                        
                        // check if the proc in the list will be called later
                        if(proc_iterator->time > proc->time){
                            // the proc in the list will be called later
                            
                            // so we insert the proc in the fist slot in the list
                            proc->prev = nullptr;
                            proc->next = proc_iterator;
                            
                            proc_iterator->prev = proc;
                            
                            // and update the list entry point
                            proc_list = proc;
                            
                            // and we are done
                            not_done = false;
                        }
                        else{
                            // the only option is to insert the element
                            // after the last one
                            proc->prev = proc_iterator;
                            proc_iterator->next = proc;
                            
                            // no one is after
                            proc->next = nullptr;
                            
                            // and we are done
                            not_done = false;
                        }
                        
                    }
                    else if(proc_iterator->prev == nullptr){
                        // we are at the beginning of the list
                        
                        // check if the proc in the list will be called later
                        if(proc_iterator->time > proc->time){
                            // the proc in the list will be called later
                            
                            // so we insert the proc in the fist slot in the list
                            proc->prev = nullptr;
                            proc->next = proc_iterator;
                            
                            proc_iterator->prev = proc;
                            
                            // and update the list entry point
                            proc_list = proc;
                            
                            // and we are done
                            not_done = false;
                        }
                        else{
                            // lets go to the next element in the list
                            proc_iterator = proc_iterator->next;
                        }
                    }
                    else if(proc_iterator->next != nullptr){
                        // we are in the middle of the list
                        
                        // check if the proc in the list will be called later
                        if(proc_iterator->time > proc->time){
                            // the proc in the list will be called later
                            
                            // so we insert the proc in the list before the proc_iterator
                            // because the total delay with its value is larger than desired
                            proc->prev = proc_iterator->prev;
                            proc->next = proc_iterator;
                            
                            proc_iterator->prev->next = proc;
                            proc_iterator->prev = proc;
                            
                            // and we are done
                            not_done = false;
                        }
                        else{
                            // lets go to the next element in the list
                            proc_iterator = proc_iterator->next;
                        }
                        
                    }
                    else{
                        // we are at the end of the list
                        
                        // check if the acc delay is longer than the desired value
                        if(proc_iterator->time > proc->time){
                            // it is longer
                            
                            // so we insert the proc in the list before the proc_iterator
                            // because the total delay with its value is larger than desired
                            proc->prev = proc_iterator->prev;
                            proc->next = proc_iterator;
                            
                            proc_iterator->prev->next = proc;
                            proc_iterator->prev = proc;
                            
                            // and we are done
                            not_done = false;
                        }
                        else{
                            // the only option is to insert the element
                            // after the last one
                            proc->prev = proc_iterator;
                            proc_iterator->next = proc;
                            
                            // no one is after
                            proc->next = nullptr;
                            
                            // and we are done
                            not_done = false;
                        }
                        
                    }
                    
                }
                
            }
            
        }
        
    }
    
}

void sch::proc_kill(proc_t * proc){
    sch::remove_item_from_list(proc);
}
 
