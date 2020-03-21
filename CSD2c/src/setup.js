let fs = require("fs");
const os = require('os');

let dialog = require('electron').remote.dialog;
let path = require('path');
const net = require('net');


const {spawn} = require('child_process');
const {clipboard} = require('electron');


let sbarwidth = 350;

let tab = 1;

let midisliders;

let sliders = [];


const { Client } = require('node-osc');

const client = new Client('127.0.0.1', 9000);




// Link to our read-write working directory
let home = os.homedir() + "/Documents/Circuitry";
// __dirname refers to the read-only working directory inside the electron app

let connections = []; // Where we store our components
let boxes = []; // Where we store our connections
let connecting = -1; // Is any inlet currently in the connecting state?
let blocked = false; // for blocking actions that cause glitches, like either inputting, deleting or connecting at the same time

let code = '';


// Set background color
document.body.style.backgroundColor = "#505050";
document.head.style.height = "1500px";


// Set up a file structure and copy all example files there
// The inside of an electron app becomes read-only once compiled
if (!fs.existsSync(home)) {
  fs.mkdirSync(home);
  fs.mkdirSync(home + "/Saves");
  fs.mkdirSync(home + "/Saves/Examples");
  fs.mkdirSync(home + "/Outputs");
  fs.mkdirSync(home + "/Media");


  // copying everything...
  fs.copyFile(__dirname + '/../media/Solo.wav', home + '/Media/Solo.wav', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../media/sample-44k.wav', home + '/Media/sample-44k.wav', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/Fuzzface.ncl', home + '/Saves/Examples/Fuzzface.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/BigMuff.ncl', home + '/Saves/Examples/BigMuff.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/ActiveLowpass.ncl', home + '/Saves/Examples/ActiveLowpass.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/ChorusDelay.ncl', home + '/Saves/Examples/ChorusDelay.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/RClowpass.ncl', home + '/Saves/Examples/RCLowpass.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/Octaver.ncl', home + '/Saves/Examples/Octaver.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/SimpleOscillator.ncl', home + '/Saves/Examples/SimpleOscillator.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/MidiSynth.ncl', home + '/Saves/Examples/MidiSynth.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/RIHighpass.ncl', home + '/Saves/Examples/RIHighpass.ncl', (err) => {
    if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/Examples/Wah-Wah.ncl', home + '/Saves/Examples/Wah-Wah.ncl', (err) => {
    if (err) throw err;
  });

}


// Add invisibe below the sidebar so we can make room for scrolling
let scrollitem = document.createElement("div");
scrollitem.innerHTML = '!!';
scrollitem.style.position = "absolute";
scrollitem.style.height = "8px";
scrollitem.style.width = "8px";
document.body.appendChild(scrollitem);


// We use the dragselect library for selecting our components
// It happened to intergrate fairly well with the dragElement function I copied of Stackoverflow, so that's nice
let ds = new DragSelect({});


function dragElement(elmnt, sidebar) {
  var pos1 = 0,
    pos2 = 0,
    pos3 = 0,
    pos4 = 0;

  elmnt.onmousedown = dragMouseDown;

  var adjustBodyWidth = function(nPixels) {
    scrollitem.style.left = nPixels + 'px';
  };

  function dragMouseDown(e) {
    e = e || window.event;
    e.preventDefault();
    // get the mouse cursor position at startup:
    pos3 = e.clientX;
    pos4 = e.clientY;
    document.onmouseup = closeDragElement;
    // call a function whenever the cursor moves:
    document.addEventListener("mousemove", elementDrag);
  }

  function elementDrag(e) {
    e = e || window.event;
    e.preventDefault();
    // calculate the new cursor position:
    pos1 = pos3 - e.clientX;
    pos2 = pos4 - e.clientY;
    pos3 = e.clientX;
    pos4 = e.clientY;

    if (sidebar == undefined && blocked == false) {
      // Make sure the element stays selected
      ds.addSelection(elmnt);
      // Get all selected elements (for dragging multiple components at once with shift/ctrl)
      let elements = ds.getSelection();
      // Set the new position for all elements
      for (var i = 0; i < elements.length; i++) {
        elements[i].style.left = (elements[i].offsetLeft - pos1) + "px";
        elements[i].style.top = (elements[i].offsetTop - pos2) + "px";
        elements[i].x = (elements[i].offsetLeft - pos1);
        elements[i].y = (elements[i].offsetTop - pos2);
        // Make sure they don't fall off the screen !! MIGHT BE OPTIONAL NOW WE USE THAT INVISIBLE MARKER
        if (elements[i].x > window.innerWidth - sbarwidth - 30) {
          adjustBodyWidth(elmnt.x + sbarwidth + 100);
        }

      }
      for (var i = 0; i < connections.length; i++) {
        connections[i].update();
      }
    } else if (blocked == false) {
      // If we're dragging the sidebar, just set the horizontal position
      sbarwidth = window.innerWidth - (elmnt.offsetLeft - pos1);
      elmnt.style.right = sbarwidth - 30 + "px";
      sidebar.windowresize();
    }
    ds.break();

  }

  function closeDragElement() {
    // stop moving when mouse button is released:
    document.onmouseup = null;
    document.removeEventListener("mousemove", elementDrag);
  }
}
