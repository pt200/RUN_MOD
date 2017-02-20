import tkinter as tk
import time
from socket import *


class Application(tk.Frame):
    sock = None
    def __init__(self, master=None):
        tk.Frame.__init__(self, master)

        self.sock = socket(AF_INET, SOCK_DGRAM)
        self.sock.bind(('',49087))
        self.sock.settimeout(0.005)
#        self.sock.send( b'c')
        self.pack()
        self.createWidgets()

    def createWidgets(self):
        self.now = tk.StringVar()
        self.time = tk.Label(self, font=('Helvetica', 196))
        self.time.pack(side="top")
        self.time["textvariable"] = self.now

        self.QUIT = tk.Button(self, text="QUIT", fg="red",
                                            command=root.destroy)
        self.QUIT.pack(side="bottom")

        # initial time display
        self.onUpdate()

    def onUpdate(self):
        # update displayed time
        s = 0.0;
        n = 0
        while( 1):
          try:
            lines = self.sock.recv( 4096).decode().split()
#            print( lines)
          except:
            break
          for v in lines:
            s += ( ( float( v) + 169000) * 5.1) / 167000
            n += 1
        if( n > 0):
          self.now.set( "   {:.1f}   ".format( s / n))
        # schedule timer to call myself after 1 second
        self.after(250, self.onUpdate)

root = tk.Tk()
app = Application(master=root)
root.mainloop()