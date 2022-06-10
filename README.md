# Firmware for Livolo light switches

HUGE thanks to the original repository!!! https://github.com/jamarju/livolo-firmware
This is the fork.

Livolo sensor board is based on **PIC16F690**(C0/C1 revision) microchip or **PIC16lF1559**(D0/D1 revision marked as `5VL599006  LIVOLO 1947725` which not supported at this time)

My goal was to make a new firmware to use it for smart home.

I'm using it to operate another relay into my electrical shield.

In my ideal world the switch should:
1. Be able to work as `PUSH` button or as `STATE` button
2. Be able to switch between `HI/LO` output modes
3. Be able to work with onboard relay (pulse signal to SET pin to switch ON relay, pulse signal to RES pin to switch OFF relay), or just toggle SET pin to HI/LO (to use it just as a signal wire)
4. Be able to work with ONW/TWO button switches
5. Be configurable

## Demo

This is how to the switch works on PUSH/STATE modes. 

Switch connected to 12v supply and using LEDs as output demonstration (you can toggle the output state for ON/OFF mode by configurations)  

[![Livolo control video](https://i9.ytimg.com/vi/Xp0Ba5l0h7c/mq2.jpg?sqp=CID0i5UG&rs=AOn4CLCO5fgaO9oXm9slgYmzUaaqMRKHIQ)](https://youtu.be/Xp0Ba5l0h7c)
[![Livolo control video](https://i9.ytimg.com/vi/i4VSr7Syjqk/mq2.jpg?sqp=CID0i5UG&rs=AOn4CLAtvMt8t85b4vcV3bc63mrPrDOtPw)](https://youtu.be/i4VSr7Syjqk)
 
## Schematic

This is the switch board pinout schematic

<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/C701X.png?raw=true" width="500" alt="">


## Wiring

To use the switch bypass 220v you can connect it to 12v power supply:

<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/photo_2022-06-10_10-25-05.jpg?raw=true" width="500" alt="">
<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/photo_2022-06-10_10-25-10.jpg?raw=true" width="500" alt="">
<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/photo_2022-06-10_10-25-14.jpg?raw=true" width="500" alt="">
<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/photo_2022-06-10_10-25-19.jpg?raw=true" width="500" alt="">


## Configuring

The Firmware stores his own configuration into EEPROM and might be changed by EXTERNAL button connected between EXT and GND contacts.

I highly recommend connecting the switch by UART to see the configuration menu
`screen /dev/ttyUSB0 57600`

<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/photo_2022-06-10_10-24-35.jpg?raw=true" width="500" alt="">
<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/photo_2022-06-10_10-25-01.jpg?raw=true" width="500" alt="">

Clicking the external button you will be able to switch between the next menu options:
```
VERSION                     v1.0.0
1.BUTTON PRESS TYPE:        STATE/PUSH
2.OUT PIN STATE WHEN ON:    HIGH/LOW
3.OUT PIN LOGIC:            LIVOLO/SET_PIN
4.SWITCH BUTTONS COUNT:     TWO/ONE
5.TRIP THRESHOLD:           20
6.RELEASE THRESHOLD:        5
DEBUG:                      OFF/ON
REBOOT
```

To enter into `edit mode` long press the EXT button and you see an additional `[E]` symbol

```
1.BUTTON PRESS TYPE:        STATE [E]
```

to change the option short click the EXT button
```
1.BUTTON PRESS TYPE:        PUSH [E]
```

long click the EXT button to exit from `edit mode` 

_Note_: The RED led on sensor panel blinks N times (menu number) when you switch between menus. When you enter into edit mode it glows RED.

## Debug

When entering the `DEBUG` mode, `raw` data from the sensors is displayed.

## Reset

I highly recommend to `REBOOT` the switch after any modification of configuration data 

## Additional files

<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/livolo-circuit.png?raw=true" width="500" alt="">

<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/livolo-serial.png?raw=true" width="500" alt="">

<img src="https://github.com/sevrugin/livolo-firmware/blob/master/img/touch-circuit.png?raw=true" width="500" alt="">

## Pull requests welcome 

This is work in progress. I plan to use this and I don't want to set my house on fire, so either if you want to study how the switch works internally or fork it to further work on it, I'd love to hear from suggestions or improvements!

## Supported boards:

 * [VL-C701X-2](https://www.aliexpress.com/item/Free-Shipping-Livolo-Luxury-White-Crystal-Glass-Switch-Panel-EU-Standard-VL-C701-11-110-250V/512886492.html) VER: C0 (1-gang, 1-way, no RF, EU version)

Should be easy to work with this, too:

 * [VL-C702X-2](https://www.aliexpress.com/item/Free-Shipping-White-Crystal-Glass-Switch-Panel-EU-Standard-VL-C702-11-Livolo-AC-110-250V/512770913.html) VER: C1 (2-gang, 1-way, no RF, EU version)

Note that there is no guarantee that buying from these links will get you those versions since they seem to change designs often.

## Working principle

The Livolo switches use a PIC16F690 to detect slight capacitance changes on a glass plate when a finger is pressed against it. The working principle is described in Microchip's AN1101 (recommended read). The differences with regards to the application note are:

 * The output of the C2 comparator is connected to the clock input of Timer 0 instead of Timer 1.
 * The 1/3 voltage divider is replaced with the internal 0.6V reference.
 * The 2/3 voltage divider is replaced with the internal VR reference, which can be configured via VRCON.

## How to use

Once you flash the PIC, there is no coming back!

The Livolo PIC has its CP bit on, which means its flash is unreadable, and thus you can't make a backup copy of the pre-installed firmware. 

Because of this, I recommend replacing the PIC with a blank one. This way you still have a working chip with the original behavior to roll back to.

You need MPLAB X IDE + XC8 compiler. Load up the project, compile and flash.

`config.h` contains a few flags that can be finetuned, ie. the trip and release threshold values.

**Important**! Set in project configuration Pack: PIC16Fxxx_DFP = 1.0.9 AND XC8 = 1.45

## Serial connection

Okay, first off the ground traces of the Livolo board (including PIC's GND) are actually referenced to the live wire. **Repeat: GND IS ACTUALLY LIVE, so the challenge here is to be able to talk to a PC without electrocuting yourself**. 

In order to pull data safely from the circuit there are two options:

### Battery + inverter / isolation transformer

I won't go into much detail because you should know what you are doing if you want to play with 220V. But if you want to get serial data out of the PIC or just use an oscilloscope to probe the signals, one option is to isolate the Livolo power from the mains supply.

I used a cheap (150W) car battery inverter powered by a 3S LiPo because that's what I had at hand. You can use an isolation transformer as well. If you reference the PIC's Vss to ground via the scope's alligator clip, the whole board now becomes referenced to the real ground and it's a bit safer to play around with. Just don't touch the live and neutral wires of the Livolo.

### Optocouplers

I also tried this, but personally I find the battery + inverter easier and safer while debugging.

Anyhow, the idea is to use optocouplers. But apparently in the optocouplers world you can only have two of a) cheap, b) low current, c) fast. I had a few 6N137 at hand so I used that. They are cheap, they are fast, but they swallow too much current for Livolo's puny power supply.

So I had to add a battery to provide the extra oomph to light up the diode of one of the OCs and drive the collector of the other, yuck!

![schematic](img/livolo-serial.png)

Of course this is only valid for debugging to get data out of the PIC. The battery thing makes this unfeasible for a permanent installation and I'm still looking for other solutions.

Note that I had to bitbang pin RB4 on the PIC for TX out because that PIN is exposed on the empty J6 header along with RX and Vss. That way we have everything we need in J6, which it's through hole and easier to solder on.

If you choose to use the real TX pin, you can track it down to a couple of empty resistor pads, but I found it harder to solder a wire on an SMD pad securely and thus I preferred the bitbang way even if it makes transmission slower.