#include "usb.h"

#include "NUC123.h"

int usb_attached(void)
{
    return USBD_IS_ATTACHED();
}