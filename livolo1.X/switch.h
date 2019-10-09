#ifndef SWITCH_H
#define	SWITCH_H

#include <xc.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define SWITCH_OFF  0
#define SWITCH_ON   1


uint8_t switch_status[2];

void switch_preinit();
void switch_init();
void switch_toggle(uint8_t n);
//inline bit switch_is_on();
void switch_on(uint8_t n);
void switch_off(uint8_t n);


#ifdef	__cplusplus
}
#endif

#endif	/* SWITCH_H */

