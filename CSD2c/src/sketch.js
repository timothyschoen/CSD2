let dialog = require('electron').remote.dialog;
let fs = require("fs");
const { spawn } = require('child_process');

// All our objects, plus the data that we need to convert it to halite
let types = {
  // Halite standard components
  'resistor': {'inlets': 1, 'outlets': 1, 'colors': ['#229FD7', '#229FD7'], 'args': 1, 'code': " resistor, a0, i0, i1"},
  'ground': {'inlets': 1, 'outlets': 0, 'args': 0, 'colors': ['#000000'], 'code': ""},
  'voltage': {'inlets': 0, 'outlets': 2, 'args': 1, 'colors': ['#ff0000', '#000000'], 'code': " voltage, a0, i0, i1"},
  'capacitor': {'inlets': 1, 'outlets': 1, 'args': 1, 'colors': ['#229FD7', '#229FD7'], 'code': " capacitor, a0, i0, i1"},
  'diode': {'inlets': 1, 'outlets': 1, 'args': 0, 'colors': ['#ff0000', '#229FD7'], 'code': " diode, i0, i1"},
  'bjt': {'inlets': 1, 'outlets': 2, 'args': 1, 'colors': ['#ff0000', '#000000', '#229FD7'], 'code': " bjt, i0, i1, i2, a0"}, // color coding!!!
  'op-amp': {'inlets': 4, 'outlets': 1, 'args': 0, 'colors': ['#229FD7', '#000000', '#ff0000', '#000000', '#229FD7'], 'code': " opa, i4, i0, i1, i2, i3"},

  // Our analog Components
  'varres': {'inlets': 2, 'outlets': 1, 'datatypes': ['analog', 'digital', 'analog'], 'colors': ['#229FD7', '#229FD7'], 'args': 1, 'code': " varres, a0, i0, i1, d1"},
  'pot': {'inlets': 2, 'outlets': 2, 'datatypes': ['analog', 'digital', 'analog', 'analog'], 'args': 1, 'colors': ['#ff0000', '#229FD7', '#000000', '#229FD7'], 'code': " potentiometer, a0, i0, i2, i3, d1"},
  'cycle': {'inlets': 0, 'outlets': 2, 'args': 1, 'colors': ['#ff0000', '#000000'], 'code': " cycle, a0, i0, i1"},
  'input': {'inlets': 0, 'outlets': 2, 'args': 2, 'colors': ['#ff0000', '#000000'], 'code': " input, a0, a1, i0, i1"},
  'output': {'inlets': 2, 'outlets': 0, 'args': 1, 'colors': ['#229FD7'], 'code': " probe, i0, i1, a0"},
  'print': {'inlets': 1, 'outlets': 1, 'colors': ['#229FD7', '#229FD7'], 'args': 0, 'code': " print, i0, i1"},
  'delay': {'inlets': 2, 'outlets': 1, 'datatypes': ['analog', 'digital', 'analog'], 'colors': ['#229FD7', '#229FD7'], 'args': 2, 'code': " delay, a0, a1, i0, d1, i2"},

  // conversion objects
  'dac': {'inlets': 1, 'outlets': 2, 'datatypes': ['digital', 'analog', 'analog'], 'colors': ['#229FD7', '#229FD7'], 'args': 0, 'code': " dac, d0, i1, i2"},
  'adc': {'inlets': 2, 'outlets': 1, 'datatypes': ['analog', 'analog', 'digital'], 'colors': ['#229FD7', '#229FD7'], 'args': 0, 'code': " adc, i0, i1, d2"},

  // digital objects
  // I/O
  'input-': {'inlets': 0, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 2, 'colors': ['#ff0000', '#000000'], 'code': " input-, a0, a1, d0"},
  'stinput-': {'inlets': 0, 'outlets': 2, 'datatypes': ['digital', 'digital'], 'args': 2, 'colors': ['#ff0000', '#000000'], 'code': " stinput-, a0, a1, d0, d1"},
  'rtinput-': {'inlets': 0, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 1, 'colors': ['#ff0000', '#000000'], 'code': " rtinput-, a0, d0"},
  'output-': {'inlets': 2, 'outlets': 0, 'datatypes': ['digital', 'digital'], 'args': 1, 'colors': ['#229FD7'], 'code': " output-, a0 , d0, d1"},
  'dcblock-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " dcblock-, d0, d1"},

  'accum-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " accum-, d0, d1"},
  'change-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " change-, d0, d1"},
  'delta-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " delta-, d0, d1"},
  'and-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " and-, d0, d1"},
  '&&-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " &&-, d0, d1"},
  'or-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " or-, d0, d1"},
  '||-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " ||-, d0, d1"},
  'abs-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " abs-, d0, d1"},
  'ceil-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " ceil-, d0, d1"},
  'floor-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " floor-, d0, d1"},


  'elapsed-': {'inlets': 0, 'outlets': 1, 'datatypes': ['digital'], 'args': 0, 'colors': ['#229FD7'], 'code': " elapsed-, d0"},


  'midiin-': {'inlets': 0, 'outlets': 2, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7', '#229FD7'], 'code': " midiin-, d0, d1"},
  'notein-': {'inlets': 0, 'outlets': 2, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7', '#229FD7'], 'code': " notein-, d0, d1"},
  'mtof-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'args': 0, 'colors': ['#229FD7', '#229FD7'], 'code': " mtof-, d0, d1"},

  // Signal generation
  'cycle-': {'inlets': 1, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'colors': ['#229FD7', '#229FD7'], 'args': 0, 'code': " cycle-, d0, d1"},
  'sig-': {'inlets': 0, 'outlets': 1, 'datatypes': ['digital', 'digital'], 'colors': ['#229FD7', '#229FD7'], 'args': 1, 'code': " sig-, a0, d0"},

  // Delay
  'delay-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " digidelay, d0, d1, d2"},

  // arithmetics without arg
  '+-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " +-, d0, d1, d2"},
  '--': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " --, d0, d1, d2"},
  '!--': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " !--, d0, d1, d2"},
  '*-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " *-, d0, d1, d2"},
  '/-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " /-, d0, d1, d2"},
  '!/-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " !/-, d0, d1, d2"},
  '>-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " >-, d0, d1, d2"},
  '<-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " <-, d0, d1, d2"},
  '>=-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " >=-, d0, d1, d2"},
  '<=-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " <=-, d0, d1, d2"},
  '==-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " ==-, d0, d1, d2"},
  '!=-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " !=-, d0, d1, d2"},
  '%-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " %, d0, d1, d2"},
  '!%-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " !%, d0, d1, d2"},

  // Components from Gen
  'phasor-': {'inlets': 2, 'outlets': 1, 'datatypes': ['digital', 'digital', 'digital'], 'colors': ['#229FD7', '#229FD7', '#229FD7'], 'args': 0, 'code': " phasor, d0, d1, d2"},

}

let preset = ["ground", "output 0.3", 'input "./samples/sample-44k.wav" 0.2'];


let boxesprox = []; // Where we store our components
let connectionsprox = []; // Where we store our connections
let connecting = -1; // Is any inlet currently in the connecting state?



var arrayChangeHandler = {
  get: function(target, property) {
  // property is index in this case
  return target[property];
},
  set: function(target, property, value, receiver) {
    target[property] = value;
    if (property == 'length' && !initializing) {
      // disabeled for now because it's too glitchy
     //changed();

   }
    // you have to return true to accept the changes
    return true;
  }
};

var connections = new Proxy( connectionsprox, arrayChangeHandler );
var boxes = new Proxy( boxesprox, arrayChangeHandler );

let halite; // letiable for halite process

let realtime_playing = false; // Checks if we are playing in realtime

let sbar; // Sidebar (see sidebar.js)
let sbarwidth = 350;

let cnvwidth; // Width of our canvas without the sidebar

let code = ''; // letiable to store code to use for exporting and displaying

let draginstance = -1; // Is any component being dragged? -1 means no, otherwise it contains the index number

let selectX, selectY, selwidth, selheight; // X, Y, W, H of our selection

let selecting = false; // Are we selecting?

let halsettings = [44100, 44100, 24, 'WAV']; // Export and audio settings for halite (CURRENTLY UNUSED!!)

let jackstatus = 0; // Is jack running?

let jack; // letiable for jack process

let typing = false; // Check if we're typing, if so, disable keyboard shortcuts

let initializing = true;

let history = [];
let histpos = 0;


// Close background processes when quitting or reloading
// DOESN'T ALWAYS WORK!!
window.onbeforeunload = function(){
  generatesave(1, './lastsession.ncl');
  try {
  if (realtime_playing) {
    halite.kill('SIGINT');
  }
  if (jackstatus) {
    jack.kill('SIGINT')
  }
  }
  catch (e) {}
};

process.on('SIGINT', window.onbeforeunload); // catch ctrl-c
process.on('SIGTERM', window.onbeforeunload); // catch kill

function preload() {
  //Fix for the indented closebutton on macOs, needed to make window draggable
  let windowTopBar = document.createElement('div');
  windowTopBar.style.width = "100%";
  windowTopBar.style.height = "20px";
  windowTopBar.style.position = "fixed";
  windowTopBar.style.top = windowTopBar.style.left = 0;
  windowTopBar.style.webkitAppRegion = "drag";
  document.body.appendChild(windowTopBar);
}

// Dragging and dropping files
document.ondragover = document.ondrop = (ev) => {
  ev.preventDefault()
}

document.body.ondrop = (ev) => {
  // If we get a new file, move it to our media library
  fs.createReadStream(ev.dataTransfer.files[0].path).pipe(fs.createWriteStream('./media/' + ev.dataTransfer.files[0].name));
  ev.preventDefault()
  // Update the files in the medialibrary
  sbar.libraryUpdate();
}

// All our control buttons in an array
let buttonpresets = [['+', 100, function () {boxes.push(new Component(undefined, 500, 500))}], ['R', 0, function () {startHalite(1)}],
                    ['S', -100, function () {startHalite(0)}], ['N', 100, function () { localStorage.setItem("load", 'new'); window.onbeforeunload(); location.reload(true);}],
                    ['O', 150, loadfile], ['S', 200, function () { generatesave(1)} ], ['R', 250, function () { generatesave(1, './lastsession.ncl'); loadfile('./lastsession.ncl');}]];
let buttons = [];


function setup() {
  // Limit framerate for improved performance (maybe set is lower though?)
  frameRate(50);

  createCanvas(windowWidth, windowHeight);

  // Initialize the sidebar (from sidebar.js)
  sbar = new Sidebar;

  // Set the canvas size
  cnvwidth = windowWidth-sbarwidth;

  // Loads our last session from local storage
  // This should make sure that refreshing or closing the app (or a potential crash) won't mean our patch is lost!
  var state = localStorage.getItem("load");
  if(state == undefined || state == 'new') {
    for(let i = 0; i<3; i++) {
      boxes[i] = new Component(preset[i], (windowWidth/2)+((i-1)*100), windowHeight/2-250+((3-i)*100));
    }
  }
  else {
    openSavedFile(state);
  }
  initializing = false;
  changed();

  // set up main buttons at the bottom
  // I had to set up so many buttons throughout this program that using loops saves me hundreds of lines
  for (let i = 0; i < 3; i++) {
    buttons[i] = createButton(buttonpresets[i][0])
    buttons[i].size(48, 48);
    buttons[i].style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#229FD7; position:fixed;  bottom:60px;  left:x0px;".replace("x0", (cnvwidth/2)+buttonpresets[i][1]));
    buttons[i].mousePressed(buttonpresets[i][2]);
  }
  // Buttons at the top
  for (let i = 3; i < buttonpresets.length; i++) {
    buttons[i] = createButton(buttonpresets[i][0])
    buttons[i].size(32, 32);
    buttons[i].style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#229FD7; position:fixed; top:10px; left:x0px; ".replace("x0", buttonpresets[i][1]));
    buttons[i].mousePressed(buttonpresets[i][2]);
  }



  }


function draw() {

  background(255,255,255);

  noStroke();

  if(width < windowWidth) {
    resizeCanvas(windowWidth, windowHeight);
  }

  sbar.draw()

  for(let i = 0; i<boxes.length; i++) {
    boxes[i].draw();
  }

// Draw each connection
  connections.forEach(conn => {
    conn.draw();
  });


  // TODO maybe make a mousedown function so we don't need to do this at every draw?
  if(mouseIsPressed && draginstance == -1 && !selecting) {
    selecting = true;
    selectX = mouseX;
    selectY = mouseY;
  }
  else if(!mouseIsPressed && selecting) {
    selecting = false;
  }

  // Making selections with a selection box
  if (selecting) {
    stroke(30);
    fill(60, 60, 60, 40);
    selwidth = mouseX-selectX;
    selheight = mouseY-selectY
    let selrect = rect(selectX, selectY, selwidth, selheight);

    for (let i = 0; i < boxes.length; i++) {
      // Get size and position of all the components
      // Might be inefficient??
      let boxpos = boxes[i].getposition();
      let boxsize = boxes[i].getsize();

      // Selecting with a selection box
      if (intersect(selectX, selectX+selwidth, selectY, selectY+selheight, boxpos[0], boxpos[0]+boxsize[0], boxpos[1], boxpos[1]+boxsize[1])) {
        boxes[i].select()
     }
      else {
        boxes[i].deselect()
     }


   }
}



  //Draw a line to the mouse while making the connection
if(connecting != -1) {
  let linex = connecting[0].getinlets()[connecting[1]].getposition()[0];
  let liney = connecting[0].getinlets()[connecting[1]].getposition()[1];
  stroke(0)
  line(linex, liney, mouseX, mouseY);
}
  sbar.draw2();
}



// Convert our line-based network into an electrical nodes network that Halite can interpret
function precompile(save = 1) {
    let iterconnections = [];
    let digitalconns = [];
    let iterboxes = [];
    let nodes = [];
    for(let i = 0; i < connections.length; i++) {
      if(connections[i].gettype() != 'digital') {
        let lets = connections[i].getinlets();
        iterconnections.push([[lets[0][0], lets[1][0]], [lets[0][1], lets[1][1]]]);
      }
      else {
        let lets = connections[i].getinlets();
        digitalconns.push([[lets[0][0], lets[1][0]], [lets[0][1], lets[1][1]]]);
      }
    }



  // First find all unique connection points
  // This is a real pain in the ass because js doesn't really allow comparing subarrays...
    for(let x = 0; x < iterconnections.length; x++) {
        for(let z = 0; z < iterconnections[x].length; z++) {
          if(!JSON.stringify(nodes).includes(JSON.stringify(iterconnections[x][z]))) {
              // fix ground to 0
            if (iterconnections[x][0][0] == 0) {
              nodes.unshift(iterconnections[x]);
            }
            else {
            nodes.push(iterconnections[x]);
          }
          }
        }
    }
    // Check if these points are connected to other points
      for(let x = 0; x < nodes.length; x++) {
          for(let y = 0; y < nodes[x].length; y++) {
            for(let z = 0; z < nodes.length; z++) {
                for(let a = 0; a < nodes[z].length; a++) {
            if(JSON.stringify(nodes[x]).includes(JSON.stringify(nodes[z][a])) && x != z) {
              nodes[x].push(nodes[z][0]);
              nodes[x].push(nodes[z][1]);
              nodes[x] = removeDuplicates(nodes[x]);
              nodes[z] = [];
              }
            }
        }
      }
    }

  // remove empty leftovers
  for(let x = nodes.length-1; x >= 0;  x--) {
      if(nodes[x].length == 0 || nodes[x] == null) {
        nodes.splice(x, 1);
      }
  }
  let digitalconnections = 0;
  let digiExists = [[0, 0]];
  for(let i = 0; i < boxes.length; i++) {
    let boxtype = boxes[i].gettype();
    let boxargs = boxes[i].getargs();
    let optargs = boxes[i].getoptargs();
    let itercode = types[boxtype]['code'];
    let amtinlets = int(types[boxtype]['inlets']) + int(types[boxtype]['outlets']);

    for(let x = 0; x < amtinlets; x++) {
      let target = JSON.stringify([i, x]);
      for(var y=0; y<nodes.length; y++) {
             if( JSON.stringify(nodes[y]).includes(target)) break;
          }
        if(y >= nodes.length) {
          if(types[boxtype]["datatypes"] != undefined &&  types[boxtype]["datatypes"][x] == 'digital') {

            for (var d = 0; d < digitalconns.length; d++) {
              if(JSON.stringify(digitalconns[d]).includes(target)) {
                // hier ingrijpen!!!
                console.log(JSON.stringify(digitalconns[d]), d , '\n');

                for (var ex = 0; ex < digiExists.length; ex++) {
                  //console.log('length: ', digiExists.length, 'index: ', ex, 'array: ', digiExists[ex] == undefined);

                  console.log();
                  if(digiExists[ex] != undefined && (JSON.stringify(digiExists[ex]).includes(JSON.stringify(digitalconns[d][0])) || JSON.stringify(digiExists[ex]).includes(JSON.stringify(digitalconns[d][1])))) {
                    digiExists[ex].push(digitalconns[d][0])
                    digiExists[ex].push(digitalconns[d][1])
                    itercode = itercode.replace("d"+x, ex+nodes.length);
                  }
                  else {
                    itercode = itercode.replace("d"+x, d+nodes.length);
                    digiExists[d] = digitalconns[d];
                  }
                }
            }

        }
      }
        itercode = itercode.replace("d"+x, 0);

          y = 0;
        }
            itercode = itercode.replace("i"+x, y)


    }
    for(let x = 0; x < boxargs.length; x++) {
    itercode = itercode.replace("a"+x, boxargs[x])
  }
    iterboxes[i] = itercode + ', ' + JSON.stringify(optargs);
  }
  let setupnodes = " ground, nodes".replace('nodes', nodes.length);
  code = setupnodes + iterboxes.join("\n");
  sbar.codeUpdate(code);

  if(save) {
  fs.writeFileSync("./precompile.ncl", code);
  }
}

function startHalite(realtime) {
  let haliteappendix;
  let halitecmd = './compiled/Halite'

  // Write our patch to a file that Halite can read
  precompile(1);

  if(realtime && !realtime_playing && sbar.getJackStatus()) {
    haliteappendix = ['-r'];
    buttons[1].style("color:red;");
  }
  else if (!realtime) {
      // If we are not running in realtime, get a path to save the wav/aif file
    let savepath = dialog.showSaveDialog({
      filters: [{
        name: "Audio",
        extensions: ['wav', 'aif']
    }]
  });
    haliteappendix = ['-o x0'.replace('x0', savepath)]; // this doesn't contain everything yet...
  }

  if(!(realtime && (realtime_playing || !sbar.getJackStatus())))  {

    halite = spawn('./compiled/Halite',  haliteappendix);

    halite.stdout.on('data', function (data) {
      console.log('Halite: ' + data);
    });

    halite.stderr.on('data', function (data) {
      console.warn('Halite Error: ' + data);
    });

    halite.on('close', function (code) {
        buttons[1].style("color:white;");
        console.log('Halite closed with code ' + code);
        realtime_playing = false;
    });
    realtime_playing = true;


  }
  else if (realtime_playing) {
    buttons[1].style("color:white;");
    // hacky... but it works for now
    halite.kill('SIGINT')
    realtime_playing = false;
  }
  if(realtime && sbar.getJackStatus() == 0) {
    console.warn("No jack server running");
  }
}




// Store our current state in a file
function generatesave(save, path) {
  if (path == undefined && save) {
  path = dialog.showSaveDialog({ filters: [{
      name: "Halite Save file",
      extensions: ['ncl']
  }]
});
}
  let boxnames = [];
  for (let i = 0; i < boxes.length; i++) {
    boxnames.push([i, boxes[i].getname(), boxes[i].getposition()])
  }
  let conns = [];
  for (let i = 0; i < connections.length; i++) {
    let formattedconn = connections[i].getinlets().concat(connections[i].gettype());
    conns.push(formattedconn);
  }
  let savefile = [boxnames, conns];
  if(save) {
  fs.writeFileSync(path, JSON.stringify(savefile));
  }
  return savefile;

}


// Open saved files from a path (usually from localStorage)
function openSavedFile(path) {
  try {
    let input = fs.readFileSync(path,'utf8');
      input = JSON.parse(input);
    for (let i = 0; i < input[0].length; i++) {
      boxes[input[0][i][0]] = new Component(input[0][i][1],input[0][i][2][0], input[0][i][2][1]);
    }
    for (let i = 0; i < input[1].length; i++) {
      connections.push(new Connection([boxes[input[1][i][0][0]], input[1][i][1][0]], [boxes[input[1][i][0][1]], input[1][i][1][1]], input[1][i][2]));
    }
} catch(err) {
  console.warn(err);
    for(let i = 0; i<3; i++) {
      boxes[i] = new Component(preset[i], (windowWidth/2)+((i-1)*100), windowHeight/2-250+((3-i)*100));
    }
  }
}


// Function for the 'load' button
// Writes the path of the save to localStorage
// Then we refresh to load it
function loadfile(path) {
  if (path == undefined) {
  var path = dialog.showOpenDialog({ properties: ['openFile', 'multiSelections'] });
  }
  localStorage.setItem("load", path);
  window.onbeforeunload();
  location.reload(true);
}

// Process keyboard commands and shortcuts
document.addEventListener("keydown", function(event) {
  if(!typing) { //ignore shortcuts when typing
  switch(keyCode)
     {
        case (8 || 46): // 'backspace' or 'delete' is pressed
        for (let i = boxes.length-1; i >= 0; i--) {
          boxes[i].deleteIfSelected();
        }
        break;

        case 78:  // 'n' is pressed
          boxes.push(new Component(undefined, 500, 500));
          break;
        case (90):
            unredo(-1);
          if(event.metaKey) {
            console.log('meta')
          }
        case (82):
          if(event.metaKey) {
            unredo(1);
          }
          break;
        default:
        break;

     }

    }
    else {
      if(keyCode === 13) { // if enter is pressed while typing, close textbox
        boxes.forEach(box => { box.closeinput()});
      }
    }
  });



// Functions to run on actions like clicks, resizes etc.

// Selects a component
function mouseClicked() {
  boxes.forEach(function(component) {
    component.mouseClicked();
  });
}

// This opens the name inputting field on a component
function doubleClicked() {
  for(let i = 0; i<boxes.length; i++) {
    boxes[i].doubleclick();
  }
}

// Resize everything accordingly on windowResize
function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
  sbar.windowresize();
  for (let i = 0; i < 3; i++) {
    buttons[i].style("left:x0px;".replace("x0", (cnvwidth/2)+buttonpresets[i][1]));
  }

}

//TODO: use this for undo/redo
function changed() {
  if (!initializing) {

  //console.log(history.length)
  //history.splice(histpos, history.length-histpos)
  history[histpos] = generatesave(0);
  histpos++;
  //console.log(history.length)
  //console.log('histpos', histpos)
  //precompile(0);
}
}



function unredo(step) {
  //console.log('histpos', histpos)
  histpos = histpos-2;
  histpos = histpos*(histpos>=0);
  let input = history[histpos];

  initializing = true;
  for (var i = boxes.length-1; i >= 0; i--) {
    boxes[i].delete();
  }
  //console.log(history);
  //connections = new Proxy( connectionsprox, arrayChangeHandler );
  //boxes = new Proxy( boxesprox, arrayChangeHandler );
  for (let i = 0; i < input[0].length; i++) {
    boxes[input[0][i][0]] = new Component(input[0][i][1],input[0][i][2][0], input[0][i][2][1]);
  }
  for (let i = 0; i < input[1].length; i++) {
    connections.push(new Connection([boxes[input[1][i][0][0]], input[1][i][1][0]], [boxes[input[1][i][0][1]], input[1][i][1][1]]));
  }
  initializing = false;
}


// Utility functions

// Change array order (used to make sure ground is at 0)
Array.prototype.move = function(from, to) {
    this.splice(to, 0, this.splice(from, 1)[0]);
};

// Intersection of rectangles
function intersect(aleft, aright, atop, abottom, bleft, bright, btop, bbottom) {
  return (Math.max(aleft, bleft) < Math.min(aright, bright) || Math.max(aright, bleft) < Math.min(aleft, bright)) &&
        (Math.max(atop, btop) < Math.min(abottom, bbottom) || Math.max(abottom, btop) < Math.min(atop, bbottom));
}

// Remove duplicates from array (necessary for precompile())
function removeDuplicates(array) {
  let x = {};
  array.forEach(function(z) {
    let i = JSON.stringify(z);
    if(!x[i]) {
      x[i] = z
    }
  })
  return Object.values(x);
};
