#include "NUC123.h"

#define CPU_CLOCK       48000000

void system_config(void)
{
    SYS_UnlockReg();

    /* Brown-out Detector */
    /* Should already be set from config0 */
    SYS_EnableBOD(SYS_BODCR_BOD_RST_EN, SYS_BODCR_BOD_VL_2_7V);
    SYS_SET_BOD_LPM();

    /* Enable XT1_OUT(PF.0) and XT1_IN(PF.1) */ 
    /* Should already be set from config0 */
    SYS->GPF_MFP &= ~(SYS_GPF_MFP_PF0_Msk | SYS_GPF_MFP_PF1_Msk);
    SYS->GPF_MFP |= SYS_GPF_MFP_PF0_XT1_OUT | SYS_GPF_MFP_PF1_XT1_IN;

    /* Enable external XTAL 12MHz clock */
    /* Should already be set from config0 */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock using PLL, PLLFOUT will be 2 x CPU_CLOCK */
    CLK_SetCoreClock(CPU_CLOCK);

    CLK_SetSysTickClockSrc(CLK_CLKSEL0_STCLK_S_HCLK_DIV2);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable PWM01 module clock */
    CLK_EnableModuleClock(PWM01_MODULE);

    /* Enable ADC module clock */
    CLK_EnableModuleClock(ADC_MODULE);

    /* Enable USB moduke clock*/
    CLK_EnableModuleClock(USBD_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));

    /* Select PWM01 module clock source */
    CLK_SetModuleClock(PWM01_MODULE, CLK_CLKSEL1_PWM01_S_HXT, 0);

    /* ADC clock source is 12MHz, set divider to 3, ADC clock is 12/30 MHz = 400 kHz */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HXT, CLK_CLKDIV_ADC(30));

    /* Select module clock source, PLLFOUT will be 2 x CPU_CLOCK, PLLFOUT / CLK_CLKDIV_USB = 48 MHz  */
    CLK_SetModuleClock(USBD_MODULE, 0, CLK_CLKDIV_USB(2));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC4_Msk | SYS_GPC_MFP_PC5_Msk);
    SYS->GPC_MFP |= SYS_GPC_MFP_PC4_UART0_RXD | SYS_GPC_MFP_PC5_UART0_TXD;
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC4_Msk | SYS_ALT_MFP_PC5_Msk);
    SYS->ALT_MFP |=  SYS_ALT_MFP_PC4_UART0_RXD | SYS_ALT_MFP_PC5_UART0_TXD;

    /* Disable the GPD0 - GPD3 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PD, 0x3F);

    /* Configure the GPD0 - GPD3 ADC analog input pins */
    SYS->GPD_MFP &= ~(SYS_GPD_MFP_PD0_Msk | SYS_GPD_MFP_PD1_Msk | SYS_GPD_MFP_PD2_Msk | SYS_GPD_MFP_PD3_Msk | SYS_GPD_MFP_PD4_Msk | SYS_GPD_MFP_PD5_Msk);
    SYS->GPD_MFP |= SYS_GPD_MFP_PD0_ADC0 | SYS_GPD_MFP_PD1_ADC1 | SYS_GPD_MFP_PD2_ADC2 | SYS_GPD_MFP_PD3_ADC3 | SYS_GPD_MFP_PD4_ADC4 | SYS_GPD_MFP_PD5_ADC5;
    SYS->ALT_MFP1 &= ~(SYS_ALT_MFP1_PD0_Msk | SYS_ALT_MFP1_PD1_Msk | SYS_ALT_MFP1_PD2_Msk | SYS_ALT_MFP1_PD3_Msk | SYS_ALT_MFP1_PD4_Msk | SYS_ALT_MFP1_PD5_Msk);
    SYS->ALT_MFP1 |= SYS_ALT_MFP1_PD0_ADC0 | SYS_ALT_MFP1_PD1_ADC1 | SYS_ALT_MFP1_PD2_ADC2 | SYS_ALT_MFP1_PD3_ADC3 | SYS_ALT_MFP1_PD4_ADC4 | SYS_ALT_MFP1_PD5_ADC5;

    /* Set PF2 and PF3 to GPIO (Acording to manual this should be GPIO after reset. But it isn't?!?!...) */
    SYS->GPF_MFP &= ~(SYS_GPF_MFP_PF2_Msk | SYS_GPF_MFP_PF3_Msk);
    SYS->GPF_MFP |= SYS_GPF_MFP_PF2_GPIO | SYS_GPF_MFP_PF3_GPIO;

    SYS_LockReg();
} 

void system_gpio(void)
{
    /* PORT A */
    GPIO_SetMode(PA, BIT10, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT11, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT15, GPIO_PMD_OUTPUT);

    /* PORT B*/
    GPIO_SetMode(PB, BIT4,  GPIO_PMD_INPUT);
    GPIO_SetMode(PB, BIT5,  GPIO_PMD_INPUT);
    GPIO_SetMode(PB, BIT6,  GPIO_PMD_INPUT);
    GPIO_SetMode(PB, BIT7,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PB, BIT8,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PB, BIT9,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PB, BIT10, GPIO_PMD_INPUT);
    GPIO_EnableInt(PB, 10, GPIO_INT_FALLING); // Enable interrupt for wake up from powerdown
    NVIC_EnableIRQ(GPAB_IRQn);
    GPIO_SetMode(PB, BIT14, GPIO_PMD_OUTPUT);

    /* PORT C */
    GPIO_SetMode(PC, BIT0,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PC, BIT1,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PC, BIT2,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PC, BIT3,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PC, BIT8,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PC, BIT9,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PC, BIT10, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PC, BIT11, GPIO_PMD_OUTPUT);
    
    /* PORT F*/
    GPIO_SetMode(PF, BIT2,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PF, BIT3,  GPIO_PMD_OUTPUT);
}

void system_gpio_powerdown(void)
{

}