#include "powerdown.h"

#include "NUC123.h"

#include "system.h"

void powerdown(void)
{
    uint32_t pmask = __get_PRIMASK();
    __disable_irq();

    /* Set the processor uses deep sleep as its low power mode */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Set system Power-down enabled and Power-down entry condition */
    SYS_UnlockReg();
    CLK->PWRCON |= (CLK_PWRCON_PWR_DOWN_EN_Msk | CLK_PWRCON_PD_WAIT_CPU_Msk);
    SYS_LockReg();

    /* GPIO to powerdown state, enable wake up interupt*/
    system_gpio_powerdown();

    /* Chip enter Power-down mode after CPU run WFI instruction */
    __DSB(); // is this neccessary ?
    __WFI();
    __ISB(); // is this neccessary ?

    /* Restore GPIO */
    system_gpio();

    /* Restore sleep
       Set the processor uses sleep as its low power mode */
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    /* Set chip in idle mode because of WFI command */
    SYS_UnlockReg();
    CLK->PWRCON &= ~CLK_PWRCON_PWR_DOWN_EN_Msk;
    SYS_LockReg();

    if (!pmask)
        __enable_irq();
}
