import usb.core
import usb.util

def get_uint32(data, offset):
    return int.from_bytes(data[offset:offset+4], byteorder='little', signed=False)

def get_uint16(data, offset):
    return int.from_bytes(data[offset:offset+2], byteorder='little', signed=False)


def chunked(size, source):
    for i in range(0, len(source), size):
        yield source[i:i+size]


class Trace:
    def from_bytes(data):
        
        header = data[:36]
        payload = data[36:]

        t = Trace()

        t.ident = get_uint32(header, 0)
        t.level = get_uint32(header, 4)
        t.state = get_uint32(header, 8)
        t.count = get_uint32(header, 12)
        t.memory_adr = get_uint32(header, 16)
        t.max = get_uint32(header, 20)
        t.in_ = get_uint32(header, 24)
        t.out = get_uint32(header, 28)
        t.size = get_uint32(header, 32)

        t.data_raw = []
        for p in chunked(2, payload):
            t.data_raw.append(get_uint16(p, 0))

        t.reorder()

        return t

    def reorder(self):
        self.data = self.data_raw[self.out:] + self.data_raw[:self.out]

    def __str__(self):

        s = [
            f'ident = {self.ident}',
            f'level = {self.level}',
            f'state = {self.state}',
            f'count = {self.count}',
            f'memory_adr = 0x{self.memory_adr:x}',
            f'max = {self.max}',
            f'in = {self.in_}',
            f'out = {self.out}',
            f'size = {self.size}',
        ]

        return '\n'.join(s)
       
def find_trace_out_interface(cfg):
    for intf in cfg:
        if intf.bInterfaceClass == 0xFF and intf.bInterfaceSubClass == 0x0C:
            return intf
    return None


def usb_setup():
    print('Start')
    # find our device
    dev = usb.core.find(idVendor=0x0416, idProduct=0xb002)

    # was it found?
    if dev is None:
        raise ValueError('Device not found')

    # set the active configuration. With no arguments, the first
    # configuration will be the active one
    #dev.set_configuration()

    # get an endpoint instance
    cfg = dev.get_active_configuration()


    intf = find_trace_out_interface(cfg)
    assert intf is not None


    ep = usb.util.find_descriptor(
        intf,
        # match the first IN endpoint
        custom_match = \
        lambda e: \
            usb.util.endpoint_direction(e.bEndpointAddress) == \
            usb.util.ENDPOINT_IN)

    print('ep', ep)

    assert ep is not None


    return ep

def usb_read(ep):
    # read the data
    try:
        trace_data = ep.read(512, 200)
        #print(trace_data, len(trace_data))
        trace = Trace.from_bytes(trace_data)
        #print(trace)

    except usb.core.USBTimeoutError:
        return None

    return trace




if __name__ == '__main__':
    ep = usb_setup()
    while True:
        trace =  usb_read(ep)
        if trace is not None:
            print(trace)
            print(trace.data)

