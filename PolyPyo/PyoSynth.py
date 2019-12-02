from pyo import *
import time
import rtmidi
import threading
from tkinter import *
from tkinter import ttk
import atexit
from os import walk
from os.path import isfile, join
import sys
import faulthandler
from tsMenu import tsMenu
from tsMenu import tsSliderMenu

s = Server(nchnls=2, duplex=0, audio='portaudio', sr=44100, buffersize=512).boot()
s.start()


# Eerste nieuwe features:

# 1. FxMenu en ModList fixen
# 2. Hardware bouwen
# 3. Teensy Code uitwerken

# Daarna:
# 1. Presets!!
# 2. bugtesten
# 3. Commenten
# 4. Extra filters en effecten toevoegen, mss meer oscillator sources, meer modulation sources (step sequencer?)
# 5. BPM Sync
# 6. Efficient maken, limieten zetten


voices = 4

lfos = {}
envelopes = {}
modDestList = {}

sliderframe = 0

filterParams = [SigTo(value=100), Sig(value=1)]
midiInput = rtmidi.MidiIn()
ctrlInput = rtmidi.MidiIn()
ctrlInput.open_port(1)
waves = [SndTable('SAW.wav') for i in range(2)]
modDestList['Oscillator Pitch'] = [[Sig(value=0) for i in range(voices)] for x in range(8)]
modDestList['Filter Cutoff'] = [[Sig(value=0) for i in range(voices)] for x in range(8)]
modDestList['Filter Resonance'] = [[Sig(value=0) for i in range(voices)] for x in range(8)]

controllers = [Sig(value=0) for i in range(16)]
ctrlupdates = [None for i in range(16)]

class Oscillator(PyoObject):
    global lfos, envelopes, modDestList, filterParams, pitch, waves, voices, mm
    def _init_(self):
        PyoObject.__init__(self)
        volumeADSR = [0.1, 0.5, 0.3, 0.02, 0.3]
        self.pitch = [Sig(value=0) for i in range(voices)]
        self.envelope = [Adsr(attack=volumeADSR[0], decay=volumeADSR[1], sustain=volumeADSR[2], release=volumeADSR[3], dur=0, mul=volumeADSR[4]) for i in range(voices)]
        self.oscParams = [Sig(value=0), Sig(value=0.3)]
        self.pitchVariation = Randi(min=-0.0001, max=0.0001, freq=4)

        self.oscil = [Osc(table=waves[0], freq=MToF(self.pitchVariation+self.pitch[i]+self.oscParams[0]+sum([modDestList['Oscillator Pitch'][z][i] for z in range(8)])), mul=self.envelope[i]*self.oscParams[1], interp=4) for i in range(voices)]
        self.filter1 = [MoogLP(self.oscil[i], freq=MToF(filterParams[0]+sum([modDestList['Filter Cutoff'][z][i] for z in range(8)])), res=filterParams[1]) for i in range(voices)]
        self.filter2 = [SVF(self.oscil[i], freq=MToF(filterParams[0]+sum([modDestList['Filter Cutoff'][z][i] for z in range(8)])), type=filterParams[1]) for i in range(voices)]
        for i in range(voices):
            self.filter2[i].stop()

        self._base_objs = sum(self.filter1)+sum(self.filter2).getBaseObjects()
    def out(self):
        return PyoObject.out(self)

    def setShape(self, path):
         [i.setTable(path) for i in self.oscil]

    def changeFilter(self, type, foo):
        filters = [self.filter1, self.filter2]
        [[filters[i][x].play() if i == type-1 else filters[i][x].stop() for i in range(len(filters))] for x in range(voices)]
        mainmenu.loadMenu('mainMenu')

    def playnote(self, note, voice):
        global pitch, envelope
        self.pitch[voice].value = note
        self.envelope[voice].play()

    def stopnote(self, voice):
        global pitch, envelope
        self.envelope[voice].stop()


oscils = [Oscillator(), Oscillator()]
[osc._init_() for osc in oscils]

def changeVoices(num):
    global voices, busyList, timeList, noteList, envelopes, oscils
    oldvoices = voices
    voices = num
    for i in modDestList.keys():
        if isinstance(modDestList[i][0], list):
            while len(modDestList[i][-1]) != voices:
                if len(modDestList[i][0]) < voices:
                    for x in range(8):
                        modDestList[i][x].append(modDestList[i][x][0])
                else:
                    for x in range(8):
                        modDestList[i][x].pop()
    busyList = [False for i in range(voices)]
    timeList = [0 for i in range(voices)]
    noteList = [0 for i in range(voices)]
    for i in envelopes.keys():
        while len(envelopes[i][0]) != voices:
            if len(envelopes[i][0]) < voices:
                envelopes[i][0].append(envelopes[i][0][0])
            else:
                envelopes[i][0].pop()
    try:
        [osc._init_() for osc in oscils]
    except:
        print('couldnt change voices')

selection = 0
root = Tk()
frame = Frame(root, width=360, height=180,borderwidth=0, relief=RAISED)
frame.configure(background='#464646')
frame.pack_propagate(False)
frame.grid(padx=0, pady=0)
frame.pack(padx=0, pady=0)
frame.active = True

def mtof(note):
    a = 440 #frequency of A (common value is 440Hz)
    return (a / 32) * (2.001 ** ((note - 9) / 12))

def setMidiInput(val, foo):
    if midiInput.is_port_open():
        midiInput.close_port()
    midiInput.open_port(val-1)
    mainmenu.loadMenu('mainMenu')

def oscSettingsFunc(osc, foo):
    global oscSettings, currentmenu, oscSliders
    osc = osc-1
    oscSettings = []
    [oscSettings.append(i) for i in [['..', 'oscMenu'], ['Shape', changeFolder, "./", osc], ['Parameters', slidermenu, oscSliders[osc], [["Done", lambda: mainmenu.loadMenu('mainMenu')]]]]]
    currentmenu = [['oscSettings', oscSettings]]
    changeMenu(1, (0,))

def destinationMenu(source, name):
    global currentmenu, menubox, modDestList
    modDestMenu = [[i, lambda a,b: applyMod(source, list(modDestList.keys())[a], name)] for i in modDestList.keys()]
    mainmenu.addItems('modDestMenu', modDestMenu)
    mainmenu.loadMenu('modDestMenu')


#implementeren???
modPositions = [False, False, False, False, False, False, False, False]

def applyMod(source, destinations, name):
    global modPositions
    if name in modPositions:
        freespace = modPositions.index(name)
    else:
        freespace = modPositions.index(False)
        modPositions[freespace] = name
    if source[2] == 'ADSR':
        if isinstance(modDestList[destinations][freespace], list):
            for i in range(voices):
                modDestList[destinations][freespace][i].value = source[0][i]
        else:
                modDestList[destinations][freespace].value = sum(source[0])
    else:
        for d in destinations:
            if isinstance(modDestList[destinations][freespace], list):
                for i in range(voices):
                    modDestList[destinations][freespace][i].value = source[0]
            else:
                modDestList[destinations][freespace].value = source[0]
    mainmenu.loadMenu('mainMenu')

def makeADSR(idx, params):
    global currentmenu, modDestList
    params = params[0]
    n = len(envelopes)+1
    name = 'ADSR' + str(n)
    while name in envelopes.keys():
        n  = n+1
        name = 'ADSR' + str(n)
    modDestList[name + ' Depth'] = [Sig(value=0) for x in range(8)]
    envelopes[name] = [[Adsr(attack=params[0], decay=params[1], sustain=params[2], release=params[3], dur=0, mul=params[4]+sum(modDestList[name + ' Depth'])) for i in range(voices)], [], 'ADSR']
    ADSRslides = [['Attack', lambda y: [envelopes[name][0][x].setAttack(y) for x in range(len(envelopes[name][0]))], 0, 2, 'slider', params[0], None], ['Decay', lambda y: [envelopes[name][0][x].setDecay(y) for x in range(len(envelopes[name][0]))], 0, 2, 'slider', params[1], None], ['Sustain', lambda y: [envelopes[name][0][x].setSustain(y) for x in range(len(envelopes[name][0]))], 0, 1, 'slider', params[2], None], ['Release', lambda y: [envelopes[name][0][x].setRelease(y) for x in range(len(envelopes[name][0]))], 0, 4, 'slider', params[3], None], ['Amp', lambda y: [setattr(envelopes[name][0][x], 'mul', y+sum(modDestList[name + ' Depth'])) for x in range(len(envelopes[name][0]))], 0, 130, 'slider', params[4], None]]
    modItem = [name, slidermenu, ADSRslides, [["Next", lambda: destinationMenu(envelopes[name], name)], ['Delete', lambda: delete()]]]
    ##modList.insert(len(modList)-1, modItem)
    mainmenu.addItems('modList', modItem)
    def delete():
        modList.pop(modList.index(modItem))
        modDestList.pop(name + ' Depth')
        envelopes.pop(name)
        if name in modPositions:
            idx = modPositions.index(name)
            for i in modDestList.keys():
                if isinstance(modDestList[i][idx], list):
                    for x in range(voices):
                        modDestList[i][idx][x].value = 0
                else:
                        modDestList[i][idx].value = 0
        mainmenu.loadMenu('mainMenu')
    slidermenu(0, [ADSRslides, [["Next", lambda: destinationMenu(envelopes[name], name)], ['Delete', lambda: delete()]]])

def makeLFO(idx, params):
    global currentmenu, modDestList, modPositions
    n = len(lfos)+1
    name = 'LFO' + str(n)
    while name in lfos.keys():
        n  = n+1
        name = 'LFO' + str(n)
    modDestList[name + ' Depth'] = [Sig(value=0) for i in range(8)]
    modDestList[name + ' Rate'] = [Sig(value=0) for i in range(8)]
    params = params[0]
    lfos[name] = [LFO(freq=params[0]+sum(modDestList[name + ' Rate']), mul=params[1]+sum(modDestList[name + ' Depth']), type=params[2]), [], 'LFO']
    LFOslides = [['Rate', lambda y: [lfos[name][0].setFreq(y+sum(modDestList[name + ' Rate']))], 0, 20, 'slider', params[0], None], ['Amp', lambda y: [setattr(lfos[name][0], 'mul', y+sum(modDestList[name + ' Depth']))], 0, 130, 'slider', params[1], None], ['Shape', lambda y: [lfos[name][0].setType(y)], 'Saw Up', ['Saw Up', 'Saw Down', 'Square', 'Triangle', 'Pulse'], 'menubox', params[2], None]]
    modItem = [name, slidermenu, LFOslides, [["Next", lambda: destinationMenu(lfos[name], name)], ['Delete', lambda: delete()]]]
    #modList.insert(len(modList)-1, modItem)
    mainmenu.addItems('modList', modItem)
    def delete():
        modList.pop(modList.index(modItem))
        modDestList.pop(name + ' Depth')
        modDestList.pop(name + ' Rate')
        lfos.pop(name)
        if name in modPositions:
            idx = modPositions.index(name)
            for i in modDestList.keys():
                if isinstance(modDestList[i][idx], list):
                    for x in range(voices):
                        modDestList[i][idx][x].value = 0
                else:
                    modDestList[i][idx].value = 0
        mainmenu.loadMenu('mainMenu')
    lfos[name][0].play()
    slidermenu(0, [LFOslides, [["Next", lambda: destinationMenu(lfos[name], name)], ['Delete', lambda: delete()]]])


def shutDown(val, foo):
    global frame, root, shutdown, s, midiInput
    shutdown = True
    s.stop()
    frame.destroy()
    root.destroy()
    sys.exit()

def changeFolder(evt, other):
    global curdir, filelist, dirlist, fullbrowser, menubox, currentmenu
    folder, osc = other
    dir = os.path.abspath(folder)
    filelist = [filenames for (dirpath, dirnames, filenames) in walk(dir) if dirpath == dir][0]
    dirlist = [dirnames for (dirpath, dirnames, filenames) in walk(dir) if dirpath == dir][0]
    fullbrowser = [["..", changeFolder, dir+"/..", osc]] +[[x, handleFile, dir+'/'+x, osc] for x in filelist]+[[x, changeFolder, dir+'/'+x, osc] for x in dirlist]
    currentmenu = [['fake menu', fullbrowser]]
    changeMenu(1, (0,))

def handleFile(evt, other):
    filepath, osc = other
    try:
        table = SndTable(filepath)
        oscils[osc].setShape(table)
        mainmenu.loadMenu('mainMenu')
    except:
        print("not a wavetable")
        head, tail = os.path.split(filepath)
        changeFolder(0, [head, osc])

effectrack = []
def applyEffect(sel, arr):
    global output, fxMenu
    effect, arguments, oldname = arr
    n = 1
    name = oldname
    while name in [x[0] for x in fxMenu]:
        n  = n+1
        name = oldname + ' ' + str(n)
    if len(effectrack) == 0:
        input = sum([osc.mix() for osc in oscils])
    else:
        input = effectrack[-1]
    fx = effect(input)
    effectrack.append(fx)
    indx = len(effectrack)-1
    for i in range(len(arguments)):
        modDestList[name + ' ' + arguments[i][0]] = [Sig(value=0) for x in range(8)]
    effectslides = [[arguments[i][0], lambda y,i=i: setattr(effectrack[-1], arguments[i][1], y+sum(modDestList[name + arguments[i][0]])), arguments[i][2], arguments[i][3], 'slider', arguments[i][4], None] for i in range(len(arguments))]
    [effectslides[i].append(arguments[i][5]) for i in range(len(effectslides)) if len(arguments[i]) > 5]
    def delete(idx):
        global fxMenu
        for i in range(len(arguments)):
            modDestList.pop(name + ' ' + arguments[i][0])
        effectrack.pop(idx)
        fxMenu.pop(idx+1)
        for i in range(len(effectrack)):
            fxMenu[i+1][3] = [['Delete', lambda i=i: delete(i)], ['Done', lambda: mainmenu.loadMenu('mainMenu')]]
            if i == 0:
                setattr(effectrack[i], 'input', sum([osc.mix() for osc in oscils]))
            else:
                setattr(effectrack[i], 'input', effectrack[i-1])
        if len(effectrack) == 0:
            output.value = sum([osc.mix() for osc in oscils])
        else:
            output.value = effectrack[-1]
        mainmenu.loadMenu('mainMenu')
    slidermenu(0, [effectslides, [['Done', lambda: mainmenu.loadMenu('mainMenu')], ['Delete', lambda indx=indx: delete(indx)]]])
    output.value = effectrack[-1]
    fxItem = [name, slidermenu, effectslides, [['Delete', lambda indx=indx: delete(indx)], ['Done', lambda: mainmenu.loadMenu('mainMenu')]]]
    fxMenu.insert(len(fxMenu)-1, fxItem)
    mainmenu.loadMenu('mainMenu')

output = Sig(value=sum([osc.mix() for osc in oscils]))
mm = Mixer(outs=2, chnls=2, time=.025)
mm.addInput(0, output)
mm.setAmp(0, 0,5)
mm.out()

voiceSlider = [['Voices', lambda x: changeVoices(int(x)), 0, 16, 'slider', 4, None, lambda y: int(y)]]

volADSRslides = [['Attack', lambda y: [[osc.envelope[x].setAttack(y) for osc in oscils] for x in range(len(oscils[0].envelope))], 0, 2, 'slider', 0.2, None], ['Decay', lambda y: [[osc.envelope[x].setDecay(y) for osc in oscils] for x in range(len(oscils[0].envelope))], 0, 2, 'slider', 1, None], ['Sustain', lambda y: [[osc.envelope[x].setSustain(y) for osc in oscils] for x in range(len(oscils[0].envelope))], 0, 1, 'slider', 0.3, None], ['Release', lambda y: [[osc.envelope[x].setRelease(y) for osc in oscils] for x in range(len(oscils[0].envelope))], 0, 4, 'slider', 0.2, None], ['Amp', lambda y: [[setattr(osc, "mul", y) for osc in oscils] for x in range(len(oscils[0].envelope))], 0, 1, 'slider', 0.5, None]]

filterslides = [['Cutoff', lambda y : [filterParams[0].setValue(y)], 15, 134.9851, 'slider', 100, None, mtof], ['Res.', lambda y : [filterParams[1].setValue(y)], 0, 1.5, 'slider', 0.6, None]]
oscSliders = [[['Pitch', lambda x, i=i: setattr(oscils[i].oscParams[0], 'value', x), -30, 30, 'slider', 0, None], ['Volume', lambda x, i=i: setattr(oscils[i].oscParams[1], 'value', x), 0, 1, 'slider', 0.3, None]] for i in range(len(oscils))]

def slidermenu(sel, arr):
    slidmen = tsSliderMenu(root, frame, arr[0], arr[1], lambda: controllerMapper(), ctrlupdates)

mainmenu = tsMenu(frame, 'mainMenu')

mainmenu.addItems('mainMenu', [['Oscillators', 'oscMenu'], ['Filters', 'filterMenu'], ['Modulation', 'modList'], ['Effects', 'fxMenu'], ['Options', 'optionsMenu'], ['Shut Down', shutDown]])
mainmenu.addItems('midiSettings', [['..', 'optionsMenu'], ['Set MIDI Input', 'midiInputs']])
mainmenu.addItems('optionsMenu', [['..', 'mainMenu'], ['MIDI', 'midiSettings'], ['Voices', slidermenu, voiceSlider, [["Done", lambda: mainmenu.loadMenu('mainMenu')]]]])
mainmenu.addItems('filterTypes', [['..', 'filterMenu'], ['Moog Ladder LP', lambda x,y: [osc.changeFilter for osc in oscils]], ['4-pole SVF Filter', lambda x,y: [osc.changeFilter for osc in oscils]]])
mainmenu.addItems('filterMenu', [['..', 'mainMenu'], ['Type', 'filterTypes'], ['Parameters', slidermenu, filterslides, [["Done", lambda: mainmenu.loadMenu('mainMenu')]]]])
mainmenu.addItems('fxMenu', [['..', 'mainMenu']] + [['+', 'listOfFx'] if len(effectrack) < 4 else []])
mainmenu.addItems('listOfFx', [['Chorus', applyEffect, Chorus, [['Depth', 'depth', 0, 20, 0.6], ['Feedback', 'feedback', 0, 2, 0.3], ['Dry/Wet', 'bal', 0, 1, 0.5]], 'Chorus'], ['Freeverb', applyEffect, Freeverb, [['Size', 'size', 0, 2, 0.5], ['Damp', 'damp', 0, 2, 0.5], ['Dry/Wet', 'bal', 0, 1, 0.5]], 'Reverb'], ['Phaser', applyEffect, Phaser, [['Frequency', 'freq', 0, 135, 80, mtof], ['Feedback', 'feedback', 0, 1, 0.6], ['Width', 'q', 0, 50, 10], ['Spread', 'spread', 0, 3, 1.1], ['Poles', 'num', 0, 16, 8]], 'Phaser'], ['Delay', applyEffect, Delay, [['Time', 'delay', 0, 2, 0.5], ['Feedback', 'feedback', 0, 2, 0.5], ['Dry/Wet', 'bal', 0, 1, 0.5]], 'Delay'], ['Stereo Reverb', applyEffect, STRev, [['Size', 'roomSize', 0, 2, 0.5], ['Reverb Time', 'revtime', 0, 2, 0.5], ['Cutoff', 'cutoff', 0, 20000, 5000], ['Dry/Wet', 'bal', 0, 1, 0.5]], 'Stereo Reverb']])
mainmenu.addItems('oscMenu', [['..', 'mainMenu'], ['Oscillator 1', oscSettingsFunc], ['Oscillator 2', oscSettingsFunc]])
mainmenu.addItems('modList', [['..', 'mainMenu'], ['Volume ADSR', slidermenu, volADSRslides, [["Done", lambda: mainmenu.loadMenu('mainMenu')]]]] + [[x, 'mainMenu'] for x in lfos.keys()] + [['+', 'newMod'] if len(lfos) < 8 else []])
mainmenu.addItems('newMod', [['..', 'modList'], ['LFO', makeLFO, [1, 1, 1, 2, 3]], ['ADSR', makeADSR, [0.1, 0.5, 0.3, 0.02, 1]]])
mainmenu.addItems('midiInputs', [['..', 'midiSettings']] + [[x, setMidiInput, x] for x in midiInput.get_ports()])

def voiceHandling():
    global shutdown, busyList, timeList, noteList, envelopes, controllers, controllermove, ctrlupdates
    while shutdown == False:
        note = midiInput.get_message()
        controller = ctrlInput.get_message()
        if note != None:
            if note[0][1] in noteList:
                freevoice = noteList.index(note[0][1])
            elif False in busyList:
                freevoice = busyList.index(False)
            else:
                freevoice = timeList.index(min(timeList))
            if note[0][2] != 0 and note[0][0] == 144:
                for i in envelopes.keys():
                    envelopes[i][0][freevoice].play()
                [osc.playnote(note[0][1], freevoice) for osc in oscils]
                noteList[freevoice] = note[0][1]
                busyList[freevoice] = True
                timeList[freevoice] = time.time()
            elif note[0][2] == 0 or note[0][0] == 128:
                if note[0][1] in noteList:
                    notepos = noteList.index(note[0][1])
                    [osc.stopnote(notepos) for osc in oscils]
                    for i in envelopes.keys():
                        envelopes[i][0][notepos].stop()
                    noteList[notepos] = 0
                    busyList[notepos] = False
        if controller != None:
            if controller[0][0] == 176 and controller[0][1] < 16:
                controllers[controller[0][1]].value = controller[0][2]
                controllermove = controller[0][1]
                print(controller[0][1])
                for i in range(len(ctrlupdates)):
                    if i == controller[0][1] and ctrlupdates[i] != None:
                        ctrlupdates[i][0](controller[0][2])
                        if ctrlupdates[i][1] != None:
                            ctrlupdates[i][1](controller[0][2])
        else:
            time.sleep(0.02)
    sys.exit()

def controllerMapper():
    global controllermove
    mapDone = False
    controllermove = None
    while mapDone == False:
        if controllermove != None:
            mapDone = True
        else:
            time.sleep(0.01)
    return controllermove

busyList = [False for i in range(voices)]
timeList = [0 for i in range(voices)]
noteList = [0 for i in range(voices)]

shutdown = False
faulthandler.enable()
voicethread = threading.Thread(target=voiceHandling, args=())
voicethread.start()
root.update()
root.mainloop()
