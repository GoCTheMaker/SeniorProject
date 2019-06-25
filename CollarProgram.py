import tkinter as tk
from tkinter import ttk
from tkinter import scrolledtext
import datetime

import threading
import time
import os
import serial


import urllib
import json

import pandas as pd
import numpy as np


LARGE_FONT=("Verdana", 12)


#App container
class CollarProgram (tk.Tk):
    def __init__(self, *args, **kwargs):

        tk.Tk.__init__(self, *args, **kwargs)

        tk.Tk.iconbitmap(self, default="GV_icon.ico")
        tk.Tk.wm_title(self, "2019 Wildlife Tracking Collar Interface")

        container = tk.Frame(self)
        container.pack(side='top', fill='both', expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}

        for F in (StartPage, PageProgram, PageDataRx, PagePowerCalc):
            frame = F(container, self)
            self.frames[F] = frame
            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(StartPage)

    def show_frame(self, cont):

        frame = self.frames[cont]
        frame.tkraise()


def qf(stringtoprint):
    print(stringtoprint)


class StartPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        label = ttk.Label(self, text="Main Menu", font=LARGE_FONT)
        label.grid(row=0, column=2)

        button1=ttk.Button(self, text="Program Collar",
                          command=lambda: controller.show_frame(PageProgram))
        button1.grid(row=1, column=0)
        button2 = ttk.Button(self, text="Download Data",
                             command=lambda: controller.show_frame(PageDataRx))
        button2.grid(row=1, column=2)
        button3 = ttk.Button(self, text="Power Calculator",
                             command=lambda: controller.show_frame(PagePowerCalc))
        button3.grid(row=1, column=4)


class PageProgram(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        tk.Frame.__init__(self, parent)

        #Labels
        ttk.Label(self, text="Program Device", font=LARGE_FONT).grid(row=0, column=0)
        ttk.Label(self, text="COM Port", font=LARGE_FONT).grid(row=1, column=0)
        ttk.Label(self, text="Serial Num", font=LARGE_FONT).grid(row=3, column=0)
        ttk.Label(self, text="Messages", font=LARGE_FONT).grid(row=5, column=0)

        ttk.Label(self, text="Fixes Per Day", font=LARGE_FONT).grid(row=0, column=3)
        ttk.Label(self, text="Hours on (XX-XX, 24Hr)", font=LARGE_FONT).grid(row=2, column=3)
        ttk.Label(self, text="Days on (M,W,R...)", font=LARGE_FONT).grid(row=4, column=3)


        #Buttons
        button1 = ttk.Button(self, text="Connect",
                             command=lambda: self.NewComConnection())
        button1.grid(row=2, column=1)
        button2 = ttk.Button(self, text="Get SN",
                             command=self.TimeStamp)
        button2.grid(row=4, column=1)
        button3 = ttk.Button(self, text="Home",
                             command=lambda: controller.show_frame(StartPage))
        button3.grid(row=1, column=4)
        button4 = ttk.Button(self, text="?",
                             command=lambda: controller.show_frame(StartPage))
        button4.grid(row=0, column=4)
        button5 = ttk.Button(self, text="Program",
                             command=self.ProgramDevice)
        button5.grid(row=6, column=3)
        button6 = ttk.Button(self, text="Get Config",
                             command=lambda: self.GetConfig())
        button6.grid(row=7, column=3)
        #Entries
        self.comPortEntry = tk.Entry(self)
        self.snEntry = tk.Entry(self)

        self.comPortEntry.grid(row=2, column=0)
        self.snEntry.grid(row=4, column=0)

        self.snEntry.insert(0, '000')
        self.snEntry.configure(state='readonly')

        self.fixE = tk.Entry(self)
        self.fixE.grid(row=1, column=3)

        self.hourE = tk.Entry(self)
        self.hourE.grid(row=3, column=3)

        self.dayE = tk.Entry(self)
        self.dayE.grid(row=5, column=3)

        #Text Box
        self.msgBox=scrolledtext.ScrolledText(self, height=10, width=40)
        self.msgBox.grid(row=7, column=0)
        self.msgBox.insert(tk.END, "Testing\n")

        #Serial Port
        self.ser = serial.Serial(
            port=None,
            #port='\\\\.\\COM3',
            baudrate=9600,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS
        )

    def NewSerialNum(self):
        self.msgBox.insert(tk.END, "poo[\n")

    #Function to connect program to collar  comport
    #Needs to read from com port text box in future
    def NewComConnection(self):
        # ser = serial.Serial(
        #
        #     port = '\\\\.\\COM' + self.comPortEntry.get(),
        #     #port='\\\\.\\COM3',
        #     baudrate=9600,
        #     parity=serial.PARITY_NONE,
        #     stopbits=serial.STOPBITS_ONE,
        #     bytesize=serial.EIGHTBITS
        # )
        self.ser.port = '\\\\.\\COM' + self.comPortEntry.get()

        # global serial_object
        # serial_object = self.ser.port
        # Connect()

        tmpString = ''
        #Check to see if port is open or if port exists
        if self.ser.isOpen():
            try:
                self.ser.close()
            except IOError:
                self.msgBox.insert(tk.END, "Could not close port: " + self.comPortEntry.get() + '\n')
                return
            self.msgBox.insert(tk.END, "Comport was open, closing now\n")

        #Attempt to open port with current params
        try:
            self.ser.open()
        except IOError:
            self.msgBox.insert(tk.END, "Could not open port: " + self.comPortEntry.get() + '\n')
            return
        if self.ser.isOpen():
            self.msgBox.insert(tk.END, "Comport " + self.comPortEntry.get() + " opened succesfully.\n")
            self.msgBox.insert(tk.END, "Attempting device handshake\n")

        #Flush port before use
        self.ser.flushInput()
        self.ser.flushOutput()
        self.update()

        #Send ACK to device
        self.ser.write(str.encode("ACK?"))
        self.msgBox.insert(tk.END, "Sent\n")
        self.update()

        #Read for response from device with timeout
        self.ser.timeout = 1
        tmpString = self.ser.read(4)

        if not tmpString: #No response
            self.msgBox.insert(tk.END, "No response from device\n")
            return

        elif tmpString != b'ACK?':
            self.msgBox.insert(tk.END, "Incorrect response from device\n")
            return

        else:
            self.msgBox.insert(tk.END, "Handshake completed successfully\n")

        now = datetime.datetime.now()

    def ProgramDevice(self):
        #Set time on RTC first
        now = datetime.datetime.now()
        print(now)

        #String takes form of:
        #second minute hour day month year weekday
        serialData = "TST?" + now.strftime('%S%M%H%d%m%y%w') + "?"

        if self.ser.isOpen():
            self.ser.write(str.encode(serialData))
        else:
            self.msgBox.insert(tk.END, "Please connect to COMPORT first\n")
            return

        self.ser.timeout = 3;

        print(self.ser.read(100))


        self.ser.timeout = 1;

        serialData = "PGM?" + self.fixE.get() + '-' + self.hourE.get() + '-' + self.dayE.get() + ".?"

        self.ser.write(str.encode(serialData))

        time.sleep(1)



        f = open(self.snEntry.get() + ".txt", "w+")
        f.write(serialData)
        f.close()

        serialData = "GPG?"
        self.ser.write(str.encode(serialData))

        print(self.ser.read(50))


    def TimeStamp(self):
        if self.ser.isOpen():
            self.ser.write(str.encode("TMP?"))
        else:
            self.msgBox.insert(tk.END, "Please connect to COMPORT first\n")
            return

        self.ser.timeout = 6;

        print(self.ser.read(100))


#=======================================================================================================================
class PageDataRx(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Receive Data", font=LARGE_FONT)
        label.pack(pady=10, padx=10)

        button1 = ttk.Button(self, text="Home",
                            command=lambda: controller.show_frame(StartPage))
        button1.pack()
#=======================================================================================================================
class PagePowerCalc(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Power Calculations", font=LARGE_FONT)
        label.pack(pady=10, padx=10)

        button1 = ttk.Button(self, text="Home",
                            command=lambda: controller.show_frame(StartPage))
        button1.pack()
#=======================================================================================================================

app = CollarProgram()
#app.update()
#after(500, app.update())
app.mainloop()

