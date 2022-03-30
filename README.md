# Devine EZ-Creator Pad USB/MIDI controller firmware replacement

## Goal

Use the Devine EZ-Creator hardware in other hobby projects.

![EZCREATORPAD](https://user-images.githubusercontent.com/31882604/160944692-cd5ef489-566a-4819-a5e7-2bbe6392d9f7.jpg)

## Project

Current software just toggles the LEDS when coresponding button/pad is pressed and outputs analog trace over USB when a pad is hit.

I'm looking for a wireless module and battery which I can hookup in the device. So I can use the device as some kind of remote.


## Build instructions

Its a CMake project, so it should be as simple as:

```
mkdir build
cd build
cmake ..
```

You might want to adjust *toolchain-arm-none-eabi.cmake*.


## Flashing

Program bootloader using openocd and cmsis-dap adapter.

```shell
openocd -f openocd/NUCxxx-cmsis-dap.cfg -c "ChipErase; WriteConfigRegs 0xF8BFFF7F 0xFFFFFFFF; ReadConfigRegs; program build/dfu.bin 0x100000; SysReset ldrom run; exit"
```

Program application using bootloader

```shell
dfu-util -a 0 -d 416:bdf0 -D build/remote_pad.bin
```

From application it is possible to return to bootloader by presssing and holding the two markless buttons. 

## Documentation

See doc folder for some additional documentation. 


## Links

- https://www.nuvoton.com/resource-files/TRM_NUC123_Series_EN_Rev2.04.pdf
- https://openocd.org/
- https://gist.github.com/elfmimi/3b5f8e42c594c670adc1422d7a38ccf7
- https://github.com/OpenNuvoton/

