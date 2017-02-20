import tkinter as tk
import time
import socket

class Application(tk.Frame):
    sock = None
    def __init__(self, master=None):
        tk.Frame.__init__(self, master)

        self.sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(("192.168.4.1", 1233))
        self.sock.send( b'c')
        print( self.sock)

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
        lines = self.sock.recv( 1024).decode().splitlines()
        for v in lines:
          self.now.set( "{:10.1f}".format( float( v) * 5.1 / 167000))
        # schedule timer to call myself after 1 second
        self.after(100, self.onUpdate)

root = tk.Tk()
app = Application(master=root)
root.mainloop()