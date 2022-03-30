#include "NUC123.h"
#include "fmc_user.h"
#include "dfu_transfer.h"

#include "io_map.h"

#define PLLCON_SETTING          CLK_PLLCON_144MHz_HXT
#define PLL_CLOCK               144000000

#define V6M_AIRCR_VECTKEY_DATA    0x05FA0000UL
#define V6M_AIRCR_SYSRESETREQ     0x00000004UL

uint32_t g_apromSize;

uint32_t GetApromSize()
{
    // The smallest of APROM size is 2K
    uint32_t size = 0x800, data;
    int result;

    do
    {
        result = FMC_Read_User(size, &data);

        if(result < 0)
        {
            return size;
        }
        else
        {
            size *= 2;
        }
    }
    while(1);
}

void SYS_Init(void)
{
    /* Enable XT1_OUT (PF.0) and XT1_IN (PF.1) */
    SYS->GPF_MFP &= ~(SYS_GPF_MFP_PF0_Msk | SYS_GPF_MFP_PF1_Msk);
    SYS->GPF_MFP |= SYS_GPF_MFP_PF0_XT1_OUT | SYS_GPF_MFP_PF1_XT1_IN;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK->PWRCON |= CLK_PWRCON_OSC22M_EN_Msk;

    /* Waiting for Internal RC clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_OSC22M_STB_Msk));

    /* Enable external XTAL 12 MHz clock */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Waiting for external XTAL clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Select HCLK clock source as HIRC and HCLK clock divider as 1 */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLK_S_Msk)) | CLK_CLKSEL0_HCLK_S_HIRC;
    CLK->CLKDIV = (CLK->CLKDIV & (~CLK_CLKDIV_HCLK_N_Msk)) | CLK_CLKDIV_HCLK(1);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK->PLLCON = PLLCON_SETTING;

    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));

    CLK->CLKDIV = (CLK->CLKDIV & (~CLK_CLKDIV_HCLK_N_Msk)) | CLK_CLKDIV_HCLK(2);
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLK_S_Msk)) | CLK_CLKSEL0_HCLK_S_PLL;

    /* Update System Core Clock */
    PllClock        = PLL_CLOCK;                // PLL
    SystemCoreClock = PLL_CLOCK / 2;            // HCLK
    CyclesPerUs     = PLL_CLOCK / 2 / 1000000;  // For CLK_SysTickDelay()

    /* Enable module clock */
    CLK->APBCLK |= CLK_APBCLK_USBD_EN_Msk | CLK_APBCLK_TMR0_EN_Msk;
    CLK->CLKDIV = (CLK->CLKDIV & (~CLK_CLKDIV_USB_N_Msk)) | CLK_CLKDIV_USB(3);

    /* Peripheral clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_TMR0_S_HXT;

    /* Disable Internal RC 22.1184MHz clock */
    CLK->PWRCON &= ~(CLK_PWRCON_OSC22M_EN_Msk);
}

void USBD_IRQHandler(void);

void timer_setup(void)
{
    TIMER_SET_PRESCALE_VALUE(TIMER0, 12);
    TIMER0->TCSR |= TIMER_CONTINUOUS_MODE;
    TIMER_Start(TIMER0);
}

uint32_t timer_delta(uint32_t *previous_time)
{
    uint32_t now = TIMER0->TDR;
    uint32_t delta = (now-*previous_time) & 0xFFFFFF;
    *previous_time = now;

    return delta;
}

#define DEBOUNCE_TIME 50000 // us
int key_debounce(int reset)
{
    static int DetectPinMemory;
    static uint32_t prev = 0;
    static uint32_t time_held = 0;

    uint32_t delta = timer_delta(&prev);
    if (KEY_INT == DetectPinMemory && !reset)
        time_held += delta;
    else
        time_held = 0;

    DetectPinMemory = KEY_INT;

    if (time_held > DEBOUNCE_TIME)
    {
        time_held = DEBOUNCE_TIME;
        return KEY_INT;
    }

    return -1;
}

__STATIC_INLINE void led_setup(void)
{
    PC->PMD = (PC->PMD & (~GPIO_PMD_PMD10_Msk)) | (GPIO_PMD_OUTPUT << GPIO_PMD_PMD10_Pos);
}

void led_blink(void)
{
    static uint32_t wait_time = 0;
    static uint32_t prev = 0;

    uint32_t delta = timer_delta(&prev);
    wait_time += delta;
    if (wait_time > 500000)
    {
        wait_time = 0;
        LED_COLUMN_6 ^= 1;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    /* Unlock write-protected registers */
    SYS_UnlockReg();

    /* Pressed */
    if (KEY_INT == 0)
    {
        /* Init system and multi-funcition I/O */
        SYS_Init();

        CLK->AHBCLK |= CLK_AHBCLK_ISP_EN_Msk;
        FMC->ISPCON |= FMC_ISPCON_ISPEN_Msk | FMC_ISPCON_APUEN_Msk | FMC_ISPCON_ISPFF_Msk;
        g_apromSize = GetApromSize();

        /* Timer */
        timer_setup();

        /* Open USB controller */
        USBD_Open(&gsInfo, DFU_ClassRequest, NULL);

        /* Init Endpoint configuration for DFU */
        DFU_Init();

        /* Start USB device */
        USBD_Start();

        /* For led blink */
        led_setup();

        int key_memory;
        key_debounce(1); // reset;
        while ((key_memory=key_debounce(0)) == -1);

        int key = key_memory;
        while (!(key_memory==1 && key==0))
        {
            led_blink();

            USBD_IRQHandler();

            key_memory = key; // Remember key state
            int result = key_debounce(0);
            if (result != -1)
                key = result;
        }
    }

    /* Reset to aprom */
    SYS->RSTSRC = (SYS_RSTSRC_RSTS_POR_Msk | SYS_RSTSRC_RSTS_RESET_Msk);//clear bit
    FMC->ISPCON &=  ~(FMC_ISPCON_ISPEN_Msk | FMC_ISPCON_BS_Msk);
    SCB->AIRCR = (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ);

    /* Trap the CPU */
    while(1);

    return 0;
}
