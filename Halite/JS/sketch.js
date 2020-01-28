var dialog = require('electron').remote.dialog;
var fs = require("fs");
const util = require('util');
const { spawn } = require('child_process');

let boxes = [];
let connections = [];
let connecting = -1;

let realtime_playing = false;

let consol = '';

let sbar;

let sbarwidth = 350;
var cnvwidth;

let code = '';

let draginstance = -1;

let addbutton, playtofile, realtimeplay, openbutton, savebutton, refreshbutton, newbutton;

Array.prototype.move = function(from, to) {
    this.splice(to, 0, this.splice(from, 1)[0]);
};



window.onbeforeunload = function(){
  generatesave('./lastsession.hcl');
  exec('killall Halite');
  app.quit();
};


function preload() {
  //Fix for the indented closebutton on macOs, needed to make window draggable
  var windowTopBar = document.createElement('div');
  windowTopBar.style.width = "100%";
  windowTopBar.style.height = "20px";
  windowTopBar.style.position = "absolute";
  windowTopBar.style.top = windowTopBar.style.left = 0;
  windowTopBar.style.webkitAppRegion = "drag";
  document.body.appendChild(windowTopBar);
}

function setup() {
  var preset = ["ground", "output 0.3", 'input "./samples/sample-44k.wav" 0.2'];
  createCanvas(windowWidth, windowHeight);

   cnvwidth = windowWidth-sbarwidth;

  var state = localStorage.getItem("load");
  if(state == undefined) {
    for(var i = 0; i<3; i++) {
      boxes[i] = new Component(preset[i], (windowWidth/2)+((i-1)*100), windowHeight/2-250+((3-i)*100));
    }
  }
  if(state == 'new') {
    console.log('new project');
  }
  else {
    try {
    var input = fs.readFileSync(state,'utf8');
      input = JSON.parse(input);
    for (var i = 0; i < input[0].length; i++) {
      boxes[input[0][i][0]] = new Component(input[0][i][1],input[0][i][2][0], input[0][i][2][1]);
    }
    for (var i = 0; i < input[1].length; i++) {
      connections.push(new Connection([boxes[input[1][i][0][0]], input[1][i][1][0]], [boxes[input[1][i][0][1]], input[1][i][1][1]]));
    }
  } catch(err) {
    for(var i = 0; i<3; i++) {
      boxes[i] = new Component(preset[i], (windowWidth/2)+((i-1)*100), windowHeight/2-250+((3-i)*100));
    }
  }
  }


  sbar = new Sidebar;

  addbutton = createButton('+');
  addbutton.size(48, 48);
  addbutton.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#229FD7;");
  addbutton.id('addbutton');
  addbutton.mousePressed(function () {
    boxes.push(new Component);
  });
  addbutton.position(cnvwidth/2+100, windowHeight-100);

  realtimeplay = createButton('R');
  realtimeplay.size(48, 48);
  realtimeplay.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#fedac3;");
  realtimeplay.id('realtimeplay');
  realtimeplay.mousePressed(function () {
    precompile();
    if(realtime_playing == false) {
    const child = spawn('./compiled/Halite -realtime');

    child.stdout.on('data', function (data) {
      console.log('Halite: ' + data);
    });

    child.stderr.on('data', function (data) {
      console.log('Halite Error: ' + data);
    });

    child.on('close', function (code) {
        console.log('child process exited with code ' + code);
    });
    realtime_playing = true;
  }
  else if (realtime_playing) {
    // hacky... but it works for now
    const { stdout2, stderr2 } = exec('killall Halite');
    realtime_playing = false;
  }
  });
  realtimeplay.position(cnvwidth/2, windowHeight-100);

//(253, 155, 163) (200, 207, 233)

  playtofile = createButton('S');
  playtofile.size(48, 48);
  playtofile.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#b7ead7;");
  playtofile.id('playtofile');
  playtofile.mousePressed(function () {
    precompile();

    const child = spawn('./compiled/Halite');

    child.stdout.on('data', function (data) {
      console.log('stdout: ' + data);
    });

    child.stderr.on('data', function (data) {
      console.log('stderr: ' + data);
    });

    child.on('close', function (code) {
        console.log('child process exited with code ' + code);
    });


  });
  playtofile.position(cnvwidth/2-100, windowHeight-100);


  newbutton = createButton('N');
  newbutton.size(32, 32);
  newbutton.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#b7ead7;");
  newbutton.id('newbutton');
  newbutton.mousePressed(function () {
    localStorage.setItem("load", 'new');
    location.reload(true);
  });
  newbutton.position(100, 10);


  openbutton = createButton('O');
  openbutton.size(32, 32);
  openbutton.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#b7ead7;");
  openbutton.id('openbutton');
  openbutton.mousePressed(function () {
    loadfile();
  });
  openbutton.position(150, 10);


  savebutton = createButton('S');
  savebutton.size(32, 32);
  savebutton.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#b7ead7;");
  savebutton.id('savebutton');
  savebutton.mousePressed(function () {
    generatesave();
  });
  savebutton.position(200, 10);


  refreshbutton = createButton('R');
  refreshbutton.size(32, 32);
  refreshbutton.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:#b7ead7;");
  refreshbutton.id('refreshbutton');
  refreshbutton.mousePressed(function () {
    generatesave('./lastsession.hcl');
    //loadfile refreshes the page automatically
    loadfile('./lastsession.hcl');
  });
  refreshbutton.position(250, 10);


  }



  function doubleClicked() {
    for(var i = 0; i<boxes.length; i++) {
      boxes[i].doubleclick();
    }
  }
  function keyPressed() {
    if (keyCode === ENTER) {
    for(var i = 0; i<boxes.length; i++) {
      boxes[i].closeinput();
    }
  }
  }

  function windowResized() {
    resizeCanvas(windowWidth, windowHeight);
    sbar.windowresize();
    addbutton.position(cnvwidth/2+100, windowHeight-100);
    realtimeplay.position(cnvwidth/2, windowHeight-100);
    playtofile.position(cnvwidth/2-100, windowHeight-100);

  }

function draw() {

  background(255,255,255);
  noStroke();

  if(width < windowWidth) {
  resizeCanvas(windowWidth, windowHeight);

}



for(var i = 0; i<boxes.length; i++) {
  boxes[i].draw();
}

connections.forEach(conn => {
  conn.draw();
});


//Draw a line to the mouse while making the connection
if(connecting != -1) {
  var linex = connecting[0].getinlets()[connecting[1]].getposition()[0];
  var liney = connecting[0].getinlets()[connecting[1]].getposition()[1];
  stroke(0)
  line(linex, liney, mouseX, mouseY);
}
sbar.draw()

}

let types = {
  'resistor': {'inlets': 1, 'outlets': 1, 'colors': ['#229FD7', '#229FD7'], 'args': 1, 'code': " resistor, a0, i0, i1"},
  'cycle': {'inlets': 0, 'outlets': 2, 'args': 1, 'colors': ['#ff0000', '#000000'], 'code': " cycle, a0, i0, i1"},
  'ground': {'inlets': 1, 'outlets': 0, 'args': 0, 'colors': ['#000000'], 'code': ""},
  'input': {'inlets': 0, 'outlets': 2, 'args': 2, 'colors': ['#ff0000', '#000000'], 'code': " input, a0, a1, i0, i1"},
  'output': {'inlets': 1, 'outlets': 0, 'args': 1, 'colors': ['#229FD7'], 'code': " output, a0, i0"},
  'voltage': {'inlets': 0, 'outlets': 2, 'args': 1, 'colors': ['#ff0000', '#000000'], 'code': " voltage, a0, i0, i1"},
  'capacitor': {'inlets': 1, 'outlets': 1, 'args': 1, 'colors': ['#229FD7', '#229FD7'], 'code': " capacitor, a0, i0, i1"},
  'diode': {'inlets': 1, 'outlets': 1, 'args': 0, 'colors': ['#ff0000', '#229FD7'], 'code': " diode, i0, i1"},
  'comment': {'inlets': 0, 'outlets': 0, 'args': 1, 'colors': [], 'code': ""},
  'bjt': {'inlets': 1, 'outlets': 2, 'args': 1, 'colors': ['#ff0000', '#000000', '#229FD7'], 'code': " bjt, i0, i1, i2, a0"}
}





function precompile() {
    var iterconnections = [];
    var iterboxes = [];
    var nodes = [];
    for(var i = 0; i < connections.length; i++) {
      var lets = connections[i].getinlets();
      iterconnections.push([[lets[0][0], lets[1][0]], [lets[0][1], lets[1][1]]]);
    }

  // Convert our line-based network into an electrical nodes network

  //First find all unique connection points
    for(var x = 0; x < iterconnections.length; x++) {
        for(var z = 0; z < iterconnections[x].length; z++) {
          if(!JSON.stringify(nodes).includes(JSON.stringify(iterconnections[x][z]))) {
              // fix ground to 0 // DOESNT WORK!!
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
      for(var x = 0; x < nodes.length; x++) {
          for(var y = 0; y < nodes[x].length; y++) {
            for(var z = 0; z < nodes.length; z++) {
                for(var a = 0; a < nodes[z].length; a++) {
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
  for(var x = nodes.length-1; x >= 0;  x--) {
      if(nodes[x].length == 0 || nodes[x] == null) {
        nodes.splice(x, 1);
      }
  }
  console.log("post: ", nodes);

  for(var i = 0; i < boxes.length; i++) {
    var boxtype = boxes[i].gettype();
    var boxargs = boxes[i].getargs();
    var itercode = types[boxtype]['code'];
    var amtinlets = int(types[boxtype]['inlets']) + int(types[boxtype]['outlets']);
    for(var x = 0; x < amtinlets; x++) {
      var target = JSON.stringify([i, x]);
      for(var y=0; y<nodes.length; y++) {
             if( JSON.stringify(nodes[y]).includes(target)) break;
          }
          if(y >= nodes.length) {
              y = 0;
          }
      itercode = itercode.replace("i"+x, y)
    }
    for(var x = 0; x < boxargs.length; x++) {
    itercode = itercode.replace("a"+x, boxargs[x])
  }
    iterboxes[i] = itercode;
  }
  var setupnodes = " ground, nodes".replace('nodes', nodes.length);
  console.log(nodes);
  code = iterboxes.join("\n");

  fs.writeFileSync("./precompile.hcl", setupnodes + code);
  }



function generatesave(path) {
  if (path == undefined) {
  path = dialog.showSaveDialog();
  }
  var boxnames = [];
  for (var i = 0; i < boxes.length; i++) {

    boxnames.push([i, boxes[i].getname(), boxes[i].getposition()])
  }
  var conns = [];
  for (var i = 0; i < connections.length; i++) {
    conns.push(connections[i].getinlets())
  }
  var savefile = [boxnames, conns];
  fs.writeFileSync(path, JSON.stringify(savefile));

}

// basis werkt, gaat iets mis met inlets...

function loadfile(path) {
  if (path == undefined) {
  var path = dialog.showOpenDialog({ properties: ['openFile', 'multiSelections'] });
  }
  localStorage.setItem("load", path);
  location.reload(true);
}

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
