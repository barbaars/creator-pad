#ifndef _USB_OUT_H_
#define _USB_OUT_H_

#include <stdint.h>

void usb_out_init(void);
void usb_out_transmitting(void);
int usb_out_send(uint8_t *ptr, uint32_t size);
int usb_out_ready(void);

#endif
