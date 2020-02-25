// I'll annotate this later because it became huuge
// Might split out some components into other files as well...

// Seperator lines
let seperator1, seperator2;

let onswitch;
let sbarwidth = 300;
let cnvwidth; // Width of our canvas without the sidebar

function Sidebar() {
    let textformat;
    let hidden = 0;
    let sbaroffset, sbardragging; // variables for our dragging feature
    let _this = this;

    // Do this in html because it's lighter and responds to resizes better
    seperator1 = document.createElement('hr');
    seperator1.style.position = "fixed";
    seperator1.style.height = "0px";
    seperator1.style.width = window.innerHeight-35 + "px";
    seperator1.style.background = "#FFFFFf";
    seperator1.style.top = "10px";
    seperator1.style.right = sbarwidth-40 + "px";



    seperator2 = document.createElement('hr');
    seperator1.style.position = "fixed";
    seperator1.style.height = "0px";
    seperator1.style.width = window.innerHeight-35 + "px";
    seperator1.style.background = "#FFFFFF";
    seperator1.style.top = "10px";
    seperator1.style.right = sbarwidth-10 + "px";


    // label for the console
    let modlabel = document.createElement("div");
    modlabel.innerHTML = 'Console:';
    modlabel.style.position = "fixed";
    modlabel.style.top = "5%";
    modlabel.style.left = cnvwidth+51 + "px";
    modlabel.style.fontSize = "5";
    modlabel.style.fontFamily = "monospace";
    modlabel.style.fontWeight = "bold";

  //  modlabel.style.cssText ="fontSize:5; fontFamily:monospace; font-weight: bold; position:fixed; left:x0px; top:5%".replace('x0', cnvwidth+51));
    //modlabel.hide();

    //Console logviewer object created in console-viewer.js!
    logviewer.windowResize(sbarwidth-60, window.innerHeight-200);

    // Initialize all sidebar components (except the console)

    let jackoptions = new jackOptions();

    let haliteoptions = new haliteOptions(); // Same tab as jackoptions

    let medialib = new mediaLibrary();

    let codebox = new codeBox();


    // Function to open a tab and hide the rest

    function showTab(tab) {
        // Show/hide
        if (!tab && !hidden) {
            buttons[0].elt.textContent = "<";
            hidden = 1;
            sbarwidth = 38;
            _this.windowresize();
        }
        else {
            buttons[0].elt.textContent = ">";
            if(!tab) {
                tab = 2;
            }
            sbarwidth = 350;
            hidden = 0;
            _this.windowresize();
        }
        let tabs = [[], [jackoptions, haliteoptions], [logviewer, modlabel], [medialib], [codebox]];
        for (var i = 0; i < 5; i++) {
            if (i != tab) {
                tabs[i].forEach((t) => t.hide());
            }
        }
        if (tab != 0) {
            tabs[tab].forEach((t) => t.show());
        }
    }

    let buttonpresets = [['>', 4, function() {
        showTab(0)
    }], ['J', 43, function() {
        showTab(1)
    }],
    ['C', 50, function() {
             showTab(2)
    }], ['E', 57, function() {
        showTab(4)
    }],
    ['M', 64, function() {
             showTab(3)
         }]];
    let buttons = [];

    for (let i = 0; i < buttonpresets.length; i++) {
        buttons[i] = document.createElement("BUTTON", buttonpresets[i][0])
                     buttons[i].size(22, 22);
        buttons[i].style.cssText ="border-radius:100%; border:none; outline:none; font-size:7; text-align:left; color:white; background-color:#229FD7; position:fixed; right:x0px; top:x1%;".replace('x0', sbarwidth-34).replace('x1', buttonpresets[i][1]);
        buttons[i].mousePressed(buttonpresets[i][2]);
    }


    this.windowresize = function() {
        cnvwidth = window.innerWidth-sbarwidth;

        seperator1.style.cssText ="right:x0px; ".replace('x0', sbarwidth-40);
        seperator1.size(0, window.innerHeight-35);
        seperator2.style.cssText ="right:x0px".replace('x0', sbarwidth-10);
        seperator2.size(0, window.innerHeight-35);

        for (let i = 0; i < buttons.length; i++) {
            buttons[i].style.cssText ="right:x0px;".replace('x0', sbarwidth-34);
        }

        jackoptions.windowResize();
        haliteoptions.windowResize();
        medialib.windowresize();
        codebox.windowResize();
        modlabel.innerHTML = 'Console:';
        modlabel.style.left = cnvwidth+51 + "px";


        logviewer.windowResize(sbarwidth-60, window.innerHeight-50);
    }

    this.codeUpdate = function(ncode) {
        return codebox.value(ncode);
    }
    this.libraryUpdate = function() {
        medialib.update();
    }
    this.getJackStatus = function() {
        return jackstatus;
    }

    this.draw = function() {
        if (mouseX > cnvwidth && mouseX < cnvwidth + 30 && draginstance == -1) {
            if(mouseIsPressed) {
                sbaroffset = cnvwidth - mouseX;
                sbardragging = true;
            }
            else if (sbardragging || draginstance != -1) {
                sbardragging = false;
            }
        }

        if (sbardragging && draginstance == -1) {
            sbarwidth = window.innerWidth - (mouseX + sbaroffset);
            textformat = new RegExp("(.{" + int((sbarwidth/9)-3).toString() + "})", "g");
            if(sbarwidth < 250) {
                sbarwidth = 250
            }
            windowResized();

        }

    }
    this.draw2 = function() {
        let xscrollOffset = window.pageXOffset;
        let yscrollOffset = window.pageYOffset*2;
        fill(255);
        noStroke()
        rect(cnvwidth+xscrollOffset, yscrollOffset, sbarwidth, height);


    }
}

function jackOptions() {
    let jack;

    let modlabel = document.createElement("div", 'Jack Settings:');
    modlabel.style.cssText = "fontSize:5; fontFamily:monospace; font-weight: bold; position:fixed; left:x0px; top:5%".replace('x0', cnvwidth+51);


    let driverlabel = document.createElement("div", 'Driver:');
    driverlabel.style.cssText ="fontSize:3; fontFamily:monospace; position:fixed; left:x0px; top:10.5%".replace('x0', cnvwidth+51)

    let driverlist = ['coreaudio', 'alsa', 'portaudio'];
    let driversel = document.createElement("SELECT");
    driversel.style.cssText ="font-size:7; position:fixed; right:11px; top:10%";
    driversel.size(100, 20);

    for (let i = 0; i < driverlist.length; i++) {
        driversel.option(driverlist[i])
    }


    let srlabel = document.createElement("div", 'Sample rate:');
    srlabel.style.cssText ="fontSize:3; fontFamily:monospace; position:fixed; left:x0px; top:18.5%".replace('x0', cnvwidth+51);

    let samplesrates = [44100, 22050, 32000, 48000, 88200, 96000];
    let sampleratesel = createSelect('44100');
    sampleratesel.style.cssText ="font-size:7; position:fixed; right:11px; top:18%";
    sampleratesel.size(100, 20);


    for (let i = 0; i < samplesrates.length; i++) {
        sampleratesel.option(samplesrates[i])
    }

    let buflabel = document.createElement("div", 'Buffer size:');
    buflabel.style.cssText ="fontSize:3; fontFamily:monospace; position:fixed; left:x0px; top:26.5%".replace('x0', cnvwidth+51);

    let bufsizes = [4096, 256, 512, 1024, 2048];
    let buffersel = document.createElement("SELECT");
    buffersel.style.cssText = "font-size:7; position:fixed; right:11px; top:26%";
    buffersel.size(100, 20);

    for (let i = 0; i < bufsizes.length; i++) {
        buffersel.option(bufsizes[i])
    }

    let jackcommand = 'jackd' + ' -d'+driversel.value() + ' -r'+sampleratesel.value() + ' -p'+buffersel.value();


    let inputlabel = document.createElement("div", 'Command:');
    inputlabel.style.cssText ="fontSize:3; fontFamily:monospace; position:fixed; left:x0px; top:35%".replace('x0', cnvwidth+51);
    let inputcmd = createInput(jackcommand);
    inputcmd.style.cssText ="font-size:7; position:fixed; right:11px; top:34%";
    inputcmd.size(sbarwidth-150, 20);

    onswitch = document.createElement("BUTTON", 'OFF');
    onswitch.size(48, 32);
    onswitch.style.cssText = "border:none; outline:none; font-size:7; color:white; background-color:#229FD7; position:fixed; left:x0px; top:42%".replace('x0', cnvwidth+(sbarwidth/2));
    onswitch.mousePressed(() => {
        if (jackstatus == 1) {
            jack.kill('SIGINT');
            onswitch.elt.textContent = "OFF";
        }
        else {
            let splitcmd = inputcmd.value().split(' ');
            jack = spawn(splitcmd[0], splitcmd.slice(1));
            onswitch.elt.textContent = "ON";
            jack.stdout.on('data', function (data) {
                console.log('Jack: ' + data);
            });

            jack.stderr.on('data', function (data) {
                console.warn('Jack Error: ' + data);
            });

            jack.on('close', function (code) {
                jackstatus = 0;
                onswitch.elt.textContent = 'OFF'
                                           console.log('Jack closed with code ' + code);
                if (code) {
                    console.warn('Jack has crashed');
                    console.warn('Please make sure you are not already running a jack server');
                }

            });

        }
        jackstatus = !jackstatus;
    });

    this.update = function() {
        jackcommand = 'jackd' + ' -d'+driversel.value() + ' -r'+sampleratesel.value() + ' -p'+buffersel.value();
        inputcmd.value(jackcommand);

    }

    driversel.changed(this.update);
    buffersel.changed(this.update);
    sampleratesel.changed(this.update);

    this.windowResize = function() {
        modlabel.style.left = cnvwidth+51 + "px";
        onswitch.style.cssText ="left:x0px;".replace('x0', cnvwidth+(sbarwidth/2));
        inputcmd.style.cssText ="width:x0px;".replace('x0', sbarwidth-180);
        inputlabel.style.cssText ="left:x0px;".replace('x0', cnvwidth+51);

        driversel.style.cssText ="width:x0px;".replace('x0', sbarwidth-180);
        sampleratesel.style.cssText ="width:x0px;".replace('x0', sbarwidth-180);
        srlabel.style.cssText ="left:x0px;".replace('x0', cnvwidth+51);
        driverlabel.style.cssText ="left:x0px;".replace('x0', cnvwidth+51);
        buflabel.style.cssText ="left:x0px;".replace('x0', cnvwidth+51);
        buffersel.style.cssText ="width:x0px;".replace('x0', sbarwidth-180);
    }



    this.show = function() {
        modlabel.show()
        onswitch.show();
        inputcmd.show();
        buffersel.show();
        sampleratesel.show();
        inputlabel.show();
        buflabel.show();
        srlabel.show();
        driversel.show();
        driverlabel.show();
    }

    this.hide = function() {
        modlabel.hide()
        onswitch.hide();
        inputlabel.hide();
        inputcmd.hide();
        buffersel.hide();
        sampleratesel.hide();
        buflabel.hide();
        srlabel.hide();
        driversel.hide();
        driverlabel.hide();
    }

    this.hide();
}

function mediaLibrary() {

    let filebuttonlist = [];

    let hidden = 1;



    let __this = this;

    let modlabel = document.createElement("div", 'Media Library:');
    //modlabel.style.cssText ="fontSize:5; fontFamily:monospace; font-weight: bold; position:fixed; left:x0px; top:5%".replace('x0', cnvwidth+51));


    let div = document.document.createElement('div');
    div.style.position = "fixed";
    div.style.font = "monospace";
    div.style.right = "0px";
    div.style.top = "55px";
    div.style.height = (window.innerHeight-70).toString()+'px'
                       div.style.width = (sbarwidth-39).toString()+'px'
                                         div.style.overflow = "auto";
    div.style.background = "white";




    this.addItem = function(err, files) {  // THROWS ERROR ON COMPILATION!!!!!
        if (files != undefined) {
            for (let i = 0; i < files.length; i++) {
                filebuttonlist[i] = new mediaButton(files[i])
                filebuttonlist[i].style()
            }
        }
    }
    this.update = function() {
        filebuttonlist = [];
        fs.readdir("./media", this.addItem);
        if (!hidden) {
            setTimeout(function()
            {
                __this.hide();
                __this.show();

            }, 100);
        }

    }

    function mediaButton(file) {
        let _this = this;

        this.style = function() {
            let top = filebuttonlist.indexOf(_this)

                      img.style.top = (11+(32*top)).toString() + "px";
            img.style.right = "50px"
            middlebutton.style.cssText ="border:none; outline:none; font-size:7; text-align:left; color:white; background-color:x3; position:absolute; left:x0px; top:x2px; width:x1px; height:32px;".replace('x0', 30).replace('x1', sbarwidth-100).replace('x2', top*32).replace('x3', ['#229FD7', '#0094d6'][top%2]);
            leftbutton.style.cssText ="border:none; outline:none; font-size:7; text-align:left; color:white; background-color:x3; position:absolute; left:x0px; top:x2px; width:x1px; height:32px;".replace('x0', 0).replace('x1', 30).replace('x2', top*32).replace('x3', ['#229FD7', '#0094d6'][top%2]);
            rightbutton.style.cssText ="border:none; outline:none; font-size:7; text-align:center; color:white; background-color:x3; position:absolute; right:x0px; top:x2px; width:x1px; height:32px;".replace('x0', 0).replace('x1', 39).replace('x2', top*32).replace('x3', ['#229FD7', '#0094d6'][top%2]);

        };
        this.show = function() {
            ;
            middlebutton.show()
            leftbutton.show()
            rightbutton.show()
        }

        this.hide = function() {
            middlebutton.hide();
            leftbutton.hide()
            rightbutton.hide()
        };
        this.windowResize = function() {
            img.style.left = "10px"
                             middlebutton.style.cssText ="left:x0px; width:x1px; height:32px".replace('x0', 30).replace('x1', sbarwidth-100);
            leftbutton.style.cssText ="left:x0px; height:32px; width:x1px".replace('x0', 0).replace('x1', 30);
            rightbutton.style.cssText ="right:x0px; height:32px; width:x1px".replace('x0', 0).replace('x1', 39);

        };
        this.mousePressed = function() {
            boxes.push(new Component('input ./media/file 0.2'.replace('file', file), window.innerWidth/2, window.innerHeight/2-250));
        };


        this.deleteFile = function() {
            fs.unlinkSync('./media/'+file, function (err) {
                if (err) throw err;
            });
            middlebutton.remove();
            leftbutton.remove()
            rightbutton.remove()
            filebuttonlist.splice(filebuttonlist.indexOf(_this), 1)
            filebuttonlist.forEach(button => button.style());
        };



        let leftbutton = document.createElement("BUTTON", '  ');
        leftbutton.mousePressed(this.mousePressed);
        leftbutton.style.zIndex = "0";

        let middlebutton = document.createElement("BUTTON", file);;
        middlebutton.mousePressed(this.mousePressed);

        let rightbutton = document.createElement("BUTTON", 'x');
        rightbutton.mousePressed(this.deleteFile);


        let img = new Image(10, 10);
        img.src = "../assets/audio-symbol.png";
        img.style.position = 'absolute';
        img.style.left = '10px';


        div.appendChild(rightbutton.elt);
        div.appendChild(leftbutton.elt);
        div.appendChild(middlebutton.elt);
        div.appendChild(img);

        this.hide();


    }
    document.body.appendChild(div);



    this.show = function() {
        for (let i = 0; i < filebuttonlist.length; i++) {
            filebuttonlist[i].show()
        }
        div.style.display = ""
                            modlabel.show()
                            hidden = 0;
    }
    this.hide = function() {
        for (let i = 0; i < filebuttonlist.length; i++) {
            filebuttonlist[i].hide()
        }
        div.style.display = "none"
                            modlabel.hide()
                            hidden = 1;
    }
    this.windowresize = function() {
        for (let i = 0; i < filebuttonlist.length; i++) {
            filebuttonlist[i].windowResize();
        }
        //modlabel.style.cssText ="left:x0px;".replace('x0', cnvwidth+51));
        div.style.height = (window.innerHeight-70).toString()+'px'
                           div.style.width = (sbarwidth-39).toString()+'px'
                                             div.scrollTop = div.scrollHeight+200;



    }

    this.update();
    this.hide()


}

function haliteOptions() {
    let modlabel = document.createElement("div", 'Halite Settings:');
    //modlabel.style.cssText ="fontSize:5; fontFamily:monospace; font-weight: bold; position:fixed; left:x0px; top:53%".replace('x0', cnvwidth+51));

    let srenginelabel = document.createElement("div", 'Halite engine sample rate:');
    srenginelabel.style.cssText ="fontSize:3; fontFamily:monospace; position:fixed; left:x0px; top:60.5%".replace('x0', cnvwidth+51)

    let samplerates = [44100, 22050, 32000, 48000, 88200, 96000];
    let srenginesel = document.createElement("SELECT");
    srenginesel.style.cssText = "font-size:7; position:fixed; right:11px; top:60%";
    srenginesel.size(100, 20);


    let sroutputlabel = document.createElement("div", 'File export sample rate:');
    sroutputlabel.style.cssText ="fontSize:3; fontFamily:monospace; position:fixed; left:x0px; top:68.5%".replace('x0', cnvwidth+51);


    let sroutputsel = document.createElement("SELECT");
    sroutputsel.style.cssText = "font-size:7; position:fixed; right:11px; top:68%";
    sroutputsel.size(100, 20);

    for (let i = 0; i < samplerates.length; i++) {
        srenginesel.option(samplerates[i]);
        sroutputsel.option(samplerates[i]);
    }

    let depths = [24, 8, 16, 32];

    let depthlabel = document.createElement("div", 'File export bit depth:');
    depthlabel.style.cssText ="fontSize:3; fontFamily:monospace; position:fixed; left:x0px; top:76.5%".replace('x0', cnvwidth+51);

    let depthsel = document.createElement("SELECT");
    depthsel.style.cssText ="font-size:7; position:fixed; right:11px; top:76%";
    depthsel.size(100, 20);


    for (let i = 0; i < depths.length; i++) {
        depthsel.option(depths[i]);
    }

    let formats = ['WAV', 'AIFF'];

    let formatlabel = document.createElement("div", 'Export format:');
    formatlabel.style.cssText ="fontSize:3; fontFamily:monospace; position:fixed; left:x0px; top:84.5%".replace('x0', cnvwidth+51);

    let formatsel = document.createElement("SELECT");
    formatsel.style.cssText ="font-size:7; position:fixed; right:11px; top:84%";
    formatsel.size(100, 20);

    for (let i = 0; i < formats.length; i++) {
        formatsel.option(formats[i]);
    }


    this.update = function() {
        halsettings[0] = srenginesel.value()
                         halsettings[1] = sroutputsel.value()
                                          halsettings[2] = depthsel.value()
                                                  halsettings[3] = formatsel.value()
    }

    depthsel.changed(this.update);
    formatsel.changed(this.update);
    sroutputsel.changed(this.update);
    srenginesel.changed(this.update);


    this.windowResize = function() {
        modlabel.style.cssText = "left:x0px;".replace('x0', cnvwidth+51);
        srenginesel.style.cssText = "width:x0px;".replace('x0', sbarwidth-280);
        srenginelabel.style.cssText = "left:x0px;".replace('x0', cnvwidth+51);
        sroutputsel.style.cssText = "width:x0px;".replace('x0', sbarwidth-280);
        sroutputlabel.style.cssText = "left:x0px;".replace('x0', cnvwidth+51);
        depthsel.style.cssText = "width:x0px;".replace('x0', sbarwidth-280);
        depthlabel.style.cssText = "left:x0px;".replace('x0', cnvwidth+51);
        formatsel.style.cssText = "width:x0px;".replace('x0', sbarwidth-280);
        formatlabel.style.cssText = "left:x0px;".replace('x0', cnvwidth+51);

    }

    this.show = function()  {
        modlabel.show()
        srenginesel.show()
        srenginelabel.show()
        sroutputsel.show()
        sroutputlabel.show()
        depthsel.show()
        depthlabel.show()
        formatsel.show()
        formatlabel.show()
    }

    this.hide = function()  {
        modlabel.hide()
        srenginesel.hide()
        srenginelabel.hide()
        sroutputsel.hide()
        sroutputlabel.hide()
        depthsel.hide()
        depthlabel.hide()
        formatsel.hide()
        formatlabel.hide()
    }

    this.hide();


}

function codeBox() {

    let modlabel = document.createElement("div", 'Code editor:');
    modlabel.style.cssText ="fontSize:5; fontFamily:monospace; font-weight: bold; position:fixed; left:x0px; top:5%".replace('x0', cnvwidth+51);


    let codebox = document.createElement('textarea', code);
    codebox.elt.style.position = "fixed";
    codebox.size(sbarwidth-70, window.innerHeight-149);
    codebox.elt.style.top = "60px";
    codebox.elt.style.right = "10px";
    codebox.elt.style.resize = "none";
    codebox.elt.style.display = "none";

    let coderefresh = document.createElement("BUTTON", 'Update code');
    coderefresh.size(48, 32);
    coderefresh.style.cssText ="border:none; outline:none; font-size:7; color:white; background-color:#229FD7; position:fixed; width:60px; left:x0px; top:90%".replace('x0', cnvwidth+(sbarwidth/2)-50);
    coderefresh.mousePressed(() => {
        precompile(0);
    });

    let coderun = document.createElement("BUTTON", 'Run code realtime');
    coderun.size(48, 32);
    coderun.style.cssText ="border:none; outline:none; font-size:7; color:white; background-color:#229FD7; position:fixed; width:60px; left:x0px; top:90%".replace('x0', cnvwidth+(sbarwidth/2)+0);
    coderun.mousePressed(() => {
        fs.writeFileSync("./precompile.hcl", codebox.value());
        startHalite(1);
    });

    let rendercode = document.createElement("BUTTON", 'Render code');
    rendercode.size(48, 32);
    rendercode.style.cssText ="border:none; outline:none; font-size:7; color:white; background-color:#229FD7; position:fixed; width:60px; left:x0px; top:90%".replace('x0', cnvwidth+(sbarwidth/2)+50);
    rendercode.mousePressed(() => {
        fs.writeFileSync("./precompile.hcl", codebox.value());
        startHalite(0);
    });

    this.value = function(val) {
        codebox.value(val);
    }

    this.windowResize = function() {
        codebox.size(sbarwidth-70, window.innerHeight-149);
        rendercode.style.cssText ="left:x0px;".replace('x0', cnvwidth+(sbarwidth/2)+70);
        coderun.style.cssText ="left:x0px;".replace('x0', cnvwidth+(sbarwidth/2)+0);
        coderefresh.style.cssText ="left:x0px;".replace('x0', cnvwidth+(sbarwidth/2)-70);
        modlabel.style.cssText ="left:x0px;".replace('x0', cnvwidth+51);
    }

    this.show = function()  {
        codebox.elt.style.display = "";
        rendercode.show();
        coderun.show();
        modlabel.show()
        coderefresh.show();
    }


    this.hide = function()  {
        codebox.elt.style.display = "none";
        rendercode.hide();
        coderun.hide();
        coderefresh.hide();
        modlabel.hide()
    }

    this.hide();
}
