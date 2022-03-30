# configA

### config0 register

- Brownout reset at 2.7 volt.
- Clock external 12MHz crystal.
- No dataflash. 68Kbytes flash.
- No Lockbit.
- bootmode APROM with IAP.

```
openocd -f openocd/NUCxxx.cfg -c "WriteConfigRegs 0xF8BFFFBF 0xFFFFFFFF" -c ReadConfigRegs -c exit
```
