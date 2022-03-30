/******************************************************************************
 * @file     cdc_serial.c
 * @brief    NUC123 series USBD VCOM sample file
 *
 * @note
 * Copyright (C) 2014~2015 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include "cdc_serial.h"

#include <stdio.h>
#include <string.h>

#include "NUC123.h"

#include "usb_out.h"

#include "rbuff.h"
#include "ttime.h"


STR_VCOM_LINE_CODING gLineCoding = {115200, 0, 0, 8};   /* Baud rate,  Stop bit, parity, data bits   */

volatile uint16_t gCtrlSignal = 0;     /* BIT0: DTR(Data Terminal Ready) , BIT1: RTS(Request To Send) */

volatile uint32_t g_u32OutToggle = 0;
volatile uint8_t  g_u8Suspend = 0;

uint8_t gBulkInBuffer[64] = {0};
volatile uint8_t *gpu8RxBuf = 0;
volatile uint32_t gu32RxSize = 0;
volatile uint32_t gu32TxSize = 0;

volatile int8_t gi8BulkOutReady = 0;

#define TX_BUFFER_SZ 512
rbuff VCOM_tx_buffer;
uint8_t VCOM_tx_buffer_mem[TX_BUFFER_SZ];

#define RX_BUFFER_SZ 256
rbuff VCOM_rx_buffer;
uint8_t VCOM_rx_buffer_mem[RX_BUFFER_SZ];

volatile int VCOM_opened = 0;

static volatile unsigned int diag_tx_buffer_overflow;
static volatile unsigned int diag_rx_buffer_overflow;

static void EP2_Handler(void);
static void EP3_Handler(void);
static void EP5_Handler(void);

static void VCOM_closed(void);

/*--------------------------------------------------------------------------*/
void USBD_IRQHandler(void)
{
    uint32_t u32IntSts = USBD_GET_INT_FLAG();
    uint32_t u32State = USBD_GET_BUS_STATE();

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_FLDET)
    {
        // Floating detect
        USBD_CLR_INT_FLAG(USBD_INTSTS_FLDET);

        if(USBD_IS_ATTACHED())
        {
            /* USB Plug In */
            USBD_ENABLE_USB();
        }
        else
        {
            /* USB Un-plug */
            USBD_DISABLE_USB();

            gCtrlSignal = 0;
        }
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_WAKEUP)
    {
        /* Clear event flag */
        USBD_CLR_INT_FLAG(USBD_INTSTS_WAKEUP);
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_BUS)
    {
        /* Clear event flag */
        USBD_CLR_INT_FLAG(USBD_INTSTS_BUS);

        if(u32State & USBD_STATE_USBRST)
        {
            /* Bus reset */
            USBD_ENABLE_USB();
            USBD_SwReset();
            g_u32OutToggle = 0;
            g_u8Suspend = 0;
        }
        if(u32State & USBD_STATE_SUSPEND)
        {
            /* Enter power down to wait USB attached */
            g_u8Suspend = 1;

            /* Enable USB but disable PHY */
            USBD_DISABLE_PHY();
        }
        if(u32State & USBD_STATE_RESUME)
        {
            /* Enable USB and enable PHY */
            USBD_ENABLE_USB();
            g_u8Suspend = 0;
        }
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_USB)
    {
        extern uint8_t g_usbd_SetupPacket[];

        // USB event
        if(u32IntSts & USBD_INTSTS_SETUP)
        {
            // Setup packet
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP);

            /* Clear the data IN/OUT ready flag of control end-points */
            USBD_STOP_TRANSACTION(EP0);
            USBD_STOP_TRANSACTION(EP1);

            USBD_ProcessSetupPacket();
        }

        // EP events
        if(u32IntSts & USBD_INTSTS_EP0)
        {

            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP0);

            // control IN
            USBD_CtrlIn();
        }

        if(u32IntSts & USBD_INTSTS_EP1)
        {

            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP1);

            // control OUT
            USBD_CtrlOut();

            // In ACK of SET_LINE_CODE
            if(g_usbd_SetupPacket[1] == SET_LINE_CODE)
            {
                //if(g_usbd_SetupPacket[4] == 0)  /* VCOM-1 */
                //    VCOM_LineCoding(0); /* Apply UART settings */
            }

        }

        if(u32IntSts & USBD_INTSTS_EP2)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP2);
            // Bulk IN
            EP2_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP3)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP3);
            // Bulk Out
            EP3_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP4)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP4);
        }

        if(u32IntSts & USBD_INTSTS_EP5)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP5);
            // Bulk IN
            EP5_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP6)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP6);
        }

        if(u32IntSts & USBD_INTSTS_EP7)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP7);
        }
    }

    event_publish(EVENT_USB);
}

static void EP2_Handler(void)
{
    /* Bulk IN - Output from device */
    gu32TxSize = 0;

}

static void EP3_Handler(void)
{
    /* Bulk OUT - Input to device **/
    if(g_u32OutToggle == (USBD->EPSTS & USBD_EPSTS_EPSTS3_Msk))
    {
        USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);
    }
    else
    {
        gu32RxSize = USBD_GET_PAYLOAD_LEN(EP3);
        gpu8RxBuf = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP3));

        g_u32OutToggle = USBD->EPSTS & USBD_EPSTS_EPSTS3_Msk;
        /* Set a flag to indicate bulk out ready */
        gi8BulkOutReady = 1;
    }
}

static void EP5_Handler(void)
{
    /* Bulk IN - Output from device */
    // output trace

    usb_out_transmitting();
    

}

/*--------------------------------------------------------------------------*/
/**
  * @brief  USBD Endpoint Config.
  * @param  None.
  * @retval None.
  */
void VCOM_Init(void)
{
    /* Init setup packet buffer */
    /* Buffer for setup packet -> [0 ~ 0x7] */
    USBD->STBUFSEG = SETUP_BUF_BASE;

    /*****************************************************/
    /* EP0 ==> control IN endpoint, address 0 */
    USBD_CONFIG_EP(EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0);
    /* Buffer range for EP0 */
    USBD_SET_EP_BUF_ADDR(EP0, EP0_BUF_BASE);

    /* EP1 ==> control OUT endpoint, address 0 */
    USBD_CONFIG_EP(EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0);
    /* Buffer range for EP1 */
    USBD_SET_EP_BUF_ADDR(EP1, EP1_BUF_BASE);

    /*****************************************************/
    /* EP2 ==> Bulk IN endpoint, address 1 */
    USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | BULK_IN_EP_NUM);
    /* Buffer offset for EP2 */
    USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_BASE);

    /* EP3 ==> Bulk Out endpoint, address 2 */
    USBD_CONFIG_EP(EP3, USBD_CFG_EPMODE_OUT | BULK_OUT_EP_NUM);
    /* Buffer offset for EP3 */
    USBD_SET_EP_BUF_ADDR(EP3, EP3_BUF_BASE);
    /* trigger receive OUT data */
    USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);

    /* EP4 ==> Interrupt IN endpoint, address 3 */
    USBD_CONFIG_EP(EP4, USBD_CFG_EPMODE_IN | INT_IN_EP_NUM);
    /* Buffer offset for EP4 ->  */
    USBD_SET_EP_BUF_ADDR(EP4, EP4_BUF_BASE);

    /* EP5 ==> Bulk IN endpoint, address 4 */
    USBD_CONFIG_EP(EP5, USBD_CFG_EPMODE_IN | VENDOR_BULK_IN_EP_NUM);
    /* Buffer offset for EP5 */
    USBD_SET_EP_BUF_ADDR(EP5, EP5_BUF_BASE);

    /* Custom / Vendor endpoint */
    usb_out_init();


    rbuff_init(&VCOM_tx_buffer, VCOM_tx_buffer_mem, TX_BUFFER_SZ);
    rbuff_init(&VCOM_rx_buffer, VCOM_rx_buffer_mem, RX_BUFFER_SZ);

    event_subscribe(EVENT_CYCLIC_1, VCOM_Task);
}

void VCOM_ClassRequest(void)
{
    uint8_t buf[8];

    USBD_GetSetupPacket(buf);

    if(buf[0] & 0x80)    /* request data transfer direction */
    {
        // Device to host
        switch(buf[1])
        {
            case GET_LINE_CODE:
            {
                if(buf[4] == 0)    /* VCOM-1 */
                {
                    USBD_MemCopy((uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)), (uint8_t *)&gLineCoding, 7);
                }
                /* Data stage */
                USBD_SET_DATA1(EP0);
                USBD_SET_PAYLOAD_LEN(EP0, 7);
                /* Status stage */
                USBD_PrepareCtrlOut(0, 0);
                break;
            }
            default:
            {
                /* Setup error, stall the device */
                USBD_SetStall(EP0);
                USBD_SetStall(EP1);
                break;
            }
        }
    }
    else
    {
        // Host to device
        switch(buf[1])
        {
            case SET_CONTROL_LINE_STATE:
            {
                if(buf[4] == 0)    /* VCOM-1 */
                {
                    gCtrlSignal = buf[3];
                    gCtrlSignal = (gCtrlSignal << 8) | buf[2];
                    //printf("RTS=%d  DTR=%d\n", (gCtrlSignal0 >> 1) & 1, gCtrlSignal0 & 1);
                }

                /* Status stage */
                USBD_SET_DATA1(EP0);
                USBD_SET_PAYLOAD_LEN(EP0, 0);
                break;
            }
            case SET_LINE_CODE:
            {
                if(buf[4] == 0)  /* VCOM-1 */
                {
                    USBD_PrepareCtrlOut((uint8_t *)&gLineCoding, 7);

                }

                /* Status stage */
                USBD_SET_DATA1(EP0);
                USBD_SET_PAYLOAD_LEN(EP0, 0);

                break;
            }
            default:
            {
                // Stall
                /* Setup error, stall the device */
                USBD_SetStall(EP0);
                USBD_SetStall(EP1);
                break;
            }
        }
    }
}

int VCOM_Send(uint8_t *data, int len)
{
    for (int i=0; i<len; i++)
    {
        int result = rbuff_put_byte(&VCOM_tx_buffer, data[i]);
        if (result != 0)
            return -1;
    }

    return 0;
}

void VCOM_TransferData(void)
{
    if ((gCtrlSignal & 1) && (!VCOM_opened))
    {
        VCOM_opened = 1;
        rbuff_clear(&VCOM_tx_buffer);
        event_publish(EVENT_USB_VCOM_OPENED);
    }
    else if ((!(gCtrlSignal & 1)) && VCOM_opened)
    {
        VCOM_opened = 0;
        VCOM_closed();
    }

    /* Check whether USB is ready for next packet or not */
    if(gu32TxSize == 0 && VCOM_opened)
    {
        /* Check whether we have new COM Rx data to send to USB or not */
        if(VCOM_tx_buffer.size)
        {
            int i32Len = VCOM_tx_buffer.size;
            if(i32Len > EP2_MAX_PKT_SIZE)
                i32Len = EP2_MAX_PKT_SIZE;

            for(int i=0; i<i32Len; i++)
            {
                if (rbuff_get_byte(&VCOM_tx_buffer, &gBulkInBuffer[i]))
                {
                    // Error
                    diag_tx_buffer_overflow++;
                }
            }

            gu32TxSize = i32Len;
            USBD_MemCopy((uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2)), (uint8_t *)gBulkInBuffer, i32Len);
            USBD_SET_PAYLOAD_LEN(EP2, i32Len);
        }
        else
        {
            /* Prepare a zero packet if previous packet size is EP2_MAX_PKT_SIZE and 
               no more data to send at this moment to note Host the transfer has been done */
            int i32Len = USBD_GET_PAYLOAD_LEN(EP2);
            if(i32Len == EP2_MAX_PKT_SIZE)
                USBD_SET_PAYLOAD_LEN(EP2, 0);
        }
    }

    /* Process the Bulk out data when bulk out data is ready. */
    if (gi8BulkOutReady && (gu32RxSize <= VCOM_rx_buffer.max - VCOM_rx_buffer.size))
    {
        // read incomming data here
        for(int i=0; i<gu32RxSize; i++)
        {
            if (rbuff_put_byte(&VCOM_rx_buffer, gpu8RxBuf[i]))
            {
                // Error
                diag_rx_buffer_overflow++;;
            }
        }
        event_publish(EVENT_USB_VCOM_RX);

        gu32RxSize = 0;
        gi8BulkOutReady = 0; /* Clear bulk out ready flag */

        /* Ready to get next BULK out */
        USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);
    }
}

void VCOM_Task(enum events event, int ticks)
{
    VCOM_TransferData();
}

static void VCOM_closed(void)
{
    gu32TxSize = 0;
    USBD_SET_PAYLOAD_LEN(EP2, 0);
    rbuff_clear(&VCOM_tx_buffer);
    VCOM_opened = 0;
    event_publish(EVENT_USB_VCOM_CLOSED);
}

#if defined (STDDRIVER_RETARGET_USER_OVERRIDE)

int _read(int fd, char *ptr, int len)
{   
    if (len <= 0)
        return 0;
    
    int count = 0;
    while(--len)
    {
        int result = rbuff_get_byte(&VCOM_rx_buffer, (uint8_t *)ptr);
        if (result)
            return count;
        count++;
    }

    return count;
}

int _write(int fd, char *ptr, int len)
{
    if (!VCOM_opened)
        return 0;

    int i = len;
    while(i--)
    {
        rbuff_put_byte(&VCOM_tx_buffer, *ptr++);
	}
    return len;
}

#endif
