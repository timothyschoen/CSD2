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

  fs.copyFile(__dirname + '/../saves/Documentation.pdf', home + '/Documentation.pdf', (err) => {
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
