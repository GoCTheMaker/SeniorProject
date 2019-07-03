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
        ttk.Label(self, text="Dongle SN", font=LARGE_FONT).grid(row=5, column=0)
        ttk.Label(self, text="Messages", font=LARGE_FONT).grid(row=7, column=0)

        ttk.Label(self, text="Fixes Per Day", font=LARGE_FONT).grid(row=0, column=3)
        ttk.Label(self, text="Hours on (XX-XX, 24Hr)", font=LARGE_FONT).grid(row=2, column=3)
        ttk.Label(self, text="Days on (M,W,R...)", font=LARGE_FONT).grid(row=4, column=3)


        #Buttons
        button1 = ttk.Button(self, text="Connect",
                             command=lambda: self.NewComConnection())
        button1.grid(row=2, column=1)
        button2 = ttk.Button(self, text="Get SN",
                             command=self.GetSN)
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
        self.rxEntry = tk.Entry(self)

        self.comPortEntry.grid(row=2, column=0)
        self.snEntry.grid(row=4, column=0)
        self.rxEntry.grid(row=6, column=0)


        self.snEntry.insert(0, '')
        self.snEntry.configure(state='readonly')

        self.fixE = tk.Entry(self)
        self.fixE.grid(row=1, column=3)

        self.hourE = tk.Entry(self)
        self.hourE.grid(row=3, column=3)

        self.dayE = tk.Entry(self)
        self.dayE.grid(row=5, column=3)

        #Text Box
        self.msgBox=scrolledtext.ScrolledText(self, height=10, width=40)
        self.msgBox.grid(row=8, column=0)


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

        # Set up the XBee device
        # must be done first due to timeout constraints
        serialData = "CFG?" + self.rxEntry.get() + "?"
        if self.ser.isOpen():
            self.ser.write(str.encode(serialData))
            self.msgBox.insert(tk.END, "Configuring XBee\n")
        else:
            self.msgBox.insert(tk.END, "Please connect to COMPORT first\n")
            return

        self.ser.timeout = 3;
        tmpStr = self.ser.read(14)
        tmpStr = self.ser.read(50)  #Should timeout
        tmpStr = tmpStr.decode()
        if tmpStr != self.rxEntry.get():
            self.msgBox.insert(tk.END, "XBee configuration ERROR, set to " + tmpStr + "\n")
            return
        else:
            self.msgBox.insert(tk.END, "XBee set to " + tmpStr + "\n")

        time.sleep(1) #Ensure xbee timeout

        #Get Device SN
        self.GetSN()

        # Set time on RTC first
        now = datetime.datetime.now()
        print(now)

        #Set up filepointer
        f = open(self.snEntry.get() + ".txt", "w+")

        #File header
        f.write("Collar configuration file for SN: " + self.snEntry.get() + "\n\n")
        f.write("Collar configured at " + str(now) + "\n")

        f.write("Dongle Target SN: " + tmpStr + "\n")

        #String takes form of:
        #second minute hour day month year weekday
        serialData = "TST?" + now.strftime('%S%M%H%d%m%y%w') + "?"

        if self.ser.isOpen():
            self.ser.write(str.encode(serialData))
        else:
            self.msgBox.insert(tk.END, "Please connect to COMPORT first\n")
            return

        self.ser.timeout = 1;

        print(self.ser.read(100))


        self.ser.timeout = 1;

        serialData = "PGM?" + self.fixE.get() + '-' + self.hourE.get() + '-' + self.dayE.get() + ".?"

        self.ser.write(str.encode(serialData))

        time.sleep(1)

        f.write("Fixes per day: " + self.fixE.get() + "\n")
        f.write("VHF Hours: " + self.hourE.get() + "\n")
        f.write("VHF Days: " + self.dayE.get() + "\n")
        f.close()

        serialData = "GPG?"
        self.ser.write(str.encode(serialData))

        print(self.ser.read(50))

        self.ser.close()


    def GetSN(self):
        tmpStr = ""
        if self.ser.isOpen():
            self.ser.write(str.encode("GSN?"))
        else:
            self.msgBox.insert(tk.END, "Please connect to COMPORT first\n")
            return

        self.ser.timeout = 2;

        tmpStr = self.ser.read(2)
        tmpStr = self.ser.read(14)

        self.snEntry.configure(state="normal")
        self.snEntry.delete(0, 'end')
        tmpStr = tmpStr.decode()
        self.snEntry.insert(0,tmpStr)
        self.snEntry.configure(state='readonly')


#=======================================================================================================================
class PageDataRx(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        tk.Frame.__init__(self, parent)
        #label = tk.Label(self, text="Receive Data", font=LARGE_FONT)
        #label.pack(pady=10, padx=10)

        #Buttons
        button1 = ttk.Button(self, text="Home",
                            command=lambda: controller.show_frame(StartPage))
        button1.grid(row=1, column=1)
        button2 = ttk.Button(self, text="Get Dongle SN",
                             command=self.GetDongleSN)
        button2.grid(row=2, column=1)

        button3 = ttk.Button(self, text="Set Target",
                             command=self.SetDongleTx)
        button3.grid(row=3, column=1)

        #Entries
        self.comPortEntry = tk.Entry(self)
        self.snEntry = tk.Entry(self)

        self.comPortEntry.grid(row=2, column=0)
        self.snEntry.grid(row=4, column=0)

        self.snEntry.insert(0, '')
        self.snEntry.configure(state='readonly')

        self.txEntry = tk.Entry(self)
        self.txEntry.grid(row=5, column=0)

        # Text Box
        self.msgBox = scrolledtext.ScrolledText(self, height=10, width=40)
        self.msgBox.grid(row=7, column=0)
        self.msgBox.insert(tk.END, "Testing\n")

        # Serial Port
        self.ser = serial.Serial(
            port=None,
            # port='\\\\.\\COM3',
            baudrate=9600,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS
        )

    def GetDongleSN(self):

        self.ser.port = '\\\\.\\COM' + self.comPortEntry.get()

        # global serial_object
        # serial_object = self.ser.port
        # Connect()

        tmpString = ''
        # Check to see if port is open or if port exists
        if self.ser.isOpen():
            try:
                self.ser.close()
            except IOError:
                self.msgBox.insert(tk.END, "Could not close port: " + self.comPortEntry.get() + '\n')
                return
            self.msgBox.insert(tk.END, "Comport was open, closing now\n")

        # Attempt to open port with current params
        try:
            self.ser.open()
        except IOError:
            self.msgBox.insert(tk.END, "Could not open port: " + self.comPortEntry.get() + '\n')
            return
        if self.ser.isOpen():
            self.msgBox.insert(tk.END, "Comport " + self.comPortEntry.get() + " opened succesfully.\n")
            self.msgBox.insert(tk.END, "Attempting device handshake\n")

        # Flush port before use
        self.ser.flushInput()
        self.ser.flushOutput()
        self.update()

        # Send +++ to device
        time.sleep(1)
        self.ser.write(str.encode("+++"))
        self.msgBox.insert(tk.END, "Sent\n")
        self.update()

        #Wait for rx
        self.ser.timeout = 2
        tmpString = self.ser.read(2)
        print(tmpString)
        if tmpString == b'OK':
            self.msgBox.insert(tk.END, "Programming mode entered\n")
        else:
            self.msgBox.insert(tk.END, "Could not connect to device\n")
            return

        #Get SN Upper
        self.msgBox.insert(tk.END, "Getting the address from dongle\n")
        self.update()
        self.ser.write(b'ATSH\r')
        tmpa = self.ser.read(8)
        self.ser.write(b'ATSL\r')
        tmpb = self.ser.read(8)
        tmpString = tmpa[1:7] + tmpb[0:8]

        self.snEntry.configure(state='normal')
        self.snEntry.insert(0, tmpString.decode())
        self.snEntry.configure(state='readonly')
        print(tmpString.decode())

    def SetDongleTx(self):
        self.ser.port = '\\\\.\\COM' + self.comPortEntry.get()

        # global serial_object
        # serial_object = self.ser.port
        # Connect()

        tmpString = ''
        # Check to see if port is open or if port exists
        if self.ser.isOpen():
            try:
                self.ser.close()
            except IOError:
                self.msgBox.insert(tk.END, "Could not close port: " + self.comPortEntry.get() + '\n')
                return
            self.msgBox.insert(tk.END, "Comport was open, closing now\n")

        # Attempt to open port with current params
        try:
            self.ser.open()
        except IOError:
            self.msgBox.insert(tk.END, "Could not open port: " + self.comPortEntry.get() + '\n')
            return
        if self.ser.isOpen():
            self.msgBox.insert(tk.END, "Comport " + self.comPortEntry.get() + " opened succesfully.\n")
            self.msgBox.insert(tk.END, "Attempting device handshake\n")

        # Flush port before use
        self.ser.flushInput()
        self.ser.flushOutput()
        self.update()

        # Send +++ to device
        time.sleep(1)
        self.ser.write(str.encode("+++"))
        self.msgBox.insert(tk.END, "Sent\n")
        self.update()

        # Wait for rx
        self.ser.timeout = 2
        tmpString = self.ser.read(2)
        print(tmpString)
        if tmpString == b'OK':
            self.msgBox.insert(tk.END, "Programming mode entered\n")
        else:
            self.msgBox.insert(tk.END, "Could not connect to device\n")
            return
        tmpString = self.txEntry.get()
        dh = tmpString[0:6]
        dl = tmpString[6:]

        self.ser.timeout=2

        self.msgBox.insert(tk.END, "Setting DH to " + dh + "\n")
        self.update()

        #setting DH
        self.ser.write(str.encode("ATDH" + dh + '\r'))
        self.ser.read(3)
        self.ser.write(b'ATAC\r')
        self.ser.read(3)
        self.ser.write(b'ATDH\r')
        tmp = self.ser.read(8)
        tmp = tmp.decode()
        self.msgBox.insert(tk.END, "DH set to " + tmp + "\n")
        self.update()

        #Setting DL
        self.msgBox.insert(tk.END, "Setting DL to " + dl + "\n")
        self.update()

        self.ser.write(str.encode("ATDL" + dl + '\r'))
        self.ser.read(3)
        self.ser.write(b'ATAC\r')
        self.ser.read(3)
        self.ser.write(b'ATDL\r')
        tmp = self.ser.read(8)
        tmp = tmp.decode()
        self.msgBox.insert(tk.END, "DL set to " + tmp + "\n")
        self.update()

        print(dh)
        print(dl)
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

