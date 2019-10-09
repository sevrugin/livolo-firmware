#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------
// Compilation flags
// --------------------------------------------------------------------

// Enable debug serial out, note this also slows down time between sensor reads
#define DEBUG

// --------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------

//#define TWO_WAY_SWITCH // Enable for two-way switch
    
#define NO_SOCKET_STATE     0 // How-to wotk if no 50Hz (0 - push, 1 - toggle)
    
// Trip threshold in 1/256 fractions of the average frequency
// working (empirical) values: 3 for 3V
#define TRIP_THRESHOLD      3

// Release threshold in 1/256 fractions of the average frequency
#define HYST_THRESHOLD      2

// Min sensor trips to actually switch
#define READS_TO_SWITCH     1

// If sensor is not released after RELEASE_TIMEOUT cycles, the last frequency
// is assumed to be the new condition, eg. if dropped some water on the plate.
// 180 = ~3s at 16.384 ms per cycle.
#define RELEASE_TIMEOUT     180

// Space out averages 1 every AVERAGING_RATE raw values. Should be a power of
// 2 optimally
#define AVERAGING_RATE      2

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
