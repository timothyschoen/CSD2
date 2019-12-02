from tkinter import *
from tkinter import ttk

class tsMenu():
    def loadMenu(self, name):
        self.menubox = Listbox(self.frame, width=40,height=10, font=('Myriad',13), disabledforeground='#464646', fg='white', selectforeground='white', selectbackground='#69BDDC', background='#464646', highlightcolor='#69BDDC', highlightbackground='#69BDDC', bd='0')
        self.menubox.selection_set(0, 1)
        self.menubox.focus()
        self.menubox.activate(0)
        self.menubox.place(x=32,y=90)
        self.menubox.pack()
        self.currentmenu = self.menuitems[name]
        for i in self.menuitems[name]:
            self.menubox.insert(END,i[0])

        def changeMenu(e, sel):
            if (e == 1 or e.char == '\r') and len(sel) > 0:
                sel = sel[0]
                self.menubox.delete(0,END)
                if isinstance(self.currentmenu[sel][1], str):
                    for i in self.menuitems[self.currentmenu[sel][1]]:
                        self.menubox.insert(END,i[0])
                    self.currentmenu = self.menuitems[self.currentmenu[sel][1]]
                elif callable(self.currentmenu[sel][1]):
                    self.menubox.destroy()
                    self.currentmenu[sel][1](sel, self.currentmenu[sel][2:])

        self.menubox.bind("<KeyPress>", lambda evt: changeMenu(evt, self.menubox.curselection()))

    def __init__(self, frame, startlist):
        self.frame = frame
        self.menuitems = {}
        self.menuitems[startlist] = []
        self.currentname = startlist
        self.loadMenu(self.currentname)


    def addItems(self, name, item):
        if name not in self.menuitems.keys():
            self.menuitems[name] = []
        [self.menuitems[name].append(i) for i in item]
        self.currentmenu = self.menuitems[self.currentname]
        try:
            self.menubox.delete(0,END)
            for i in self.currentmenu:
                self.menubox.insert(END,i[0])
        except:
            print('hey')

class tsSliderMenu():
    def __init__(self, root, frame, items, returnfunc, mapperfunc, ctrlArray):
        print(items)
        self.mapperfunc = mapperfunc
        self.ctrlArray = ctrlArray
        self.root = root
        self.items = items
        self.nextaction = returnfunc
        self.spacing = 120/len(self.items)
        self.sliderframe = Frame(frame, width=360, height=180,borderwidth=0, relief=RAISED)
        self.sliderframe.configure(background='#464646')
        self.sliderframe.pack_propagate(False)
        self.sliderframe.grid(padx=0, pady=0)
        self.sliderframe.pack(padx=0, pady=0)
        self.sliderframe.active = True
        # Name Labels
        self.selection = 0
        [Label(self.sliderframe, text=items[i][0], bg='#464646', font=('Myriad',13), fg='white').place(relx=1, x=-280, y=(i*self.spacing)+15, anchor=NE) for i in range(len(self.items))]
        self.rbuttons = [Radiobutton(self.sliderframe, value=i, variable=self.selection) for i in range(len(self.items))]
        [self.rbuttons[i].config(bg='#464646', font=('Myriad',13), fg='white') for i in range(len(self.items))]
        [self.rbuttons[i].place(relx=1, x=-330, y=(i*self.spacing)+15, anchor=NE) for i in range(len(self.items))]
        self.rbuttons[0].select()
        [self.rbuttons[i].bind() for i in range(len(self.items))]
        # Value Labels
        self.labels = [(Label(self.sliderframe, text=items[i][5])) for i in range(len(self.items)) if self.items[i][4] == 'slider']
        [self.labels[i].place(relx=1, x=-70, y=(i*self.spacing)+15, anchor=NE) for i in range(len(self.items)) if self.items[i][4] == 'slider']
        [self.labels[i].config(bg='#464646', font=('Myriad',13), fg='white') for i in range(len(self.items)) if self.items[i][4] == 'slider']

        def moveSlider():
            for i in range(len(self.sliders)):
                if self.items[i][4] == 'slider':
                    val = float(self.sliders[i].get())
                    if len(self.items[i]) > 7:
                        self.labels[i]['text'] = int(self.items[i][7](val))
                    else:
                        self.labels[i]['text'] = "{0:.2f}".format(val)
                    self.items[i][1](val)

        self.stringvars = [StringVar(self.sliderframe, self.items[i][2]) if self.items[i][4] != 'slider' else 0 for i in range(len(self.items))]
        self.sliders = [Scale(self.sliderframe, from_=self.items[i][2], to=self.items[i][3], resolution=0.01, command = lambda val: moveSlider()) if self.items[i][4] == 'slider' else OptionMenu(self.sliderframe, self.stringvars[i], *self.items[i][3], command=lambda shp: changeBox(shp, self.items[i][3], i)) for i in range(len(self.items))]
        [self.sliders[i].config(orient=HORIZONTAL, resolution=0.01, length=150, showvalue=False, fg='white', background='#464646', highlightcolor='#69BDDC', highlightbackground='#69BDDC', bd='0') if self.items[i][4] == 'slider' else self.sliders[i].config(fg='white', background='#464646', highlightcolor='#69BDDC', width=15, highlightbackground='#464646', bd='0') for i in range(len(self.items)) ]
        [self.sliders[i].place(relx=1, x=-120, y=(i*self.spacing)+19, anchor=NE) for i in range(len(self.items))]
        [self.sliders[i].set(self.items[i][5]) if self.items[i][4] == 'slider' else self.stringvars[i].set(self.items[i][3][self.items[i][5]]) for i in range(len(self.items))]

        def keydown(e):
            print(e.keysym)
            if e.keysym == 'Escape':
                close(len(self.nextaction)-1)
            elif e.keysym == 'Return':
                close(0)
            elif e.keysym == 'Up':
                self.rbuttons[self.selection].deselect()
                self.selection = (self.selection - 1)%len(self.rbuttons)
                self.rbuttons[self.selection].select()
                if self.items[self.selection][4] == 'slider':
                    self.ctrlArray[0] = [self.items[self.selection][1], lambda x: self.sliders[self.selection].set((x/127*self.items[self.selection][3]))]
                else:
                    self.ctrlArray[0] = 0
            elif e.keysym == 'Down':
                self.rbuttons[self.selection].deselect()
                self.selection = (self.selection + 1)%len(self.rbuttons)
                self.rbuttons[self.selection].select()
                if self.items[self.selection][4] == 'slider':
                    self.ctrlArray[0] = [self.items[self.selection][1], lambda x: self.sliders[self.selection].set((x/127*self.items[self.selection][3]))]
                else:
                    self.ctrlArray[0] = 0
            elif e.keysym == 'Left':
                if self.items[self.selection][4] == 'slider':
                    self.sliders[self.selection].set(self.sliders[self.selection].get()-(self.items[self.selection][3]/127))
                else:
                    self.stringvars[self.selection].set(self.items[self.selection][3][self.items[self.selection][3].index(self.stringvars[self.selection].get())-1%len(self.items[self.selection][3])])
            elif e.keysym == 'Right':
                if self.items[self.selection][4] == 'slider':
                    self.sliders[self.selection].set(self.sliders[self.selection].get()+(self.items[self.selection][3]/127))
                else:
                    self.stringvars[self.selection].set(self.items[self.selection][3][(self.items[self.selection][3].index(self.stringvars[self.selection].get())+1)%(len(self.items[self.selection][3]))])

        funcid = root.bind("<KeyPress>", keydown)
        self.mapbuttons = [Button(self.sliderframe, text='Map', command=lambda i=i: map(i)) for i in range(len(self.items)) if self.items[i][4] == 'slider']
        [self.mapbuttons[i].place(relx=1, x=-10, y=(i*self.spacing)+12, anchor=NE) for i in range(len(self.items)) if self.items[i][4] == 'slider']
        [but.config(fg='white', background='#464646', highlightcolor='#69BDDC', highlightbackground='#464646', bd='0') for but in self.mapbuttons]

        def close(which):
            self.root.unbind("<KeyPress>", funcid)
            for i in range(len(self.sliders)):
                if self.items[i][4] == 'slider':
                    self.items[i][5] = float(self.sliders[i].get())
                    if self.items[i][6] != None:
                        self.ctrlArray[self.items[i][6][0]][1] = None
                else:
                    self.items[i][5] = self.items[i][3].index(self.stringvars[i].get())
            for widget in self.sliderframe.winfo_children():
                    widget.destroy()
            self.sliderframe.destroy()
            self.nextaction[which][1]()

        for i in range(len(self.items)):
            if self.items[i][6] != None:
                but = self.items[i][6][2]
                self.ctrlArray[self.items[i][6][0]] = [self.items[i][1], lambda x: self.sliders[but].set(x)]
                self.mapbuttons[but].config(text='CC'+str(self.items[i][6][0]))

        self.buttons = [Button(self.sliderframe, text=self.nextaction[i][0], command = lambda i=i: close(i)) for i in range(len(self.nextaction))]
        hspace = 150/len(self.buttons)
        [self.buttons[b].place(relx=1, x=(b*-hspace)-120, y=142, anchor=NE) for b in range(len(self.buttons))]
        [b.config(fg='white', background='#464646', highlightcolor='#69BDDC', highlightbackground='#464646', bd='0') for b in self.buttons]

        def map(but):
            if items[but][6] == None:
                self.mapbuttons[but].config(text='...', fg='black', background='#464646', highlightcolor='#69BDDC', highlightbackground='#464646', bd='0')
                self.sliderframe.update()
                ctrl = self.mapperfunc()
                print(ctrl)
                self.sliderframe.update()
                self.mapbuttons[but].config(text='CC'+str(ctrl), fg='white', background='#464646', highlightcolor='#69BDDC', highlightbackground='#464646', bd='0')
                self.ctrlArray[ctrl] = [self.items[but][1], lambda x: self.sliders[but].set((x/127*self.items[but][3]))]
                self.items[but][6] = [int(ctrl), self.items[but][1], but]
            else:
                self.ctrlArray[self.items[but][6][0]] = None
                self.items[but][6] = None
            self.mapbuttons[but].config(text='Map', fg='white', background='#464646', highlightcolor='#69BDDC', highlightbackground='#464646', bd='0')

    def changeBox(self, shape, options, i):
        self.items[i][1](options.index(shape))

i = 0

def main():
    root = Tk()
    root.geometry("360x180")
    frame = Frame(root, width=360, height=180,borderwidth=0, relief=RAISED)
    frame.configure(background='#464646')
    frame.pack_propagate(False)
    frame.grid(padx=0, pady=0)
    frame.pack(padx=0, pady=0)
    frame.active = True
    mainmenu = tsMenu(frame, 'main', 'test', 'etc')
    arr = [0 for i in range(16)]

    def fakectrlmapper():
        global i
        print(arr)
        i = i+1
        return i


    def makeMenu(x, y):
        slidmen = tsSliderMenu(root, frame, filterslides, [['Done', lambda: mainmenu.loadMenu('main')]], lambda: fakectrlmapper(), arr)

    mainmenu.addItems('main', [['test', 'test']])
    mainmenu.addItems('test', [['main', 'main'], ['etc', makeMenu]])
    filterslides = [['Cutoff', lambda y : print(y), 15, 134.9851, 'slider', 100, None, lambda y : y*2], ['Res.', lambda y : print(y), 0, 1.5, 'slider', 0.6, None]]

    root.update()
    root.mainloop()


#main()
