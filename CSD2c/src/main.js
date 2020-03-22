// Our main code manages instances of all our other classes, handles spawning C++ and preparing the code

// All our objects, plus the data that we need to convert it to halite
let types = {
  // Halite standard components
  'resistor': {
    'inlets': 1,
    'outlets': 1,
    'colors': ['#229FD7', '#229FD7'],
    'args': 1,
    'code': " resistor, a0, i0, i1"
  },
  'ground': {
    'inlets': 1,
    'outlets': 0,
    'args': 0,
    'colors': ['#191919'],
    'code': " ground"
  },
  'voltage': {
    'inlets': 0,
    'outlets': 2,
    'args': 1,
    'colors': ['#FE3B3B', '#191919'],
    'tooltips' :   ['+', '-'],
    'code': " voltage, a0, i0, i1"
  },

  'current': {
    'inlets': 1,
    'outlets': 1,
    'args': 1,
    'colors': ['#191919', '#FE3B3B'],
    'tooltips' :   ['-', '+'],
    'code': " current, a0, i0, i1"
  },

  'click': {
    'inlets': 0,
    'outlets': 2,
    'args': 1,
    'colors': ['#FE3B3B', '#191919'],
    'tooltips' :   ['+', '-'],
    'code': " click, a0, i0, i1"
  },

  'capacitor': {
    'inlets': 1,
    'outlets': 1,
    'args': 1,
    'colors': ['#229FD7', '#229FD7'],
    'code': " capacitor, a0, i0, i1"
  },
  'inductor': {
    'inlets': 1,
    'outlets': 1,
    'args': 1,
    'colors': ['#229FD7', '#229FD7'],
    'code': " inductor, a0, i0, i1"
  },
  'transformer': {
    'inlets': 2,
    'outlets': 2,
    'args': 1,
    'colors': ['#FE3B3B', '#191919', '#FE3B3B', '#191919'],
    'tooltips' :   ['Input +', 'Input -', 'Output +', 'Output -'],
    'code': " transformer, a0, i0, i1, i2, i3"
  },

  'diode': {
    'inlets': 1,
    'outlets': 1,
    'args': 0,
    'colors': ['#FE3B3B', '#229FD7'],
    'tooltips' :   ['Anode', 'Cathode'],
    'code': " diode, i0, i1"
  },
  'bjt': { // dont use this, use pnp and npn instead. The reason im including this is because we have some old patches depending on it
    'inlets': 1,
    'outlets': 2,
    'args': 1,
    'colors': ['#FE3B3B', '#191919', '#229FD7'],
    'tooltips': ['Base', 'Collector', 'Emitter'],
    'code': " bjt, i0, i1, i2, a0"
  },
  'pnp': {
    'inlets': 1,
    'outlets': 2,
    'args': 0,
    'colors': ['#FE3B3B', '#191919', '#229FD7'],
    'tooltips': ['Base', 'Emitter', 'Collector'],
    'code': " bjt, i0, i2, i1, 1"
  },
  'npn': {
    'inlets': 1,
    'outlets': 2,
    'args': 0,
    'colors': ['#FE3B3B', '#191919', '#229FD7'],
    'tooltips': ['Base', 'Emitter', 'Collector'],
    'code': " bjt, i0, i2, i1, 0"
  },

  'op-amp': {
    'inlets': 2,
    'outlets': 1,
    'args': 0,
    'colors': ['#5B8DEE', '#191919', '#FE3B3B', '#191919', '#229FD7'],
    'tooltips': ['V+', 'V-', 'VOut'],
    'code': " opa, i0, i1, i2"
  },

  // Our analog Components
  'varres': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['analog', 'digital', 'analog'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " varres, i0, i2, d1"
  },

  'pot': {
    'inlets': 2,
    'outlets': 2,
    'datatypes': ['analog', 'digital', 'analog', 'analog'],
    'args': 1,
    'colors': ['#FE3B3B', '#229FD7', '#191919', '#229FD7'],
    'code': " potentiometer, a0, i0, i2, i3, d1"
  },
  'input': {
    'inlets': 0,
    'outlets': 2,
    'args': 2,
    'colors': ['#FE3B3B', '#191919'],
    'tooltips' :   ['+', '-'],
    'code': " input, a0, a1, i0, i1"
  },
  'output': {
    'inlets': 2,
    'outlets': 0,
    'args': 1,
    'colors': ['#FE3B3B', '#191919'],
    'tooltips' :   ['+', '-'],
    'code': " probe, i0, i1, a0"
  },

  'print-': {
    'inlets': 1,
    'outlets': 0,
    'datatypes': ['digital'],
    'colors': ['#229FD7'],
    'args': 0,
    'tooltips' :   ['Anything to be printed'],
    'code': " print-, d0"
  },

  // conversion objects
  'dac': {
    'inlets': 1,
    'outlets': 2,
    'datatypes': ['digital', 'analog', 'analog'],
    'colors': ['#229FD7', '#229FD7'],
    'args': 0,
    'code': " dac, d0, i1, i2"
  },
  'adc': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['analog', 'analog', 'digital'],
    'colors': ['#229FD7', '#229FD7'],
    'args': 0,
    'code': " adc, i0, i1, d2"
  },
  'comment': {
    'inlets': 0,
    'outlets': 0,
    'datatypes': [],
    'colors': [],
    'args': 0,
    'code': " comment, a0"
  },

  // digital objects
  // I/O

  'slider-': {
    'inlets': 0,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#FE3B3B', '#191919'],
    'code': " slider-, d0"
  },

  'input-': {
    'inlets': 0,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 2,
    'colors': ['#FE3B3B', '#191919'],
    'code': " input-, a0, a1, d0"
  },
  'stinput-': {
    'inlets': 0,
    'outlets': 2,
    'datatypes': ['digital', 'digital'],
    'args': 2,
    'colors': ['#FE3B3B', '#191919'],
    'code': " stinput-, a0, a1, d0, d1"
  },
  'rtinput-': {
    'inlets': 0,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 1,
    'colors': ['#FE3B3B', '#191919'],
    'code': " rtinput-, a0, d0"
  },
  'output-': {
    'inlets': 2,
    'outlets': 0,
    'datatypes': ['digital', 'digital'],
    'args': 1,
    'colors': ['#229FD7'],
    'code': " output-, a0 , d0, d1"
  },
  'dcblock-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " dcblock-, d0, d1"
  },

  'accum-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " accum-, d0, d1"
  },
  'change-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " change-, d0, d1"
  },
  'delta-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " delta-, d0, d1"
  },
  'and-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " and-, d0, d1"
  },
  '&&-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " &&-, d0, d1"
  },
  'or-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " or-, d0, d1"
  },
  '||-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " ||-, d0, d1"
  },
  'abs-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " abs-, d0, d1"
  },
  'ceil-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " ceil-, d0, d1"
  },
  'floor-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " floor-, d0, d1"
  },
  'gate-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " floor-, d0, d1, d2"
  },
  'scale-': {
    'inlets': 5,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital', 'digital', 'digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " scale-, d0, d1, d2, d3, d4, d5"
  },
  'clip-': {
    'inlets': 5,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital', 'digital', 'digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " clip-, d0, d1, d2, d3"
  },


  'elapsed-': {
    'inlets': 0,
    'outlets': 1,
    'datatypes': ['digital'],
    'args': 0,
    'colors': ['#229FD7'],
    'code': " elapsed-, d0"
  },


  'ctlin-': {
    'inlets': 0,
    'outlets': 2,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7', '#229FD7'],
    'code': " ctlin-, d0, d1"
  },
  'notein-': {
    'inlets': 0,
    'outlets': 2,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7', '#229FD7'],
    'code': " notein-, d0, d1"
  },
  'mtof-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'args': 0,
    'colors': ['#229FD7', '#229FD7'],
    'code': " mtof-, d0, d1"
  },

  // Signal generation
  'cycle-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'colors': ['#229FD7', '#229FD7'],
    'args': 0,
    'code': " cycle-, d0, d1"
  },
  'saw-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'colors': ['#229FD7', '#229FD7'],
    'args': 0,
    'code': " saw-, d0, d1"
  },
  'rect-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'colors': ['#229FD7', '#229FD7'],
    'args': 0,
    'code': " rect-, d0, d1"
  },
  'triangle-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'colors': ['#229FD7', '#229FD7'],
    'args': 0,
    'code': " triangle-, d0, d1"
  },

  'sig-': {
    'inlets': 0,
    'outlets': 1,
    'datatypes': ['digital', 'digital'],
    'colors': ['#229FD7', '#229FD7'],
    'args': 1,
    'code': " sig-, a0, d0"
  },

  // Delay
  'delay-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " digidelay, d0, d1, d2"
  },

  // arithmetics without arg
  '+-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " +-, d0, d1, d2"
  },
  '--': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " --, d0, d1, d2"
  },
  '!--': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " !--, d0, d1, d2"
  },
  '*-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " *-, d0, d1, d2"
  },
  '/-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " /-, d0, d1, d2"
  },
  '!/-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " !/-, d0, d1, d2"
  },
  '>-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " >-, d0, d1, d2"
  },
  '<-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " <-, d0, d1, d2"
  },
  '>=-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " >=-, d0, d1, d2"
  },
  '<=-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " <=-, d0, d1, d2"
  },
  '==-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " ==-, d0, d1, d2"
  },
  '!=-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " !=-, d0, d1, d2"
  },
  '%-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " %-, d0, d1, d2"
  },
  '!%-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " !%-, d0, d1, d2"
  },
    'pow-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " pow-, d0, d1, d2"
  },
    'sqrt-': {
    'inlets': 2,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " sqrt-, d0, d1, d2"
  },
    'ln-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " ln-, d0, d1"
  },
    'log2-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " log2-, d0, d1"
  },
    'log10-': {
    'inlets': 1,
    'outlets': 1,
    'datatypes': ['digital', 'digital', 'digital'],
    'colors': ['#229FD7', '#229FD7', '#229FD7'],
    'args': 0,
    'code': " log10-, d0, d1"
  },

}




// Aliases

types['r'] = types['resistor'];
types['c'] = types['capacitor'];
types['i'] = types['inductor'];
types['v'] = types['voltage'];
types['opamp'] = types['op-amp'];

// Max, PD en Gendsp have different names
// We accept all these names so you don't need to search for the right one
types['osc-'] = types['cycle-'];
types['sine-'] = types['cycle-'];
types['train-'] = types['rect-'];
types['square-'] = types['rect-'];
types['phasor-'] = types['saw-']; // for now, even though they arent exactly the same


// default preset
let preset = ["ground", "output 0.3", 'input ' + "./Media/sample-44k.wav 0.2"];

let halite; // variable for halite process

let halite_running = false; // Checks if we are playing in realtime

let sbar = new Sidebar; // Sidebar (see sidebar.js)

let halsettings = [44100, 44100, 24, '.WAV', 1024, 1]; // Export and audio settings for halite

//let typing = false; // Check if we're typing, if so, disable keyboard shortcuts




// Close background processes when quitting or reloading
// DOESN'T ALWAYS WORK!!
window.onbeforeunload = function() {
  generatesave(1, home + '/lastsession.ncl');
  try {
    if (halite_running) {
      halite.kill('SIGINT');
    }
  } catch (e) {}
};

process.on('SIGINT', window.onbeforeunload); // catch ctrl-c
process.on('SIGTERM', window.onbeforeunload); // catch kill


// Dragging and dropping files
document.ondragover = document.ondrop = (ev) => {
  ev.preventDefault()
}

document.body.ondrop = (ev) => // this is not working: why?
  {
    // If we get a new file, move it to our media library
    fs.createReadStream(ev.dataTransfer.files[0].path).pipe(fs.createWriteStream(home + '/media/' + ev.dataTransfer.files[0].name));
    ev.preventDefault()
    // Update the files in the medialibrary
    sbar.libraryUpdate();
  }

// All our control buttons in an array
let buttonpresets = [
  [']', 100, function() {
    boxes.push(new Component(undefined, 500, 500))
  }],
  ['{', 0, function() {
    startHalite(1)
  }],
  ['w', -100, function() {
    startHalite(0)
  }]
];
let buttons = [];


window.addEventListener("resize", () => {
  sbar.windowresize()
  for (var i = 0; i < buttons.length; i++) {
    buttons[i].style.left = ((window.innerWidth - sbarwidth) / 2) + buttonpresets[i][1] + "px";
  }

});

// Loads our last session from local storage
// This should make sure that refreshing or closing the app (or a potential crash) won't mean our patch is lost!
var state = localStorage.getItem("load");
if (state == undefined || state == 'new') {
  for (let i = 0; i < 3; i++) {
    boxes[i] = new Component(preset[i], (window.innerWidth / 2) + ((i - 1) * 100), window.innerHeight / 2 - 250 + ((3 - i) * 100));
  }
} else {
  openSavedFile(state);
}



// set up buttons at the bottom
// I had to set up so many buttons throughout this program that using loops saves me hundreds of lines
for (let i = 0; i < 3; i++) {
  buttons[i] = document.createElement("BUTTON");
  buttons[i].innerHTML = buttonpresets[i][0];
  buttons[i].style.cssText = "border-radius:100%; border:none; outline:none; font-size:32px; color:white; background-color:#303030; position:fixed;  bottom:60px;  left:x0px;".replace("x0", ((window.innerWidth - sbarwidth) / 2) + buttonpresets[i][1]);
  buttons[i].className = "button";
  buttons[i].style.height = "48px";
  buttons[i].style.width = "48px";
  buttons[i].style.fontFamily = "Entypo";
  buttons[i].style.zIndez = "5";
  buttons[i].addEventListener("click", buttonpresets[i][2]);
  document.body.appendChild(buttons[i])

}

function convertMetric(str) {
  if (!str.includes('/')) { // filter out links
    str = str.replace('p', 'e-12').replace('n', 'e-9').replace('u', 'e-6').replace('m', 'e-3').replace('k', 'e3');
  }
    else if(str.includes('./media')) { //We need this because linux is more case sensitive than mac somehow
    str = str.replace('./media', home + '/Media/');
  }
  else if(str.includes('./')) { //for shorter filenames
    str = str.replace('./', home + '/');
  }
  return str;
}

// Convert our line-based network into an electrical nodes network that Halite can interpret
function precompile(save = 1) {
  // Make sure we have correct OSC addresses for our sliders
  for (var i = 0; i < sliders.length; i++) {
    sliders[i].update();
  }


  let iterconnections = [
    [
      [0, 0],
      [0, 0]
    ]
  ];
  let digitalconns = [];
  let iterboxes = [];
  for (let i = 0; i < connections.length; i++) {
    if (connections[i].gettype() != 'digital') {
      let lets = connections[i].getinlets();
      iterconnections.push([
        [lets[0][0], lets[1][0]],
        [lets[0][1], lets[1][1]]
      ]);
    } else {
      let lets = connections[i].getinlets();
      digitalconns.push([
        [lets[0][0], lets[1][0]],
        [lets[0][1], lets[1][1]]
      ]);
    }
  }

  let exists = [
    [0, 0]
  ];
  for (let i = 0; i < boxes.length; i++) {
    let boxtype = boxes[i].gettype();
    let boxargs = boxes[i].getargs();
    let optargs = boxes[i].getoptargs();
    let itercode = types[boxtype]['code'];
    let amtinlets = parseInt(types[boxtype]['inlets']) + parseInt(types[boxtype]['outlets']);
    let inletconns = Array(amtinlets).fill([]);
    for (let x = 0; x < amtinlets; x++) {
      let target = JSON.stringify([i, x]);
      for (var d = 0; d < iterconnections.length; d++) {
        if (JSON.stringify(iterconnections[d]).includes(target)) {
          let returnval = 0;
          for (var ex = 0; ex < exists.length; ex++) {
            if (exists[ex] != undefined && (JSON.stringify(exists[ex]).includes(JSON.stringify(iterconnections[d][0])) || JSON.stringify(exists[ex]).includes(JSON.stringify(iterconnections[d][1])))) {
              exists[ex].push(iterconnections[d][0])
              exists[ex].push(iterconnections[d][1])
              itercode = itercode.replace("i" + x, ex);
              returnval = 1;
              break;
            }
          }
          if (!returnval) {
            itercode = itercode.replace("i" + x, exists.length);
            exists.push(iterconnections[d]);
          }

        }
      }

      // check of type == digital??
      for (let d = 0; d < digitalconns.length; d++) {
        if (JSON.stringify(digitalconns[d]).includes(target)) {
          inletconns[x] = inletconns[x].concat(d + 1);

        }
      }
      itercode = itercode.replace("d" + x, JSON.stringify(inletconns[x]).replace(',', ':'));

      if(itercode.includes("i" + x)) {
        console.log("Not all analog inlets connected! \n");
        console.log("Analog boxes with unconnected inlets will be ignored \n");
        x = amtinlets;
        boxes[i].isValid(false);

      }

    }
    for (let x = 0; x < boxargs.length; x++) {
      itercode = itercode.replace("a" + x, convertMetric(boxargs[x]))
    }
    if (boxes[i].isValid()) {
    itercode = itercode + ', ' + JSON.stringify(optargs).replace(',', ':') + '\n';
    }
    else {
      itercode = '';
    }

    iterboxes[i] = itercode;
  }

  let setupnodes = " setup, nodes, digiconns \n".replace('nodes', exists.length).replace('digiconns', digitalconns.length + 1);
  code = setupnodes + iterboxes.join("");
  sbar.codeUpdate(code);

  if (save) {
    fs.writeFileSync(home + "/precompile.ncl", code);
  }
}

function startHalite(realtime) {
  let params = [];
  let prefixes = ['-t ', '-s ', '-d ', '-f ', '-b ', '-n ']
  for (var i = 0; i < halsettings.length; i++) {
    params.push(prefixes[i] + halsettings[i]);
  }
  let haliteappendix;
  let halitecmd = __dirname + '/../compiled/Halite'

  // Write our patch to a file that Halite can read
  precompile(1);

  if (realtime) {
    haliteappendix = ['-r', '-i ' + home + '/precompile.ncl'];
    buttons[1].style.color = "red";
  } else {
    // If we are not running in realtime, get a path to save the wav/aif file
    let savepath = dialog.showSaveDialog({
      filters: [{
        name: "Audio",
        extensions: [halsettings[3].replace('.', '').toLowerCase()]
      }]
    });
    haliteappendix = ['-o ' + savepath, '-i ' + home + '/precompile.ncl']; // this doesn't contain everything yet...
  }



  if (!(realtime && (halite_running))) {
    halite = spawn(path.join(__dirname, '/../compiled/Halite'), haliteappendix.concat(params));



    halite.stdout.on('data', function(data) {
      console.log('Halite: ' + data);
    });

    halite.stderr.on('data', function(data) {
      console.warn('Halite Error: ' + data);
    });

    halite.on('close', function(code) {
      buttons[1].style.color = "white"
      console.log('Halite closed with code ' + code);
      halite_running = false;
    });
    halite_running = true;


  } else if (halite_running) {
    buttons[1].style.color = "white";
    // hacky... but it works for now
    halite.kill('SIGINT')
    halite_running = false;
  }

}




// Store our current state in a file
function generatesave(save, path) {
  if (path == undefined && save) {
    path = dialog.showSaveDialog({
      filters: [{
        name: "Halite Save file",
        extensions: ['ncl']
      }]
    });
  }
  if(path != undefined && !path.length == 0) {
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
  if (save) {
    fs.writeFileSync(path, JSON.stringify(savefile));
  }

  return savefile;
}

}


// Open saved files from a path (usually from localStorage)
function openSavedFile(path) {
  try {
    let input = fs.readFileSync(path, 'utf8');
    input = JSON.parse(input);
    for (let i = 0; i < input[0].length; i++) {
      boxes[input[0][i][0]] = new Component(input[0][i][1], input[0][i][2][0], input[0][i][2][1]);
    }
    for (let i = 0; i < input[1].length; i++) {
      connections.push(new Connection([boxes[input[1][i][0][0]], input[1][i][1][0]], [boxes[input[1][i][0][1]], input[1][i][1][1]], input[1][i][2]));
    }
  } catch (err) {
    console.warn(err);
    for (let i = 0; i < 3; i++) {
      boxes[i] = new Component(preset[i], (window.innerWidth / 2) + ((i - 1) * 100), window.innerHeight / 2 - 250 + ((3 - i) * 100));
    }
  }
}


// Function for the 'load' button
// Writes the path of the save to localStorage
// Then we refresh to load it
function loadfile(e, path) {
  if (path == undefined) {
    var path = dialog.showOpenDialog({
      properties: ['openFile', 'multiSelections']
    });
  }
  if (path != undefined && !path.length == 0) {
  localStorage.setItem("load", path);
  window.onbeforeunload();
  location.reload(true);
}
}

// Process keyboard commands and shortcuts
document.addEventListener("keydown", function(event) {
  var key = event.which || event.keyCode; // keyCode detection
  var ctrl = event.ctrlKey ? event.ctrlKey : ((key === 17) ? true : false); // ctrl detection

  if (!blocked) //ignore shortcuts when typing
  {
    switch (event.keyCode) {
      case (8 || 46): // 'backspace' or 'delete' is pressed
        for (let i = boxes.length - 1; i >= 0; i--) {
          boxes[i].deleteIfSelected();
        }
        break;

      case 78: // 'n' is pressed
        boxes.push(new Component(undefined, 500, 500));
        break;
      case (67): // ctrl-c
        if (event.metaKey || ctrl) {
          clipboard.writeText(JSON.stringify(copySelection()));
        }
        break;
      case (88): // ctrl-x
        if (event.metaKey || ctrl) {
          clipboard.writeText(JSON.stringify(copySelection()));
          for (let i = boxes.length - 1; i >= 0; i--) {
            boxes[i].deleteIfSelected();
          }
        }
        break;
      case (68): // ctrl-d
        if (event.metaKey || ctrl) {
          pasteSelection(copySelection()); // nice

        }
        break;
      case (86):
        if (event.metaKey || ctrl) {
          pasteSelection(JSON.parse(clipboard.readText()));
        }
        break;
      default:
        break;

    }

  } else {
    if (event.keyCode === 13) // if enter is pressed while typing, close textbox
    {
      boxes.forEach(box => {
        box.closeinput()
      });
    }
  }
});

function copySelection() {
  let boxnames = [];
  let boxindices = [];
  for (var i = 0; i < boxes.length; i++) {
    if (boxes[i].isSelected()) {
      boxindices.push(i);
      boxnames.push([i, boxes[i].getname(), boxes[i].getposition()])
    }
  }
  let conns = [];


  for (let i = 0; i < connections.length; i++) {
    let connInlets = connections[i].getinlets();
    if (boxindices.includes(connInlets[0][0]) && boxindices.includes(connInlets[0][1])) {
      let newindices = [
        [boxindices.indexOf(connInlets[0][0]), boxindices.indexOf(connInlets[0][1])]
      ];
      let formattedconn = newindices.concat([connInlets[1]].concat(connections[i].gettype()));
      conns.push(formattedconn);
    }
  }

  let savefile = [boxnames, conns];
  return savefile;



}

function pasteSelection(pasted) {

  try {
    let tempboxes = [];
    for (let i = 0; i < pasted[0].length; i++) {
      var component = new Component(pasted[0][i][1], pasted[0][i][2][0] + 50, pasted[0][i][2][1] + 50)
      boxes.push(component);
      tempboxes.push(component);
    }


    for (let i = 0; i < pasted[1].length; i++) {
      let newconn = new Connection([tempboxes[pasted[1][i][0][0]], pasted[1][i][1][0]], [tempboxes[pasted[1][i][0][1]], pasted[1][i][1][1]], pasted[1][i][2]);
      connections.push(newconn);
    }
  } catch (err) {
    console.log("Invalid clipboard content");
  }

}
