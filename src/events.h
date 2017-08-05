

#ifndef _events_H
#define _events_H


typedef int (*tileless_event_func_in_func)(void *ctrl, void*);

typedef int (*tileless_event_function)(void *ctrl, void*, tileless_event_func_in_func func_in_func);




typedef struct
{
    void *caller; //caller controll
    void *data;
    size_t data_len;
    tileless_event_function te_func;
    tileless_event_func_in_func te_func_in_func;

} tileless_event;


#endif
