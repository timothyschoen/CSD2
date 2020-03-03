// I'll annotate this later because it became huuge
// Might split out some components into other files as well...

// Seperator lines
let bar;

let onswitch;
sbarwidth = 350;
let cnvwidth = window.innerWidth-300; // Width of our canvas without the sidebar

let code = '';

let tab = 1;

function Sidebar() {
    let textformat;
    let hidden = 0;
    let sbaroffset, sbardragging; // variables for our dragging feature
    let _this = this;

    let bar = document.createElement("div");
    bar.style.position = "fixed";
    bar.style.width = "30px";
    bar.style.height = window.innerHeight + "px";
    bar.style.background = "#333";
    bar.style.top = "0px";
    bar.style.right = sbarwidth-30 + "px";
    bar.style.zIndex = "0";


    let bg = document.createElement("div");
    bg.style.position = "fixed";
    bg.style.background = "#444";
    bg.style.top = "0px";
    bg.style.right = "0px";
    bg.style.height = window.innerHeight + "px";
    bg.style.width = sbarwidth + "px";
    bg.style.zIndex = "-1";
    //bar.appendChild(bg);

    document.body.appendChild(bg);
    document.body.appendChild(bar);


    dragElement(bar, this);

    // label for the console
    let modlabel = document.createElement("div");
    modlabel.innerHTML = 'Console:';
    modlabel.hide = () => {modlabel.style.display = "none"}
    modlabel.show = () => {modlabel.style.display = "block"}
    modlabel.style.position = "fixed";
    modlabel.style.top = "45px";
    modlabel.style.right = sbarwidth-105 + "px";
    modlabel.style.fontSize = "14px";
    modlabel.style.color = "#dcdcdc";
    modlabel.style.fontFamily = "sans-serif";
    modlabel.style.fontWeight = "bold";
    document.body.appendChild(modlabel);


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
            buttons[0].textContent = "<";
            hidden = 1;
            sbarwidth = 38;
            cnvwidth = window.innerWidth-38
            _this.windowresize();
        }
        else {
            buttons[0].textContent = ">";
            if(!tab) {
                tab = 2;
            }
            sbarwidth = 350;
            cnvwidth = window.innerWidth-350
            hidden = 0;
            _this.windowresize();
        }
        let tabs = [[], [jackoptions, haliteoptions], [logviewer, modlabel], [medialib],  [codebox]];
        for (var i = 0; i < 5; i++) {
            if (i != tab) {
                tabs[i].forEach((t) =>  t.hide()); //t.hide());
            }
        }
        if (tab != 0) {
            tabs[tab].forEach((t) => t.show());
        }

        //this.windowresize();
    }

    let buttonpresets = [['>', '35px', function() {
        showTab(0)
    }], ['J', '44%', function() {
        showTab(1)
    }],
    ['C', '50%', function() {
             showTab(2)
    }], ['E', '56%', function() {
        showTab(4)
    }],
    ['M', '62%', function() {
             showTab(3)
         }]];
    let buttons = [];

    for (let i = 0; i < buttonpresets.length; i++) {
        buttons[i] = document.createElement("BUTTON");
        buttons[i].innerHTML = buttonpresets[i][0];
        buttons[i].style.cssText = "border:none; outline:none; font-size:14px; text-align:center; color:white; background-color:#202020; position:absolute; right:0px; top:x1;".replace('x0', sbarwidth-38).replace('x1', buttonpresets[i][1]);
        buttons[i].style.width = "30px";
        buttons[i].style.height = "27px";
        buttons[i].addEventListener("click", buttonpresets[i][2]);
        bar.appendChild(buttons[i])
    }


    this.windowresize = function() {
      bg.style.width = sbarwidth + "px";
      bar.style.right = sbarwidth-30 + "px";

      bg.style.height = window.innerHeight + "px";
      bar.style.height = window.innerHeight + "px";


        for (let i = 0; i < buttons.length; i++) {
          buttons[i].style.width = "30px";
          buttons[i].style.height = "27px";
        }

        jackoptions.windowResize();
        haliteoptions.windowResize();
        medialib.windowresize();
        codebox.windowResize();
        modlabel.style.right = sbarwidth-105 + "px";


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
  }



function jackOptions() {
    let jack;

    let modlabel = document.createElement("div");
    modlabel.innerHTML = 'Jack Settings:';
    modlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; font-weight: bold; position:fixed; right:x0px; top:45px".replace('x0', sbarwidth-145);
    document.body.appendChild(modlabel);

    let driverlabel = document.createElement("div");
    driverlabel.innerHTML = 'Driver:';
    driverlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:10.5%".replace('x0', sbarwidth-135)
    document.body.appendChild(driverlabel);

    let driverlist = ['coreaudio', 'alsa', 'portaudio'];
    let driversel = document.createElement("SELECT");
    driversel.style.cssText = "font-size:12px; position:fixed; right:11px; top:10%";
    driversel.style.width = "100px"
    driversel.style.height = "20px"
    document.body.appendChild(driversel);

    for (let i = 0; i < driverlist.length; i++) {
      let opt = document.createElement('option');
      opt.appendChild( document.createTextNode(driverlist[i]));
      opt.value = driverlist[i];
      driversel.appendChild(opt);
    }


    let srlabel = document.createElement("div");
    srlabel.innerHTML = 'Sample rate:';
    srlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:18.5%".replace('x0', sbarwidth-135);
    document.body.appendChild(srlabel);

    let samplesrates = [44100, 22050, 32000, 48000, 88200, 96000];
    let sampleratesel = document.createElement("SELECT");
    sampleratesel.style.cssText = "font-size:12px; position:fixed; right:11px; top:18%";
    sampleratesel.style.width = "100px"
    sampleratesel.style.height = "20px"
    document.body.appendChild(sampleratesel);


    for (let i = 0; i < samplesrates.length; i++) {
      let opt = document.createElement('option');
      opt.appendChild( document.createTextNode(samplesrates[i]));
      opt.value = samplesrates[i];
      sampleratesel.appendChild(opt);
    }

    let buflabel = document.createElement("div");
    buflabel.innerHTML = 'Buffer size:';
    buflabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:26.5%".replace('x0', sbarwidth-135);
    document.body.appendChild(buflabel);

    let bufsizes = [4096, 256, 512, 1024, 2048];
    let buffersel = document.createElement("SELECT");
    buffersel.style.cssText = "font-size:12px; position:fixed; right:11px; top:26%";
    buffersel.style.width = "100px"
    buffersel.style.height = "20px"
    document.body.appendChild(buffersel);

    for (let i = 0; i < bufsizes.length; i++) {
      let opt = document.createElement('option');
      opt.appendChild( document.createTextNode(bufsizes[i]));
      opt.value = bufsizes[i];
      buffersel.appendChild(opt);
    }

    let jackcommand = 'jackd' + ' -d'+driversel.value + ' -r'+sampleratesel.value + ' -p'+buffersel.value;


    let inputlabel = document.createElement("div");
    inputlabel.innerHTML =  'Command:';
    inputlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:35%".replace('x0', sbarwidth-135);
    document.body.appendChild(inputlabel);

    let inputcmd = document.createElement("INPUT");
    inputcmd.style.cssText = "font-size:14px; position:fixed; right:11px; top:34%";
    inputcmd.style.width = sbarwidth-150 + "px";
    inputcmd.style.height = "20px";
    document.body.appendChild(inputcmd);

    onswitch = document.createElement("BUTTON");
    onswitch.style.cssText = "border:none; outline:none; font-size:14px; color:white; background-color:#303030; position:fixed; right:x0px; top:42%".replace('x0', cnvwidth+(sbarwidth/2));
    onswitch.style.width = "48px";
    onswitch.style.height = "32px";
    onswitch.innerHTML = "OFF";
    onswitch.addEventListener("click", () => {
        if (jackstatus == 1) {
            jack.kill('SIGINT');
            onswitch.innerHTML = "OFF";
            onswitch.style.backgroundColor = "#303030";
        }
        else {
            let splitcmd = inputcmd.value.split(' ');
            jack = spawn(splitcmd[0], splitcmd.slice(1));
            onswitch.innerHTML = "ON";
            onswitch.style.backgroundColor = "#229FD7";
            jack.stdout.on('data', function (data) {
                console.log('Jack: ' + data);
            });

            jack.stderr.on('data', function (data) {
                console.warn('Jack Error: ' + data);
            });

            jack.on('close', function (code) {
                jackstatus = 0;
                onswitch.innerHTML = 'OFF'
                onswitch.style.backgroundColor = "#303030";
                                           console.log('Jack closed with code ' + code);
                if (code) {
                    console.warn('Jack has crashed');
                    console.warn('Please make sure you are not already running a jack server');
                }

            });

        }
        jackstatus = !jackstatus;
    });
    document.body.appendChild(onswitch);

    this.update = function() {
        jackcommand = 'jackd' + ' -d'+driversel.value + ' -r'+sampleratesel.value + ' -p'+buffersel.value;
        inputcmd.value = jackcommand;
        inputcmd.innerHTML = jackcommand;

    }

    driversel.onchange = this.update;
    buffersel.onchange = this.update;
    sampleratesel.onchange = this.update;

    this.windowResize = function() {

        modlabel.style.right = sbarwidth-145 + "px";
        onswitch.style.right = sbarwidth-(sbarwidth/1.6) + "px";
        inputcmd.style.width  = sbarwidth-180 + "px";
        inputlabel.style.right = sbarwidth-135 + "px";

        driversel.style.width = sbarwidth-180 + "px";
        sampleratesel.style.width = sbarwidth-180 + "px";
        srlabel.style.right = sbarwidth-135 + "px";
        driverlabel.style.right = sbarwidth-135 + "px"
        buflabel.style.right = sbarwidth-135 + "px";
        buffersel.style.width = sbarwidth-180 + "px";
    }



    this.show = function() {
        modlabel.style.display = "block"
        onswitch.style.display = "block";
        inputcmd.style.display = "block";
        buffersel.style.display = "block";
        sampleratesel.style.display = "block";
        inputlabel.style.display = "block";
        buflabel.style.display = "block";
        srlabel.style.display = "block";
        driversel.style.display = "block";
        driverlabel.style.display = "block";
    }

    this.hide = function() {
        modlabel.style.display = "none"
        onswitch.style.display = "none";
        inputlabel.style.display = "none";
        inputcmd.style.display = "none";
        buffersel.style.display = "none";
        sampleratesel.style.display = "none";
        buflabel.style.display = "none";
        srlabel.style.display = "none";
        driversel.style.display = "none";
        driverlabel.style.display = "none";
    }

    this.hide();
}

function mediaLibrary() {

    let filebuttonlist = [];

    let hidden = 1;



    let __this = this;

    let modlabel = document.createElement("div");
    modlabel.innerHTML = 'Media Library:';
    modlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; font-weight: bold; position:fixed; right:x0px; top:45px".replace('x0', sbarwidth-151);
    document.body.appendChild(modlabel);

    let div = document.createElement('div');
    div.style.position = "fixed";
    div.style.font = "sans-serif";
    div.style.right = "0px";
    div.style.top = "75px";
    div.style.height = (window.innerHeight-70) +'px'
                       div.style.width = sbarwidth-30 +'px'
                                         div.style.overflow = "auto";
    div.style.background = "#444";




    this.addItem = function(err, files) {
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

            img.style.top = (11+(32*top)) + "px";
            img.style.right = "50px"
            middlebutton.style.cssText ="border:none; outline:none; font-size:14px; text-align:left; color:white; background-color:x3; position:absolute; left:32px; top:x2px; width:x1px; height:32px;".replace('x0', 30).replace('x1', sbarwidth-95).replace('x2', top*32).replace('x3', ['#303030', '#383838'][top%2]);
            middlebutton.innerHTML = file;
            leftbutton.style.cssText ="border:none; outline:none; font-size:14px; text-align:left; color:white; background-color:x3; position:absolute; left:0px; top:x2px; width:32px; height:32px;".replace('x1', 30).replace('x2', top*32).replace('x3', ['#303030', '#383838'][top%2]);
            rightbutton.style.cssText ="border:none; outline:none; font-size:14px; text-align:center; color:white; background-color:x3; position:absolute; right:0px; top:x2px; width:x1px; height:32px;".replace('x1', 39).replace('x2', top*32).replace('x3', ['#303030', '#383838'][top%2]);
        };
        this.show = function() {
            middlebutton.style.display = "block"
            leftbutton.style.display = "block"
            rightbutton.style.display = "block"
        }

        this.hide = function() {
            middlebutton.style.display = "none";
            leftbutton.style.display = "none"
            rightbutton.style.display = "none"
        };
        this.windowResize = function() {
            img.style.left = "10px"
            middlebutton.style.width = sbarwidth-100 + "px";


        };
        this.onclick = function() {
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
        leftbutton.onclick = this.onclick;
        leftbutton.style.zIndex = "0";

        let middlebutton = document.createElement("BUTTON", file);;
        middlebutton.onclick = this.onclick;

        let rightbutton = document.createElement("BUTTON");
        rightbutton.onclick = this.deleteFile;
        rightbutton.innerHTML = 'x';


        let img = new Image(10, 10);
        img.src = "../assets/audio-symbol.png";
        img.style.position = 'absolute';
        img.style.left = '10px';


        div.appendChild(rightbutton);
        div.appendChild(leftbutton);
        div.appendChild(middlebutton);
        div.appendChild(img);

        this.hide();

    document.body.appendChild(div);
  }



    this.show = function() {
        for (let i = 0; i < filebuttonlist.length; i++) {
            filebuttonlist[i].style.display = "block"
        }
        div.style.display = ""
                            modlabel.style.display = "block"
                            hidden = 0;
    }
    this.hide = function() {
        for (let i = 0; i < filebuttonlist.length; i++) {
            filebuttonlist[i].style.display = "none"
        }
        div.style.display = "none"
                            modlabel.style.display = "none"
                            hidden = 1;
    }
    this.windowresize = function() {
        for (let i = 0; i < filebuttonlist.length; i++) {
            filebuttonlist[i].windowResize();
        }
        modlabel.style.right = sbarwidth-141 + "px";
        div.style.height = (window.innerHeight-70).toString()+'px'
                           div.style.width = sbarwidth-30 +'px'
                                             div.scrollTop = div.scrollHeight+200;



    }

    this.update();
    this.hide()

}

function haliteOptions() {
    let modlabel = document.createElement("div");
    modlabel.innerHTML = 'Halite Settings:';
    document.body.appendChild(modlabel);
    modlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; font-weight: bold; position:fixed; right:x0px; top:53%".replace('x0', sbarwidth-201);

    let srenginelabel = document.createElement("div");
    srenginelabel.innerHTML = 'Engine sample rate:';
    srenginelabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:60.5%".replace('x0', sbarwidth-51);
    document.body.appendChild(srenginelabel);

    let samplerates = [44100, 22050, 32000, 48000, 88200, 96000];
    let srenginesel = document.createElement("SELECT");
    srenginesel.style.cssText = "font-size:12px; position:fixed; right:11px; top:60%";
    srenginesel.style.width = "100px"
    srenginesel.style.height = "20px"
    document.body.appendChild(srenginesel);


    let sroutputlabel = document.createElement("div");
    sroutputlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:68.5%".replace('x0', sbarwidth/2);
    sroutputlabel.innerHTML =  'Export sample rate:';
    document.body.appendChild(sroutputlabel);

    let sroutputsel = document.createElement("SELECT");
    sroutputsel.style.cssText = "font-size:12px; position:fixed; right:11px; top:68%";
    sroutputsel.style.width = "100px"
    sroutputsel.style.height = "20px"
    document.body.appendChild(sroutputsel);

    for (let i = 0; i < samplerates.length; i++) {
      let opt = document.createElement('option');
      opt.appendChild( document.createTextNode(samplerates[i]));
      opt.value = samplerates[i];

      let opt2 = document.createElement('option');
      opt2.appendChild( document.createTextNode(samplerates[i]));
      opt2.value = samplerates[i];
      srenginesel.appendChild(opt);
      sroutputsel.appendChild(opt2);
    }

    let depths = [24, 8, 16, 32];

    let depthlabel = document.createElement("div");
    depthlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:76.5%".replace('x0', sbarwidth-251);
    depthlabel.innerHTML = 'Export bit depth:';
    document.body.appendChild(depthlabel);

    let depthsel = document.createElement("SELECT");
    depthsel.style.cssText = "font-size:12px; position:fixed; right:11px; top:76%";
    depthsel.style.width = "100px"
    depthsel.style.height = "20px"
    document.body.appendChild(depthsel);

    for (let i = 0; i < depths.length; i++) {
      let opt = document.createElement('option');
      opt.appendChild( document.createTextNode(depths[i]));
      opt.value = depths[i];
      depthsel.appendChild(opt);
    }

    let formats = ['WAV', 'AIFF'];

    let formatlabel = document.createElement("div");
    formatlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:84.5%".replace('x0', sbarwidth-251);
    formatlabel.innerHTML = 'Export format:';
    document.body.appendChild(formatlabel);

    let formatsel = document.createElement("SELECT");
    formatsel.style.cssText = "font-size:12px; position:fixed; right:11px; top:84%";
    formatsel.style.width = "100px";
    formatsel.style.height = "20px";
    document.body.appendChild(formatsel);

    for (let i = 0; i < formats.length; i++) {
      let opt = document.createElement('option');
      opt.appendChild( document.createTextNode(formats[i]));
      opt.value = formats[i];
      formatsel.appendChild(opt);
    }


    this.update = function() {
        halsettings[0] = srenginesel.value
                         halsettings[1] = sroutputsel.value
                                          halsettings[2] = depthsel.value
                                                  halsettings[3] = formatsel.value
    }

    depthsel.onchange = this.update;
    formatsel.onchange = this.update;
    sroutputsel.onchange = this.update;
    srenginesel.onchange = this.update;


    this.windowResize = function() {



        modlabel.style.right = sbarwidth-155 + "px";
        srenginesel.style.width = sbarwidth-200 + "px";
        srenginelabel.style.right = sbarwidth-180+ "px";
        sroutputsel.style.width = sbarwidth-200 + "px";
        sroutputlabel.style.right = sbarwidth-180 + "px";
        depthsel.style.width = sbarwidth-200 + "px";
        depthlabel.style.right = sbarwidth-180 + "px";
        formatsel.style.width = sbarwidth-200 + "px";
        formatlabel.style.right = sbarwidth-180 + "px";

    }

    this.show = function()  {
        modlabel.style.display = "block"
        srenginesel.style.display = "block"
        srenginelabel.style.display = "block"
        sroutputsel.style.display = "block"
        sroutputlabel.style.display = "block"
        depthsel.style.display = "block"
        depthlabel.style.display = "block"
        formatsel.style.display = "block"
        formatlabel.style.display = "block"
    }

    this.hide = function()  {
        modlabel.style.display = "none"
        srenginesel.style.display = "none"
        srenginelabel.style.display = "none"
        sroutputsel.style.display = "none"
        sroutputlabel.style.display = "none"
        depthsel.style.display = "none"
        depthlabel.style.display = "none"
        formatsel.style.display = "none"
        formatlabel.style.display = "none"
    }

    this.hide();


}

function codeBox() {

    let modlabel = document.createElement("div");
    modlabel.innerHTML = 'Code editor:';
    modlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; font-weight: bold; position:fixed; right:x0px; top:45px".replace('x0', sbarwidth-131);
    document.body.appendChild(modlabel);

    let codebox = document.createElement('textarea', code);
    codebox.style.position = "fixed";
    codebox.style.width = sbarwidth-10 + "px";
    codebox.style.height =  window.innerHeight-220 + "px";
    codebox.style.top = "80px";
    codebox.style.right = "10px";
    codebox.style.resize = "none";
    codebox.style.display = "none";
    document.body.appendChild(codebox);

    let coderefresh = document.createElement("BUTTON");
    coderefresh.style.cssText ="border:none; outline:none; font-size:12px; color:white; background-color:#303030; position:fixed; width:60px; right:x0px; top:90%".replace('x0', (sbarwidth/2)-50);
    coderefresh.style.width = "52px";
    coderefresh.style.height = "32px";
    coderefresh.innerHTML = 'Update code'
    coderefresh.addEventListener("click", () => {
        precompile(0);
    });
    document.body.appendChild(coderefresh);

    let coderun = document.createElement("BUTTON");
    coderun.style.cssText = "border:none; outline:none; font-size:12px; color:white; background-color:#303030; position:fixed; width:60px; right:x0px; top:90%".replace('x0', (sbarwidth/2)+0);
    coderun.style.width =  "52px";
    coderun.style.height = "32px";
    coderun.innerHTML = 'Run realtime';
    coderun.addEventListener("click", () => {
        fs.writeFileSync("./precompile.hcl", codebox.value);
        startHalite(1);
    });
    document.body.appendChild(coderun);

    let rendercode = document.createElement("BUTTON");
    rendercode.style.cssText ="border:none; outline:none; font-size:12px; color:white; background-color:#303030; position:fixed; width:60px; right:x0px; top:90%".replace('x0', (sbarwidth/2)+50);
    rendercode.style.width = "52px"
    rendercode.style.height = "32px";
      rendercode.innerHTML = 'Render to file';
    rendercode.addEventListener("click", () => {
        fs.writeFileSync("./precompile.hcl", codebox.value);
        startHalite(0);
    });
    document.body.appendChild(rendercode);

    this.value = function(val) {
        codebox.innerHTML = val;
    }

    this.windowResize = function() {
        codebox.style.width = sbarwidth-70 + "px";
        codebox.style.height = window.innerHeight-220 + "px";;
        rendercode.style.right = ((sbarwidth-100)/2)+70 + "px"
        coderun.style.right = ((sbarwidth-100)/2)+0 + "px";
        coderefresh.style.right = ((sbarwidth-100)/2)-70 + "px";
        modlabel.style.right = sbarwidth-130 + "px";
    }

    this.show = function()  {
        codebox.style.display = "block";
        rendercode.style.display = "block";
        coderun.style.display = "block";
        modlabel.style.display = "block"
        coderefresh.style.display = "block";
    }


    this.hide = function()  {
        codebox.style.display = "none";
        rendercode.style.display = "none";
        coderun.style.display = "none";
        coderefresh.style.display = "none";
        modlabel.style.display = "none"
    }

    this.hide();
}
