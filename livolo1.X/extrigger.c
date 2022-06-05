#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "extrigger.h"
#include "config.h"


/*
 * Private consts
 */
#define EXT_TRIG    RB5    


/*
 * Private vars
 */
static bit previous_state;


/*
 * Public vars
 */
uint8_t extrigger_status;


/*
 * Public functions
 */
uint8_t
extrigger_read(void)
{
//    extrigger_status = previous_state;    // use as tmp
//    previous_state = EXT_TRIG;
//    extrigger_status = (extrigger_status && !previous_state);
//    return extrigger_status;
    extrigger_status = (uint8_t)EXT_TRIG;
    
    return extrigger_status;
}

