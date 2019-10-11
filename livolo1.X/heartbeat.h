#ifndef POWER_H
#define	POWER_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

uint16_t heartbeat_cycles;

void heartbeat_preinit(void);
void heartbeat_update(void);
bit no_50hz(void);


#ifdef	__cplusplus
}
#endif

#endif	/* POWER_H */

