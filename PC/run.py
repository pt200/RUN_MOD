import tkinter as tk
import time
from socket import *
import os
import sqlite3

db_filename = 'run.db'


class Application(tk.Frame):
    sock = None
    def __init__(self, _db, master=None):
        tk.Frame.__init__(self, master)

        self.db = _db

        self.sdyn = socket(AF_INET, SOCK_DGRAM)
        self.sdyn.bind(('',49087))
        self.sdyn.settimeout(0.005)

        self.sdist = socket(AF_INET, SOCK_DGRAM)
        self.sdist.bind(('',49088))
        self.sdist.settimeout(0.005)
#        self.sock.send( b'c')
        self.pack()
        self.createWidgets()

    def createWidgets(self):
        self.vdyn = tk.StringVar()
        self.vdist = tk.StringVar()

        self.ldyn = tk.Label(self, font=('Helvetica', 196))
        self.ldist = tk.Label(self, font=('Helvetica', 196))

        self.ldyn.pack(side="top")
        self.ldist.pack(side="top")

        self.ldyn["textvariable"] = self.vdyn
        self.ldist["textvariable"] = self.vdist

        self.QUIT = tk.Button(self, text="QUIT", fg="red",
                                            command=root.destroy)
        self.QUIT.pack(side="bottom")

        # initial time display
        self.onUpdate()

    def onUpdate(self):
        # update displayed data
        while( 1):
          try:
            v = self.sdist.recv( 4096).decode()
            self.vdist.set( "{:10.1f}".format( int( v) / 1000))

#            self.db.execute( "insert into dist( cnt) values(" + v + ")")
          except Exception as e:
            print( e)
            break
        dyn = 0.0
        n = 0
        while( 1):
          try:
            lines = self.sdyn.recv( 4096).decode().splitlines()
            for v in lines:
              dyn += int( v)
              n += 1
          except Exception as e:
#            print( e)
            break
#          pass
        if( n > 0):
          self.vdyn.set( "{:10.1f}".format( float( ( dyn / n) + 163000) * 5.1 / 167000))

        self.after( 50, self.onUpdate)


db = sqlite3.connect( db_filename)
#db.execute("""
#  create table dist(
#    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
#    sqltime TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
#    cnt INTEGER NOT NULL)
#  """)
#db.execute("""
#  create table t(
#    bb INTEGER NOT NULL)
#  """)

root = tk.Tk()
app = Application( _db = db, master=root)
root.mainloop()
#print( "Close DB")
#for q in range( 10000):
#  db.execute( "insert into t values( " + str( q) + ")")
db.close()
