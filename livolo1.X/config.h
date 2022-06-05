#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------
// Compilation flags
// --------------------------------------------------------------------
#define SOCKET_MODE_PUSH 0
#define SOCKET_MODE_TOGGLE 1


// Enable debug serial out, note this also slows down time between sensor reads
//#define DEBUG
    
// --------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------

#define CFG_BTN_TYPE() (switchMode & (1 << 0)? 1: 0)
    
#define CFG_ON_STATE() (switchMode & (1 << 1)? 1: 0)
#define CFG_OFF_STATE() (switchMode & (1 << 1)? 0: 1)
    
#define CFG_OUT_TYPE() (switchMode & (1 << 2)? 1: 0)
    
#define CFG_TWO_BTN_SWITCH() (switchMode & (1 << 3)? 1: 0)

    
/*
 * Relay switch type
 * 0 - (Livolo default) use relay op time to on/off relay (RELAY_SET/RELAY_RESET pins.
 * 1 - use only RELAY_SET pin to on/off relay
 */
#define RELAY_SWITCH_TYPE         1  

    // Trip threshold in 1/256 fractions of the average frequency
// working (empirical) values: 3 for 3V
//#define TRIP_THRESHOLD      20
    //switched to "cfgTripThreshold"

// Release threshold in 1/256 fractions of the average frequency
//#define HYST_THRESHOLD      5
    //switched to "cfgHystThreshold"

// Min sensor trips to actually switch
#define READS_TO_SWITCH     1

// If sensor is not released after RELEASE_TIMEOUT cycles, the last frequency
// is assumed to be the new condition, eg. if dropped some water on the plate.
// 180 = ~3s at 16.384 ms per cycle. 60 = 1sec
#define RELEASE_TIMEOUT     60 * 60

// Space out averages 1 every AVERAGING_RATE raw values. Should be a power of
// 2 optimally
#define AVERAGING_RATE      5

// Time to shutdown the relays after an outage is detected. In read cycles.
// Undefine to disable outage detection completely to help debugging.
#define TIME_TO_SHUTDOWN    20

// Relay operation time (ms), must be >recommended max spec (10 ms), see
// Hongfa HFE60 datasheet.
#define RELAY_OP_TIME       15

// Power up delay: time to charge the cap before the relays can switch for
// the first time. In ms.
#define POWERUP_TIME        2000

// Police lights frequency controls the speed of the powerup light sequence.
// Must be a power of 2 <=128. Hint: 4 in XC8 free mode, 1 in pro mode.
#define POLICE_LIGHTS_FREQ  4
   

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */
