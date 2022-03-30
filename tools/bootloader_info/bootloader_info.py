#!/usr/bin/env python3

import usb
import json

def main():

	dev = usb.core.find(idVendor=0x0416, idProduct=0xbdf0)

	info = {}
	info['vendor'] = usb.util.get_string(dev, 1)
	info['product'] = usb.util.get_string(dev, 2)
	info['version'] = usb.util.get_string(dev, 4)

	print(json.dumps(info))

if __name__ == "__main__":
	main()
