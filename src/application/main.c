#include "NUC123.h"

#include "system.h"

#include "cdc_serial.h"
#include "keys.h"
#include "leds.h"
#include "analog_inputs.h"

#include "scheduler.h"
#include "shell.h"
#include "pads.h"
#include "led_toggle.h"
#include "programs.h"



void UART0_Init()
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

int main(void)
{
    system_config();
    system_gpio();

    UART0_Init();

    /* Open USB controller */
    USBD_Open(&gsInfo, VCOM_ClassRequest, NULL);

    /* Endpoint configuration */
    VCOM_Init();

    /* Start USB device */
    USBD_Start();
    NVIC_EnableIRQ(USBD_IRQn);

    keys_task_init();
    leds_task_init();
    analog_inputs_init();

    pads_as_key_task_init();
    led_toggle_init();

    bootloader_check_task_init();
    knightrider_task_init();
    cpu_load_task_init();

    shell_init();
    scheduler_loop();

    /* We should never return from scheduler_loop() */
    while(1);

    return 0;
}
