sbarwidth = 350;






let tab = 1;

function Sidebar() {
  let hidden = 0;
  let _this = this;

  // draggable bit
  let bar = document.createElement("div");
  bar.style.position = "fixed";
  bar.style.width = "30px";
  bar.style.height = window.innerHeight + "px";
  bar.style.background = "#333";
  bar.style.top = "0px";
  bar.style.right = sbarwidth - 30 + "px";
  bar.style.zIndex = "0";

  // background
  let bg = document.createElement("div");
  bg.style.position = "fixed";
  bg.style.background = "#444";
  bg.style.top = "0px";
  bg.style.right = "0px";
  bg.style.height = window.innerHeight + "px";
  bg.style.width = sbarwidth + "px";
  bg.style.zIndex = "-1";

  document.body.appendChild(bg);
  document.body.appendChild(bar);


  // Make the bar draggable
  dragElement(bar, this);

  // label for the console
  let modlabel = document.createElement("div");
  modlabel.innerHTML = 'Console:';
  modlabel.hide = () => {
    modlabel.style.display = "none"
  }
  modlabel.show = () => {
    modlabel.style.display = "block"
  }
  modlabel.style.position = "fixed";
  modlabel.style.top = "45px";
  modlabel.style.right = sbarwidth - 105 + "px";
  modlabel.style.fontSize = "14px";
  modlabel.style.color = "#dcdcdc";
  modlabel.style.fontFamily = "sans-serif";
  modlabel.style.fontWeight = "bold";
  document.body.appendChild(modlabel);


  //Console logviewer object created in console-viewer.js!
  logviewer.windowResize(sbarwidth - 60, window.innerHeight - 200);

  // Initialize all sidebar components (except the console)
  let audiosettings = new audioSettings();

  let medialib = new mediaLibrary();

  let codebox = new codeBox();



  // Function to open a tab and hide the rest
  function showTab(tab) {
    // Show/hide
    if (!tab && !hidden) {
      buttons[0].textContent = "\xE7"; // symbols for our icon font
      sbarwidth = 38;
      hidden = 1;
      _this.windowresize();

    } else {
      buttons[0].textContent = "\xE5";
      if (!tab) {
        tab = 2;
      }
      sbarwidth = 350;
      hidden = 0;
      _this.windowresize();
    }

    let tabs = [
      [],
      [audiosettings],
      [logviewer, modlabel],
      [medialib],
      [codebox],
    ];
    for (var i = 0; i < 5; i++) {
      if (i != tab) {
        tabs[i].forEach((t) => t.hide());
      }
    }
    if (tab != 0) {
      tabs[tab].forEach((t) => t.show());
    }
  }

  let buttonpresets = [
    ['\xE5', '35px', function() {
      showTab(0)
    }],
    ['\xE1', '44%', function() {
      showTab(1)
    }],
    ['i', '50%', function() {
      showTab(2)
    }],
    ['&', '56%', function() {
      showTab(4)
    }],
    ['r', '62%', function() {
      showTab(3)
    }],
  ];
  let buttons = [];

  // Set up all the buttons to switch pages
  for (let i = 0; i < buttonpresets.length; i++) {
    buttons[i] = document.createElement("BUTTON");
    buttons[i].innerHTML = buttonpresets[i][0];
    buttons[i].style.cssText = "border:none; outline:none; font-size:24px; text-align:center; color:white; background-color:#202020; position:absolute; right:0px; top:x1;".replace('x0', sbarwidth - 38).replace('x1', buttonpresets[i][1]);
    buttons[i].style.fontFamily = "Entypo";
    buttons[i].style.width = "30px";
    buttons[i].style.height = "30px";
    buttons[i].addEventListener("click", buttonpresets[i][2]);
    bar.appendChild(buttons[i])
  }

// Windowresize is called whenever the sidebar or window changes size
  this.windowresize = function() {
    bg.style.width = sbarwidth + "px";
    bar.style.right = sbarwidth - 30 + "px";

    bg.style.height = window.innerHeight + "px";
    bar.style.height = window.innerHeight + "px";


    for (let i = 0; i < buttons.length; i++) {
      buttons[i].style.width = "30px";
      buttons[i].style.height = "30px";
    }

    audiosettings.windowResize();
    medialib.windowresize();
    codebox.windowResize();
    modlabel.style.right = sbarwidth - 105 + "px";


    logviewer.windowResize(sbarwidth - 60, window.innerHeight - 50);
    if (hidden) {
      bg.style.width = +"0px";
      bar.style.right = "8px";
    }
  }

  // Update the codebox' code
  this.codeUpdate = function(ncode) {
    return codebox.value(ncode);
  }
  // Update our media library
  this.libraryUpdate = function() {
    medialib.update();
  }

}

// Media library makes it easier to load in audio files, without having to type a long url
function mediaLibrary() {
  let filebuttonlist = []; // list of all our items
  let hidden = 1;
  let __this = this;

  let modlabel = document.createElement("div");
  modlabel.innerHTML = 'Media Library:';
  modlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; font-weight: bold; position:fixed; right:x0px; top:45px".replace('x0', sbarwidth - 151);
  document.body.appendChild(modlabel);

  let div = document.createElement('div');
  div.style.position = "fixed";
  div.style.font = "sans-serif";
  div.style.right = "0px";
  div.style.top = "75px";
  div.style.height = (window.innerHeight - 70) + 'px'
  div.style.width = sbarwidth - 30 + 'px'
  div.style.overflow = "auto";
  div.style.background = "#444";



  // add to the list
  this.addItem = function(err, files) {
    if (files != undefined) {
      for (let i = 0; i < files.length; i++) {
        filebuttonlist[i] = new mediaButton(files[i])
        filebuttonlist[i].style()
      }
    }
  }
  // update
  this.update = function() {
    filebuttonlist = [];
    // Get all the files in our media folder
    fs.readdir(home + "/Media", this.addItem);
    if (!hidden) {
      setTimeout(function() {
        __this.hide();
        __this.show();

      }, 100);
    }

  }

  // Class for each individual button
  function mediaButton(file) {
    let _this = this;

    this.style = function() {
      let top = filebuttonlist.indexOf(_this)

      // nice picture to indicate this is audio
      img.style.top = (11 + (32 * top)) + "px";
      img.style.right = "50px"
      middlebutton.style.cssText = "border:none; outline:none; font-size:14px; text-align:left; color:white; background-color:x3; position:absolute; left:32px; top:x2px; width:x1px; height:32px;".replace('x0', 30).replace('x1', sbarwidth - 95).replace('x2', top * 32).replace('x3', ['#303030', '#383838'][top % 2]);
      middlebutton.innerHTML = file;
      leftbutton.style.cssText = "border:none; outline:none; font-size:14px; text-align:left; color:white; background-color:x3; position:absolute; left:0px; top:x2px; width:32px; height:32px;".replace('x1', 30).replace('x2', top * 32).replace('x3', ['#303030', '#383838'][top % 2]);
      rightbutton.style.cssText = "border:none; outline:none; font-size:14px; text-align:center; color:white; background-color:x3; position:absolute; right:0px; top:x2px; width:x1px; height:32px;".replace('x1', 39).replace('x2', top * 32).replace('x3', ['#303030', '#383838'][top % 2]);
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
      middlebutton.style.width = sbarwidth - 100 + "px";


    };
    // Create a new input when clicked
    this.onclick = function() {
      boxes.push(new Component('input file 0.2'.replace('file', "./Media/" + file), window.innerWidth / 2, window.innerHeight / 2 - 250));
    };

    // Delete a file from our media folder when the user clicks X
    this.deleteFile = function() {
      fs.unlinkSync(home + "/Media/" + file, function(err) {
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
    img.src = __dirname + "/../assets/audio-symbol.png";
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
    modlabel.style.right = sbarwidth - 141 + "px";
    div.style.height = (window.innerHeight - 70).toString() + 'px'
    div.style.width = sbarwidth - 30 + 'px'
    div.scrollTop = div.scrollHeight + 200;



  }

  this.update();
  this.hide()

}

// Page to set our audio settings for realtime and non-realtime playback
// This is mostly just making lots of selects and styling them
function audioSettings() {
  let modlabel = document.createElement("div");
  modlabel.innerHTML = 'Audio Settings:';
  document.body.appendChild(modlabel);
  modlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; font-weight: bold; position:fixed; right:x0px; top:7%".replace('x0', sbarwidth - 201);


  let buflabel = document.createElement("div");
  buflabel.innerHTML = 'Buffer size:';
  buflabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:13%".replace('x0', sbarwidth - 135);
  document.body.appendChild(buflabel);

  let bufsizes = [4096, 256, 512, 1024, 2048];
  let buffersel = document.createElement("SELECT");
  buffersel.style.cssText = "font-size:12px; position:fixed; right:11px; top:13%";
  buffersel.style.width = "100px"
  buffersel.style.height = "20px"
  document.body.appendChild(buffersel);

  for (let i = 0; i < bufsizes.length; i++) {
    let opt = document.createElement('option');
    opt.appendChild(document.createTextNode(bufsizes[i]));
    opt.value = bufsizes[i];
    buffersel.appendChild(opt);
  }

  let srenginelabel = document.createElement("div");
  srenginelabel.innerHTML = 'Engine sample rate:';
  srenginelabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:20%".replace('x0', sbarwidth - 51);
  document.body.appendChild(srenginelabel);

  let samplerates = [44100, 22050, 32000, 48000, 88200, 96000];
  let srenginesel = document.createElement("SELECT");
  srenginesel.style.cssText = "font-size:12px; position:fixed; right:11px; top:20%";
  srenginesel.style.width = "100px";
  srenginesel.style.height = "20px";
  document.body.appendChild(srenginesel);


  let sroutputlabel = document.createElement("div");
  sroutputlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:27%".replace('x0', sbarwidth / 2);
  sroutputlabel.innerHTML = 'Export sample rate:';
  document.body.appendChild(sroutputlabel);

  let sroutputsel = document.createElement("SELECT");
  sroutputsel.style.cssText = "font-size:12px; position:fixed; right:11px; top:27%";
  sroutputsel.style.width = "100px";
  sroutputsel.style.height = "20px";
  document.body.appendChild(sroutputsel);

  for (let i = 0; i < samplerates.length; i++) {
    let opt = document.createElement('option');
    opt.appendChild(document.createTextNode(samplerates[i]));
    opt.value = samplerates[i];

    let opt2 = document.createElement('option');
    opt2.appendChild(document.createTextNode(samplerates[i]));
    opt2.value = samplerates[i];
    srenginesel.appendChild(opt);
    sroutputsel.appendChild(opt2);
  }

  let depths = [24, 8, 16, 32];

  let depthlabel = document.createElement("div");
  depthlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:34%".replace('x0', sbarwidth - 251);
  depthlabel.innerHTML = 'Export bit depth:';
  document.body.appendChild(depthlabel);

  let depthsel = document.createElement("SELECT");
  depthsel.style.cssText = "font-size:12px; position:fixed; right:11px; top:34%";
  depthsel.style.width = "100px"
  depthsel.style.height = "20px"
  document.body.appendChild(depthsel);

  for (let i = 0; i < depths.length; i++) {
    let opt = document.createElement('option');
    opt.appendChild(document.createTextNode(depths[i]));
    opt.value = depths[i];
    depthsel.appendChild(opt);
  }

  let formats = ['.WAV', '.AIFF'];

  let formatlabel = document.createElement("div");
  formatlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; position:fixed; right:x0px; top:41%".replace('x0', sbarwidth - 251);
  formatlabel.innerHTML = 'Export format:';
  document.body.appendChild(formatlabel);

  let formatsel = document.createElement("SELECT");
  formatsel.style.cssText = "font-size:12px; position:fixed; right:11px; top:41%";
  formatsel.style.width = "100px";
  formatsel.style.height = "20px";
  document.body.appendChild(formatsel);

  for (let i = 0; i < formats.length; i++) {
    let opt = document.createElement('option');
    opt.appendChild(document.createTextNode(formats[i]));
    opt.value = formats[i];
    formatsel.appendChild(opt);
  }


  this.update = function() {
    halsettings[0] = srenginesel.value
    halsettings[1] = sroutputsel.value
    halsettings[2] = depthsel.value
    halsettings[3] = formatsel.value
    halsettings[4] = buffersel.value
  }
  buffersel.onchange = this.update;
  depthsel.onchange = this.update;
  formatsel.onchange = this.update;
  sroutputsel.onchange = this.update;
  srenginesel.onchange = this.update;


  this.windowResize = function() {
    modlabel.style.right = sbarwidth - 155 + "px";
    buffersel.style.width = sbarwidth - 200 + "px";
    buflabel.style.right = sbarwidth - 180 + "px";
    srenginesel.style.width = sbarwidth - 200 + "px";
    srenginelabel.style.right = sbarwidth - 180 + "px";
    sroutputsel.style.width = sbarwidth - 200 + "px";
    sroutputlabel.style.right = sbarwidth - 180 + "px";
    depthsel.style.width = sbarwidth - 200 + "px";
    depthlabel.style.right = sbarwidth - 180 + "px";
    formatsel.style.width = sbarwidth - 200 + "px";
    formatlabel.style.right = sbarwidth - 180 + "px";

  }

  this.show = function() {
    modlabel.style.display = "block"
    buffersel.style.display = "block"
    buflabel.style.display = "block"
    srenginesel.style.display = "block"
    srenginelabel.style.display = "block"
    sroutputsel.style.display = "block"
    sroutputlabel.style.display = "block"
    depthsel.style.display = "block"
    depthlabel.style.display = "block"
    formatsel.style.display = "block"
    formatlabel.style.display = "block"
  }

  this.hide = function() {
    modlabel.style.display = "none"
    buffersel.style.display = "none"
    buflabel.style.display = "none"
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


// Big text box that shows the code that is being sent to the C++ program
// Really useful for debugging!
function codeBox() {

  let modlabel = document.createElement("div");
  modlabel.innerHTML = 'Code editor:';
  modlabel.style.cssText = "color:#dcdcdc; font-size:14px; font-family:sans-serif; font-weight: bold; position:fixed; right:x0px; top:45px".replace('x0', sbarwidth - 131);
  document.body.appendChild(modlabel);

  let codebox = document.createElement('textarea', code);
  codebox.style.position = "fixed";
  codebox.style.width = sbarwidth - 10 + "px";
  codebox.style.height = window.innerHeight - 220 + "px";
  codebox.style.top = "80px";
  codebox.style.right = "10px";
  codebox.style.resize = "none";
  codebox.style.display = "none";
  document.body.appendChild(codebox);

  let coderefresh = document.createElement("BUTTON");
  coderefresh.style.cssText = "border:none; outline:none; font-size:12px; color:white; background-color:#303030; position:fixed; width:60px; right:x0px; top:90%".replace('x0', (sbarwidth / 2) - 50);
  coderefresh.style.width = "52px";
  coderefresh.style.height = "32px";
  coderefresh.innerHTML = 'Update code'
  coderefresh.addEventListener("click", () => {
    precompile(0);
  });
  document.body.appendChild(coderefresh);

  // Run the code in the box realtime
  let coderun = document.createElement("BUTTON");
  coderun.style.cssText = "border:none; outline:none; font-size:12px; color:white; background-color:#303030; position:fixed; width:60px; right:x0px; top:90%".replace('x0', (sbarwidth / 2) + 0);
  coderun.style.width = "52px";
  coderun.style.height = "32px";
  coderun.innerHTML = 'Run realtime';
  coderun.addEventListener("click", () => {
    fs.writeFileSync("./precompile.hcl", codebox.value);
    startHalite(1);
  });
  document.body.appendChild(coderun);

  // Run the code in the box and export audio to file
  let rendercode = document.createElement("BUTTON");
  rendercode.style.cssText = "border:none; outline:none; font-size:12px; color:white; background-color:#303030; position:fixed; width:60px; right:x0px; top:90%".replace('x0', (sbarwidth / 2) + 50);
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
    codebox.style.width = sbarwidth - 70 + "px";
    codebox.style.height = window.innerHeight - 220 + "px";;
    rendercode.style.right = ((sbarwidth - 100) / 2) + 70 + "px"
    coderun.style.right = ((sbarwidth - 100) / 2) + 0 + "px";
    coderefresh.style.right = ((sbarwidth - 100) / 2) - 70 + "px";
    modlabel.style.right = sbarwidth - 130 + "px";
  }

  this.show = function() {
    codebox.style.display = "block";
    rendercode.style.display = "block";
    coderun.style.display = "block";
    modlabel.style.display = "block"
    coderefresh.style.display = "block";
  }


  this.hide = function() {
    codebox.style.display = "none";
    rendercode.style.display = "none";
    coderun.style.display = "none";
    coderefresh.style.display = "none";
    modlabel.style.display = "none"
  }

  this.hide();
}

function singleSlider(divComponent) {


  var slider = document.createElement("INPUT");
  slider.type = "range";
  slider.min = 0;
  slider.max = 1;
  slider.step = 0.001;
  slider.style.position = "relative";
  slider.style.width = 155 + "px";
  slider.style.height = "20px"
  slider.style.bottom = "5px";
  slider.style.right = "0px";
  slider.style.resize = "none";
  slider.style.display = "none";
  slider.style.zIndex = "1";

  divComponent.style.width = 185 + "px";
  divComponent.appendChild(slider);


  slider.addEventListener('mousedown',function (event){
   event.stopPropagation();
});


  let valuelabel = document.createElement("div");
  valuelabel.style.cssText = "color:#dcdcdc; font-size:13px; font-family:sans-serif; position:relative; bottom:24px; left:163px".replace('x0', 20);
  valuelabel.innerHTML = parseFloat(slider.value).toFixed(2);
  divComponent.appendChild(valuelabel);

  slider.style.display = "block";
  valuelabel.style.display = "block";


slider.update = function() {
    divComponent.setargs(["/slider" + sliders.indexOf(divComponent), JSON.stringify(sliders.indexOf(divComponent))]);
}

divComponent.update = function(a) {
  slider.update();
}

  slider.oninput = function() {
      valuelabel.innerHTML = parseFloat(slider.value).toFixed(2);
    client.send("/slider"+sliders.indexOf(divComponent), parseFloat(slider.value), (err) => {
      if (err) console.log(err);
    });
    slider.update();
  }


  this.delete = function() {
      sliders.splice(sliders.indexOf(divComponent), 1);
      for (var i = 0; i < sliders.length; i++) {
        sliders[i].update();
      }

      // undo slider specific modifications we made to the component
      divComponent.setargs([]);
      divComponent.style.width = "";
      divComponent.style.paddingTop = "3px";
      divComponent.style.paddingBottom = "1px";
      divComponent.style.paddingRight = "12px";
      divComponent.update = undefined;

      slider.remove();
      valuelabel.remove();

  }
  slider.update();
}
