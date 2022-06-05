/* 
 * File:   extrigger.h
 * Author: javi
 *
 * Created on July 29, 2017, 2:56 PM
 */

#ifndef EXTRIGGER_H
#define	EXTRIGGER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define EXT_ON   0
#define EXT_OFF  1
    
uint8_t extrigger_status;

uint8_t extrigger_read(void);



#ifdef	__cplusplus
}
#endif

#endif	/* EXTRIGGER_H */

