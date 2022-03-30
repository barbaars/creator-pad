#include "usb_out.h"

#include "NUC123.h"

#include "usbd.h"

volatile uint8_t *usb_out_tx_ptr;
volatile uint32_t usb_out_tx_size;

/*
 * usb_out_init
 * 
 */
void usb_out_init(void)
{
    usb_out_tx_size = 0;
}

/*
 * usb_out_transmitting
 * 
 */
void usb_out_transmitting(void)
{
    int usb_packet_size = USBD_GET_PAYLOAD_LEN(EP5);

    usb_out_tx_ptr += usb_packet_size;
    usb_out_tx_size -= usb_packet_size;

    if (usb_out_tx_size)
    {
        usb_packet_size = usb_out_tx_size > 64 ? 64 : usb_out_tx_size;

        USBD_MemCopy((uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP5)), (uint8_t *)usb_out_tx_ptr, (uint32_t)usb_packet_size);
        USBD_SET_PAYLOAD_LEN(EP5, usb_packet_size);
    }
    else if (usb_packet_size == 64)
    {
        /* Prepare a zero packet if previous packet size is 64 and 
               no more data to send at this moment to note Host the transfer has been done */
        USBD_SET_PAYLOAD_LEN(EP5, 0);
    }
}

/*
 * usb_out_send
 * 
 *    Send packet through custom usb endpoint
 * 
 */
int usb_out_send(uint8_t *ptr, uint32_t size)
{
    if (usb_out_tx_size)
        return -1;

    usb_out_tx_ptr = ptr;
    usb_out_tx_size = size;

    int usb_packet_size = usb_out_tx_size > 64 ? 64 : usb_out_tx_size;

    USBD_MemCopy((uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP5)), (uint8_t *)usb_out_tx_ptr, (uint32_t)usb_packet_size);
    USBD_SET_PAYLOAD_LEN(EP5, usb_packet_size);

    return 0;
}

/*
 * usb_out_ready
 * 
 *    Ready for next packet
 * 
 */
int usb_out_ready(void)
{
    return usb_out_tx_size == 0;
}
