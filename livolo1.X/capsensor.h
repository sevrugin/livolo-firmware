#ifndef CAPSENSOR_H
#define	CAPSENSOR_H

#include <stdint.h>
#include "config.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
uint16_t cap_raw;
uint16_t cap_rolling_avg[2];
uint16_t cap_frozen_avg[2];
uint8_t cap_cycles[2];


/*
 * Public functions
 */    
void capsensor_init(uint8_t n);
bit capsensor_is_button_pressed(uint8_t n);


#ifdef	__cplusplus
}
#endif

#endif	/* CAPSENSOR_H */

