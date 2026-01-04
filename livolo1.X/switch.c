#include <xc.h>
#include <stdint.h>
#include "switch.h"
#include "config.h"
#include "util.h"
#include "heartbeat.h"

extern uint8_t switchMode;

/*
 * Private constants
 */
#define RELAY1_RESET    RC7 // RC7 relay 1 reset (Hongfa HFE60 2-coil latched)
#define RELAY1_SET    RC6 // RC6 relay 1 set

#define RELAY2_RESET    RC2 // RC2 relay 2 reset
#define RELAY2_SET    RA1 // RA1 relay 2 set

/*
 * Public vars
 */

uint8_t switch_status[2];
uint8_t relay_status[2] = {0, 0};

/*
 * Public functions
 */

/**
 * HFE60 datasheet says to not energize both set and reset coils at the
 * same time. Since PORTx output status is unknown on reset, let's pull
 * them down early
 * 
 * If R and S are pulled high at the same time the pic hangs and the whole
 * thing starts drawing a lot of current.
 */
void
switch_preinit()
{
    switch_off(0);
    switch_off(1);
//    RELAY1_RESET = RELAY1_SET = 0;
//    RELAY2_RESET = RELAY2_SET = 0;
//    LED1 = LED2 = LED_RED;
//    switch_status[0] = switch_status[1] = SWITCH_OFF;
}

void
switch_init()
{
//    if (! no_50hz()) { // if switch is working from 220v
//        // Delay until the cap is charged before we can switch for the first time
//        uint16_t t = POWERUP_TIME * 1000UL / 65536;
//        uint8_t pwmt = 0;   // time
//        uint8_t pwmd = 0;   // duty
//        TMR1ON = 0;
//        TMR1 = 0;
//        TMR1ON = 1;
//        while (t) {
//            if (pwmt == pwmd || (pwmt ^ pwmd) == 0xff) LED1 = ~LED1;
//            if (++pwmt == 0) pwmd += POLICE_LIGHTS_FREQ;
//            if (TMR1IF) { 
//                t--;
//                TMR1IF = 0;
//            }
//        }
//        TMR1ON = 0;
//    }
    switch_off(0);
    switch_off(1);
}


void
switch_toggle(uint8_t n)
{
    if (switch_status[n]) {
        switch_off(n);
    } else {
        switch_on(n);
    }
}


void
switch_on(uint8_t n)
{
    switch_status[n] = SWITCH_ON;
    switch (n) {
        case 1:
            if (CFG_OUT_TYPE() == 1) { // livolo type
                RELAY2_SET = 1;
                CLK_125KHZ();
                DELAY_125KHZ(RELAY_OP_TIME);
                RELAY2_SET = 0;
                CLK_4MHZ();
            } else {
                if (RELAY2_SET == 0) {
                    RELAY2_RESET = ! RELAY2_RESET;
                    if (RELAY2_RESET == 1) {
                        CLK_125KHZ();
                        DELAY_125KHZ(RELAY_OP_TIME);
                        CLK_4MHZ();
                    }
                }
                if (RELAY2_SET == 1) {
                    RELAY2_RESET = ! RELAY2_RESET;
                    if (RELAY2_RESET == 0) {
                        CLK_125KHZ();
                        DELAY_125KHZ(RELAY_OP_TIME);
                        CLK_4MHZ();
                    }
                }
                RELAY2_SET = CFG_ON_STATE();
            }
            LED2 = LED_RED;
            break;
        default: // 0
            if (CFG_OUT_TYPE() == 1) { // livolo type
                RELAY1_SET = 1;
                CLK_125KHZ();
                DELAY_125KHZ(RELAY_OP_TIME);
                RELAY1_SET = 0;
                CLK_4MHZ();
            } else {
                CLK_125KHZ();
                RELAY1_RESET = ! RELAY1_RESET;
                if (RELAY1_RESET != RELAY1_SET) {
                    DELAY_125KHZ(RELAY_OP_TIME);
                }

                RELAY1_SET = CFG_ON_STATE();
                DELAY_125KHZ(SWITCH_OP_DELAY);
                CLK_4MHZ();
            }
            LED1 = LED_RED;
            break;
    }
}

void
switch_off(uint8_t n)
{
    switch_status[n] = SWITCH_OFF;
    switch (n) {
        case 1:
            if (CFG_OUT_TYPE() == 1) { // livolo type
                RELAY2_RESET = 1;
                CLK_125KHZ();
                DELAY_125KHZ(RELAY_OP_TIME);
                RELAY2_RESET = 0;
                CLK_4MHZ();
            } else {
                RELAY2_SET = CFG_OFF_STATE();
            }
            LED2 = LED_BLUE;
            break;
        default: // 0
            if (CFG_OUT_TYPE() == 1) { // livolo type
                RELAY1_RESET = 1;
                CLK_125KHZ();
                DELAY_125KHZ(RELAY_OP_TIME);
                RELAY1_RESET = 0;
                CLK_4MHZ();
            } else {
                RELAY1_SET = CFG_OFF_STATE();
            }
            LED1 = LED_BLUE;
            break;
    }
}

