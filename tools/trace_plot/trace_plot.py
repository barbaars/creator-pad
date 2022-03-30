import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from random import randrange
from datetime import datetime


import threading, queue, time

from usb_read import usb_setup, usb_read

q = queue.Queue()

def worker(stop):
    print('Worker started')
    ep = usb_setup()
    while True:
        trace = usb_read(ep)
        if trace is not None:
            q.put(trace)

        if stop():
            break
    
    print('Worker stopped')


def main():
    stop_threads = False
    t1 = threading.Thread(target=worker,  args =(lambda : stop_threads, ))
    t1.start()
    
    try:
        fig, ax = plt.subplots()
        
        lines = []
        def update(frame):
            
            try:
                trace = q.get_nowait()
            except queue.Empty:
                return
            
            if len(lines) >= 6:
                lines.pop(0).remove()

            l, = ax.plot(trace.data, label=f'a{trace.ident:02} ({datetime.now():%H:%M:%S})')
            lines.append(l)
            ax.legend(loc='upper right')
            fig.gca().relim()
            fig.gca().autoscale_view()
            
        animation = FuncAnimation(fig, update, interval=500)
        
        plt.show()
    except KeyboardInterrupt:
        print('oi')
        pass
        
    stop_threads = True
    t1.join()
    print('Stopped')
    
    
if __name__ == '__main__':
    main()
