#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "uart.h"
#include "capsensor.h"
#include "switch.h"
#include "heartbeat.h"
#include "util.h"
#include "extrigger.h"
#include "math.h"

__EEPROM_DATA(0x00, 0b00000000, 20, 5, 0x00, 0x00, 0x00, 0x00); // Fill first 8 bytes of EEPROM

#define VERSION "1.0.0"

/*
 * red blink     | blue blink
 * 1 (btn type)  | 0(push btn), 1(save state)
 * 2 (ON state)  | 0(low), 1(high)     
 * 3 (out type)  | 0(only SET pin 1/0), 1(Livolo type SET/RESET pin)
 */
uint8_t switchMode = 0b00000000; // re-initiated in main())
/*                          0b00000000 - default
 *                                   0 - 1 btn type (press, state)
 *                                  0  - 2 ON state (low/high)
 *                                 0   - 3 out type (only SET, Livolo type)
 */
uint8_t cfgTripThreshold;
uint8_t cfgHystThreshold;
uint8_t cfgDebug = 0;

uint8_t ext_prev = 0;
long unsigned int ext_time = 0;
uint8_t ext_menu = 0;
uint8_t ext_click_type = 0; // 1-short, 2-long
bit ext_in_edit = 0x00;
    
void show_uart_config()
{
    if (micro() % 50) { // each 50ms
        extrigger_read();
        ext_click_type = 0;
        if (extrigger_status == EXT_ON) {
            if (ext_time == 0 && ext_prev != extrigger_status) { // just pressed
                ext_time = micro();
            } else if (ext_time != 0 && (micro() - ext_time) > 500) { // long press
                ext_click_type = 2;
                ext_time = 0;
            }
        } else if (ext_time != 0) { // btn up
            if (ext_time > 100) { // btn was pressed more then 10ms
                ext_click_type = 1;
            }
            ext_time = 0;
        }
        ext_prev = extrigger_status;
        if (ext_click_type != 0) {
            printf("%c%c%c%c",0x1B,0x5B,0x32,0x4A); // clear screen
            printf("\r");
            
            if (ext_in_edit == 0x00) { // list mode
                if (ext_click_type == 1) {
                    ext_menu++;
                    if (ext_menu > 8) {
                        ext_menu = 1;
                    }
                }
            }
            if (ext_click_type == 2) {
                if (ext_in_edit == 0x00) {
                    ext_in_edit = 0x01;
                } else {
                    ext_in_edit = 0x00;
                    eeprom_write(0x01, switchMode);
                    eeprom_write(0x02, cfgTripThreshold);
                    eeprom_write(0x03, cfgHystThreshold);
                    if (ext_menu == 4) {
                        capsensor_init(0);
                        if (CFG_TWO_BTN_SWITCH() == 1) {
                            capsensor_init(1);
                        }
                    }
                }
            }

            switch (ext_menu) {
                case 1:
                    if (ext_in_edit == 0x01 && ext_click_type == 1){
                        switchMode ^= (1 << 0);
                    }
                    printf("1.BUTTON PRESS TYPE: \t\t%s", CFG_BTN_TYPE()? "STATE": "PUSH");
                    break;
                case 2:
                    if (ext_in_edit == 0x01 && ext_click_type == 1){
                        switchMode ^= (1 << 1);
                    }
                    printf("2.OUT PIN STATE WHEN ON: \t%s", CFG_ON_STATE()? "HIGH": "LOW");
                    break;
                case 3:
                    if (ext_in_edit == 0x01 && ext_click_type == 1){
                        switchMode ^= (1 << 2);
                    }
                    printf("3.OUT PIN LOGIC: \t\t%s", CFG_OUT_TYPE()? "LIVOLO": "SET_PIN");
                    break;
                case 4:
                    if (ext_in_edit == 0x01 && ext_click_type == 1){
                        switchMode ^= (1 << 3);
                    }
                    printf("4.SWITCH BUTTONS COUNT: \t%s", CFG_TWO_BTN_SWITCH()? "TWO": "ONE");
                    break;
                case 5:
                    if (ext_in_edit == 0x01 && ext_click_type == 1){
                        cfgTripThreshold++;
                        if (cfgTripThreshold > 50) {
                            cfgTripThreshold = 0;
                        }
                    }
                    printf("5.TRIP THRESHOLD: \t\t%u", cfgTripThreshold);
                    break;
                case 6:
                    if (ext_in_edit == 0x01 && ext_click_type == 1){
                        cfgHystThreshold++;
                        if (cfgHystThreshold >= cfgTripThreshold) {
                            cfgHystThreshold = 0;
                        }
                    }
                    printf("6.RELEASE THRESHOLD: \t\t%u", cfgHystThreshold);
                    break;
                case 7:
                    if (ext_in_edit == 0x01 && ext_click_type == 1){
                        cfgDebug = cfgDebug? 0: 1;
                    }
                    printf("DEBUG: \t\t\t\t%s", cfgDebug? "ON": "OFF");
                    break;
                case 8:
                    if (ext_click_type == 2) {
                        printf("REBOOTED");
                        asm ("GOTO 0"); 
                    } else {
                        printf("REBOOT");
                    }
                    break;
                default:
                    break;
            } 
            if (ext_in_edit == 0x01) {
                printf(" [E]");
            } 

        }
    }
    
//    printf("\r\n");
//    printf("BTN_TYPE=%s\r\n", CFG_BTN_TYPE()? "STATE": "PUSH");
//    printf("ON_STATE=%s\r\n", CFG_ON_STATE()? "HIGH": "LOW");
//    printf("OUT_TYPE=%s\r\n", CFG_OUT_TYPE()? "LIVOLO": "SET_PIN");
}

void main(void) 
{
    switchMode = eeprom_read(0x01);
    cfgTripThreshold = eeprom_read(0x02);
    cfgHystThreshold = eeprom_read(0x03);
    
    // Early inits (before osc is settled)
    switch_preinit();
    heartbeat_preinit();

    /* Tris config (1=in/analog, 0=out)
     * 
     * JP1-CR: C=column R=row where 12v=A7, Vdd=B1, Vss=B2 and so on
     * In brackets [behavior] as observed in the original firmware
     * RF = RF connector on main board
     * J1 = J1 connector on PIC board
     */
    
    TRISA   = 0b11011100;
    // RA0 (19) -------0 <--> JP1-B7, ??? [4 ms high-Z pulse on start, then 0v]
    // RA1 (18) ------0- ---> JP1-A6 ---> RELAY1-S
    // RA2 (17) -----1-- <--- RC4 (6) C2OUT (T0CKI from relaxation osc)
    // RA3 ( 4) ----1--- MCLR only [high-Z]
    // RA4 ( 3) ---1---- <--- JP1-A3 <--- AC POWER HEARTBEAT
    // RA5 ( 2) --0----- ---> JP1-A1 ---> R23 --> 2-way COM TX [3v when red, 0v when blue]
    // unimpl   xx------
    
    TRISB   = 0b00101111;
    // unimpl   ----xxxx
    // RB4 (13) ---0---- ---> JP1-B3 ---> RF5 External feedback OUT (or soft-uart TX)
    // RB5 (12) --1----- <--- JP1-B4 <--- RF4 External feedback IN
    // RB6 (11) -0------ ---> LED1 (0=blue, 1=red)
    // RB7 (10) 0------- <--- JP1-A2 <--- R24 <--- 2-way COM RX (out if unpopulated to save power)
    
    TRISC   = 0b00001010;
    // RC0 (16) -------0 ---> JP1-A4 ---> BUZZER (if any) [4 ms high-Z pulse on start, then 0v]
    // RC1 (15) ------1- C12IN1- (negative feedback of C2), CAP READ 2 (2-gang only)
    // RC2 (14) -----0-- ---> JP1-A5 ---> RELAY2-S (2-gang only)
    // RC3 ( 7) ----1--- C12IN3- (negative feedback of C2), CAP READ 1
    // RC4 ( 6) ---0---- C2OUT
    // RC5 ( 5) --0----- ---> LED2 (0=blue, 1=red) (2-gang only)
    // RC6 ( 8) -0------ ---> JP1-B5 ---> RELAY2-R (2-gang only)
    // RC7 ( 9) 0------- ---> JP1-B6 ---> RELAY1-R

    // All pins digital
    ANSEL   = 0b00000000;
    ANSELH  = 0b00000000;

    // Pull-ups
    WPUA    = 0b00000000;
    // RA0 (19) -------0
    // RA1 (18) ------0-
    // RA2 (17) -----0--
    // RA4 ( 3) ---0----
    // RA5 ( 2) --0-----
    // unimpl   xx--x---

    WPUB    = 0b00100000;
    // unimpl   ----xxxx
    // RB4 (13) ---0----
    // RB5 (12) --1----- yes
    // RB6 (11) -0------
    // RB7 (10) 0-------
    
    nRABPU  = 0; // PORTA/PORTB pull-ups are enabled by individual PORT latch values
            
    // Initial pin values
    PORTA   = 0b00000000;
    PORTB   = 0b00000000;
    PORTC   = 0b00000000;
    
    // Wait until osc is stable
    while (HTS == 0) ;
    
    switch_init();
    capsensor_init(0);
    if (CFG_TWO_BTN_SWITCH() == 1) {
        capsensor_init(1);
    }
    
    // WDT prescaler
    CLRWDT();
//    WDTCON  = 0b00001010;
    WDTCON  = 0b00000110;
    // unimpl   xxx-----
    // WDTPS    ---0100- 1:512  ~ 34/17/11ms @ 15/31/45 KHz LFINTOSC (min/typ/max)
    // WDTPS    ---0101- 1:1024 ~ 68/33/23ms @ 15/31/45 KHz LFINTOSC (min/typ/max)
    // SWDTEN   -------0 WDT is turned off (for now)

    uint8_t prevState[2] = {0,0};
    uint8_t pressed;
    long unsigned int t = micro();

    printf("\r");
    printf("%c%c%c%c",0x1B,0x5B,0x32,0x4A); // clear screen
    printf("\r");
    printf("FIRMWARE VERSION: \t%s", VERSION);
    
    for (;;) {
        show_uart_config();
#ifdef TIME_TO_SHUTDOWN
        heartbeat_update();
//        if (heartbeat_outage()) { // No 50Hz
//            // Switch off only if we're on so as not to energize the coil
//            // pointlessly, which would drain the cap and cause voltage
//            // drops everywhere.
//            switch_off(1);
//            switch_off(2);
//            }
//        }
#endif
        short btnCount = CFG_TWO_BTN_SWITCH()? 2: 1; // Number of switches
        
        for (uint8_t i = 0; i < btnCount; i++) {
            pressed = capsensor_is_button_pressed(i);
            //printf("pressed %u = %u\r\n", i, pressed);
            if (prevState[i] != pressed) {
//                if (NO_SOCKET_MODE == SOCKET_MODE_PUSH) { 
                if (CFG_BTN_TYPE() == 0) { // btn mode push button
                    if (pressed) {
                        switch_on(i);
                    } else { // SOCKET_MODE_TOGGLE
                        switch_off(i);
                    }
                } else if (pressed) { // 1 for toggle button
                    switch_toggle(i);
                }
                prevState[i] = pressed;
            }
        }
        
        if (cfgDebug == 1 && (micro() - t) > 500) {
            t = micro();
            printf("#0 ROL: %u, FROZ: %u, RAW: %u(%i > %i), CYCLES: %u,%u,%u\r\n", 
                cap_rolling_avg[0], 
                cap_frozen_avg[0], 
                cap_raw[0],
                (int16_t)(cap_raw[0] - cap_rolling_avg[0]),
                (int16_t)((cap_rolling_avg[0] / 256) * cfgTripThreshold),
                cap_cycles[0],
                heartbeat_cycles,
                switch_status[0]);
            printf("#1 ROL: %u, FROZ: %u, RAW: %u(%i > %i), CYCLES: %u,%u,%u\r\n", 
                cap_rolling_avg[1], 
                cap_frozen_avg[1], 
                cap_raw[1],
                (int16_t)(cap_raw[1] - cap_rolling_avg[1]),
                (int16_t)((cap_rolling_avg[1] / 256) * cfgTripThreshold),
                cap_cycles[1],
                heartbeat_cycles,
                switch_status[1]);
            printf("\r\n");
        }

        SWDTEN = 1;
        SLEEP();    // clear WDT and sleep
        SWDTEN = 0;
    }
}

