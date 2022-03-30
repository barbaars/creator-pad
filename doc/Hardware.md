# Devine EZ-Creator Pad Hardware


## Components

- Nuvoton NUC123LD4AN0 (Cortex M0)
- Crystal 12 MHz
- 74HC4015 (Analog Multiplexer)
- Leds
- Buttons


### Power Supply

- Everything is powered from the 5V USB supply


### MCU

Nuvoton NUC123LD4AN0 (Cortex M0)

- LQFP 48 pin
- FLASH 68K (or 64K + 4K DataFlash)
- RAM 20K


## IO

| Pin | Function | Name          | Remark                                                                                         |
|-----|----------|---------------|------------------------------------------------------------------------------------------------|
| 1   | Power    | Pvss          |                                                                                                |
| 2   | GPIO     | PB_8          | Output to diode D5. (Switches)                                                                 |
| 3   | GPIO     | PB_14         | Unused output ???                                                                              |
| 4   | GPIO     | PA_11         | Output push-pull to 74HC5051.S2                                                                |
| 5   | GPIO     | PA_10         | Unused output ???                                                                              |
| 6   | GPIO     | PB_4          | Input for Switches                                                                             |
| 7   | GPIO     | PB_5          | Input for Switches                                                                             |
| 8   | GPIO     | PB_6          | Input for Switches                                                                             |
| 9   | GPIO     | PB_7          | Output to diode D6 (Switches)                                                                  |
| 10  | Power    | LDO_CAP       |                                                                                                |
| 11  | Power    | Vdd           |                                                                                                |
| 12  | Power    | Vss           | GND                                                                                            |
| 13  | USB      | USB_VBUS      |                                                                                                |
| 14  | USB      | USB_VDD33_CAP |                                                                                                |
| 15  | USB      | USB_D-        |                                                                                                |
| 16  | USB      | USB_D+        |                                                                                                |
| 17  | UART     | UART0_TXD     |                                                                                                |
| 18  | UART     | UART0_RXD     |                                                                                                |
| 19  | GPIO     | PC_3          | Output to Q5 -> LEDS positive                                                                  |
| 20  | GPIO     | PC_2          | Output to Q4 -> LEDS positive                                                                  |
| 21  | GPIO     | PC_1          | Output to Q3 -> LEDS positive                                                                  |
| 22  | GPIO     | PC_0          | Output to Q2 -> LEDS positive                                                                  |
| 23  | GPIO     | PB_10         | Input with pull up. Markless button left. Why is this sepperate? Interupt enabled in software. |
| 24  | GPIO     | PB_9          | Output to diode D4. (Switches)                                                                 |
| 25  | GPIO     | NC            |                                                                                                |
| 26  | GPIO     | NC            |                                                                                                |
| 27  | GPIO     | PC_11         | Output to LEDS negative                                                                        |
| 28  | GPIO     | PC_10         | Output to LED negative (single led)                                                            |
| 29  | GPIO     | PC_9          | Output to LEDS negative                                                                        |
| 30  | GPIO     | PC_8          | Output to LEDS negative                                                                        |
| 31  | GPIO     | PA_15         | Output to LEDS negative                                                                        |
| 32  | GPIO     | PA_14         | Output to LEDS negative                                                                        |
| 33  | GPIO     | PA_13         | Output to LEDS negative                                                                        |
| 34  | GPIO     | PA_12         | Output to LEDS negative                                                                        |
| 35  | ICE      | ICE_DAT       |                                                                                                |
| 36  | ICE      | ICE_CLK       |                                                                                                |
| 37  | Power    | Avdd          |                                                                                                |
| 38  | ADC      | ADC0          | to 74HC5051.A                                                                                  |
| 39  | ADC      | ADC1          | to connector                                                                                   |
| 40  | ADC      | ADC2          | to connector                                                                                   |
| 41  | ADC      | ADC3          | to connector                                                                                   |
| 42  | ADC      | ADC4          | to connector                                                                                   |
| 43  | ADC      | ADC5          | Volume slider                                                                                  |
| 44  | XTAL     | XT1_OUT       |                                                                                                |
| 45  | XTAL     | XT1_IN        |                                                                                                |
| 46  | RESET    | RESET         |                                                                                                |
| 47  | GPIO     | PF_2          | Output push-pull to 74HC5051.S0                                                                |
| 48  | GPIO     | PF_3          | Output push-pull to 74HC5051.S1                                                                |


## DEBUG1 pin header

1. pwr 5 volt - reset
2. GDN
3. ICE_DAT
4. ICE_CLK


## Summary

Pressure sensitive pads, 4 connected directly to MCU ADC. 8 connected through 74HC5051 analog multiplexer. Last remainig ADC channel is connected to volume slider.

Additonal buttons are multiplexed connected to GPIO, except for "left markless button". 

All leds are multiplexed connected to GPIO, except for led beneath "right markless button".

