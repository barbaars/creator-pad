# Reading the firmware

## SWD

The DEBUG1 port on the PCB is a Serial Wire Debug (SWD) port. Since its a Cortex M processor any CMSIS-DAP adapter should be able to connect. And I have a LPC-Link2 lying around.

Through this port it's possible to read the entire RAM. 

Unfortunately the FLASH is read protected. This read protection is enabled through the *CONFIG* registers. These registers can be read.

```shell
openocd -f openocd/NUCxxx.cfg -c ReadConfigRegs -c exit
```

This returns:

```shell
Open On-Chip Debugger 0.11.0
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
Info : CMSIS-DAP: SWD  Supported
Info : CMSIS-DAP: JTAG Supported
Info : CMSIS-DAP: FW Version = 1.10
Info : CMSIS-DAP: Interface Initialised (SWD)
Info : SWCLK/TCK = 1 SWDIO/TMS = 1 TDI = 1 TDO = 1 nTRST = 0 nRESET = 1
Info : CMSIS-DAP: Interface ready
Info : clock speed 1000 kHz
Info : SWD DPIDR 0x0bb11477
Info : NuMicro.cpu: hardware has 4 breakpoints, 2 watchpoints
Info : starting gdb server for NuMicro.cpu on 3333
Info : Listening on port 3333 for gdb connections
Reading Configuration registers.
Device ID (PDID)    :0x00012335 (NUC123LD4AN0)
target halted due to debug-request, current mode: Thread 
xPSR: 0x21000000 pc: 0x00001f08 msp: 0x20004fc8
Config0 (0x00300000):0xF8FFFFBD
Config1 (0x00300004):0xFFFFFFFF
```

- Config0 (0x00300000):0xF8FFFFBD 
- Config1 (0x00300004):0xFFFFFFFF

Using the Nuvoton documentation to figure out what this means:

```
Config0:0xF8FFFFBD

[0] = 1 -> DFEN, 1 == If DFE there is no Data Flash and all 68 Kbytes size is used for APROM.
[1] = 0 -> Lock bit, 0 == locked
[2] = 1 -> DFVSEN, 1 == The Data Flash is fixed as 4 Kbytes size (Doesn't matter since DFEN == 1)
[3] = 1 -> ???
[4] = 1 -> ???
[5] = 1 -> ???
[6] = 0 -> CBS[0] bootmode APROM with IAP
[7] = 1 -> CBS[1] bootmode APROM with IAP

CBS = b10 = boot from APROM with IAP mode.

[16] = 1
[17] = 1
[18] = 1
[19] = 1
[20] = 1 -> CBORST
[21] = 1 -> CBOV0
[22] = 0 -> CBOV1
[23] = 1 -> CBODEN

Brownout reset at 4.5 volt

[24] = 0 ->  Clock Source Selection 
[25] = 0 ->  Clock Source Selection 
[26] = 0 ->  Clock Source Selection 
[27] = 1 -> XTAL pin used for XTAL
[28] = 1 -> ???
[29] = 1 -> ???
[30] = 1 -> ??? 
[31] = 1 -> ??? 

Clock Source Selection  = b000 = Clock source from external 4~24 MHz high speed crystal clock.


Config1 = DFBADR DataFlash Adress
```

Config0 bit1 == 0 is locked device. Unlocking the device is only possible through a erase and unlock sequence. 

So performing this chip erase sequence will allow us to flash our own firmware. But the origninal firmware will be gone forever and it won't be possible to return the device to its original state.

Challenge accepted! Lets see if it is possible to read the firmware some otherway.


## Google is your friend

 A quick google search turned up this really interesting article https://securelist.com/hacking-microcontroller-firmware-through-a-usb/89919/

 This points to some USB vulnerabilities in the SDK supplied by Nuvoton. 

 I already had an Arduino lying aroud. Bought the USB shield. Lets get to work.


 ## USB Control endpoint

There are a few control request where it is possible to write more then intended by setting a large *wLength* in the USB control transfer *SETUP* packet. However only a write of max 255 bytes is possible. Hmmmm..... that's not enough to reach the top of the stack and perform the same hack as mentioned in the article.

But I'm curious, lets see where in RAM I can write. openocd allows me to check the RAM before and after the buffer overflows.

```
SETUP packet      -> address in RAM from where it is possible to write 255 bytes
34 1 0 1 1280 255 -> 0x2000003c
34 1 0 2 1280 255 -> 0x20000042
34 1 0 1 1536 255 -> 0x2000003d
34 1 0 2 1536 255 -> 0x2000004c
34 1 1 2 1280 255 -> 0x20000040
34 1 1 2 1536 255 -> 0x2000004a
```


## Nuvoton SDK vunuralbilities

The *usbd.c* found in the Nuvoton SDK shows the workings of the standard USB device driver.

There are these global variables: 

```c
static volatile uint8_t *g_usbd_CtrlInPointer = 0;
static volatile uint32_t g_usbd_CtrlInSize = 0;
```

Writing to these values might allow us to dump the complete firmware through USB.

Lets check by sending some USB Control IN transfer requests and checking the RAM using openocd. 

```
0x20000144 -> g_usbd_CtrlInPointer
0x20000148 -> g_usbd_CtrlInSize
```

0x2000004c + 255 = 0x2000014b. PFEWWW THAT IS LUCKY. Just missed the last byte of *g_usbd_CtrlInSize* but who cares; little endian 0xFF 0xFF 0xFF 0x00 is already way larger then 68K (the size of the FLASH). We could even do with 2 bytes less since its possible to alter *g_usbd_CtrlInPointer* and just perform the same hack multiple times.  

We can write 0 to *g_usbd_CtrlInPointer* and a very large number to *g_usbd_CtrlInSize*. Then we need a Control IN transfer that doesn't alter these values and the firmware will drop right out.

And yes luckely there is:

```c
bmRequestType = 0xA2;            // Device to host
bRequest = 0x82;                 // UAC_GET_MIN
wVal_u.wValueLo = 0x00;          
wVal_u.wValueHi = 0x02;          // UAC_VOLUME_CONTROL
wIndex = 0x00;                   // Any value other then 1 or 2
wLength = 0xFFFF;
```

Because of the way the *usbd.c* driver works the third up to and including the eighth byte are not from FLASH. But these missing bytes are easely gathered by using the same hack multiple times with *g_usbd_CtrlInPointer* set to 0x2, 0x4 and 0x6.


## SUCCES

And since there is a separte programmable FLASH area for bootloaders(LDROM) it is possible to easily switch between original firmware and custom firmware.


