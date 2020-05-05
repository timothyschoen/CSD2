## RUN WITH SUDO
## This program will block your mouse from being used by the OS
## Use a second mouse!

## DEPENDENCIES
## libusb or openUSB (brew install libusb or sudo apt-get install libusb)
## python-rtmidi (pip3 install python-rtmidi)

from pythonosc import udp_client
import array
import sys
import usb.core
import usb.util
import time
import rtmidi
from rtmidi.midiconstants import (CONTROL_CHANGE, MODULATION_WHEEL,
                                  NOTE_OFF, NOTE_ON, RESET_ALL_CONTROLLERS)
import serial
from serial.tools import list_ports



#portname = list(list_ports.grep("16C0:0483"))[0][0] # Find teensy by HWID
#potmeter = serial.Serial(portname, 19200, timeout=1)    # Make a serial port




# Open midi port
midiout = rtmidi.MidiOut()
midiout.open_virtual_port("My virtual output")


# ENTER YOUR OWN MOUSE SETTINGS!!
VID = 0x275d
PID = 0x0ba6
DATA_SIZE = 5



previousRead = 0

ringing = False
note = 7
major = [0, 2, 4, 5, 7, 9, 11]
scale = [major[i%7]+(int(i/7)*12) for i in range(24, 60)]
noteout = 60
previousNote = 60
olddir = 1
realdir = 1;

def processOutput(data):
    global previousRead, ringing, scale, note, noteout, previousNote, olddir, realdir
    #print(scale)
    dir = olddir
    if data != 0 and ((data[2] > 2 and data[3] > 127) or (255-data[2] > 2 and data[3]<=127)):
        ringing = False
        velocity = 0
        #print(data) # Raw mouse data, use to figure out your mouse array output
        if(data[3] > 127):
            velocity = (255-data[2]) / 4 + (255-data[3])*30
            dir = 1
        elif(data[3] <= 127):
            dir = 2
            velocity = data[2]/4+data[3]*30
        # Smoothing
        a = 0.1 # smoothing factor: lower=smoother
        EWMF = (1-a)*previousRead + a*velocity
        previousRead = EWMF
    elif previousRead < 40 and not ringing: #In case of a soft release
        a = 0.25 # smoothing factor: lower=smoother
        EWMF = (1-a)*previousRead + a*0
        previousRead = EWMF
    else: #In case of a fast release
        ringing = True
        a = 0.4 # smoothing factor: lower=smoother
        EWMF = (1-a)*previousRead + a*0
        previousRead = EWMF

    transpose = 0

    if  data != 0 and data[4] == 255: # Move pitch up when scrolling up
        note = note + 1
    elif data != 0 and data[4] == 1: # Move pitch down when scrolling down
        note = note - 1
        noteout = scale[note]
    elif  data != 0 and data[0] == 1: # Pitch with mouseclicks
        transpose += 1
    elif  data != 0 and data[0] == 2:
        transpose += 2
    elif  data != 0 and data[0] == 3:
        transpose += 3
    elif  data != 0 and data[0] == 4:
        transpose += 5

    if note <= 0:
        note = 0;
    elif note >= 36:
        note = 36

    noteout = scale[note]

    a = 0.2
    glide = (1-a)*previousNote + a*noteout
    previousNote = glide
    glide += transpose

    # Output our velocity
    #print(int(min(EWMF, 127))) # Smoothed velocity value, limited at 127
    midiout.send_message([CONTROL_CHANGE | 0, 2, int(min(EWMF, 127))]) # Output velocity on CC channel 2
    midiout.send_message([CONTROL_CHANGE | 0, 3, dir]) # Output direction on CC channel 3

    realdir = (1-0.1)*realdir + 0.1*dir
    if int(realdir) != olddir:
        EWMF = EWMF/2
        olddir = realdir



    if EWMF > 3:
        client.send_message("/n", float(min(EWMF, 127)/127.))
    elif EWMF < 3:
        client.send_message("/o", float(min(EWMF, 127)/127.))

    client.send_message("/f", glide)

# Try to find USB mouse
device = usb.core.find(idVendor = VID, idProduct = PID)
if device is None:
    sys.exit("Could not find USB mouse.")

# Make sure the hiddev kernel driver is not active
if device.is_kernel_driver_active(0):
    try:
        device.detach_kernel_driver(0)
    except usb.core.USBError as e:
        sys.exit("Could not detatch kernel driver: %s" % str(e))

# Set configuration
try:
    device.reset()
    device.set_configuration()
except usb.core.USBError as e:
    sys.exit("Could not set configuration: %s" % str(e))

endpoint = device[0][(0,0)][0]
#print(endpoint.wMaxPacketSize)


data = array.array('B',(0,)*4)

client = udp_client.SimpleUDPClient("127.0.0.1", 9002)

# Get mouse information!
while True:
    #potvalue = int(str(potmeter.readline()).split("'")[1].split('\\')[0])
    #print(potvalue)
    #midiout.send_message([NOTE_ON | 0, (potvalue/10)+60, 100])   # Note out
    #midiout.send_message([CONTROL_CHANGE | 0, 1, (potvalue%10)*1.2]) # Pitch bend

    try:
        data = device.read(endpoint.bEndpointAddress, endpoint.wMaxPacketSize, 30) # 30ms timeout: lower=more responsive but less smooth
        processOutput(data)
    except usb.core.USBError as e:
        processOutput(0)
        if e.args == ('Operation timed out',):
            print("timeout")
            continue
