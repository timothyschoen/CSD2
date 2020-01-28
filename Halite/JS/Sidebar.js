
function Sidebar() {
  var tab = 1;
  var textformat;

  var sbaroffset, sbardragging;

  let jackbutton = createButton('J');
  jackbutton.size(22, 22);
  jackbutton.style("border-radius:100%; border:none; outline:none; font-size:7; text-align:left; color:white; background-color:#229FD7;");
  jackbutton.id('jackbutton');
  jackbutton.mousePressed(function () {
    tab = 3;
  });
  jackbutton.position(cnvwidth+12, windowHeight/2-50);

  let consolebutton = createButton('Con');
  consolebutton.size(22, 22);
  consolebutton.style("border-radius:100%; border:none; outline:none; font-size:7; text-align:left; color:white; background-color:#229FD7;");
  consolebutton.id('consolebutton');
  consolebutton.mousePressed(function () {
    tab = 1;
  });
  consolebutton.position(cnvwidth+12, windowHeight/2);

  let codebutton = createButton('C++');
  codebutton.size(22, 22);
  codebutton.style("border-radius:100%; border:none; outline:none; font-size:7; text-align:left; color:white; background-color:#229FD7;");
  codebutton.id('consolebutton');
  codebutton.mousePressed(function () {
    tab = 2;
  });
  codebutton.position(cnvwidth+12, windowHeight/2+50);

  this.windowresize = function() {
    cnvwidth = windowWidth-sbarwidth;
    jackbutton.position(cnvwidth+12, windowHeight/2-50);
    consolebutton.position(cnvwidth+12, windowHeight/2);
    codebutton.position(cnvwidth+12, windowHeight/2+50);
  }

  this.draw = function() {
    if (mouseX > cnvwidth && mouseX < cnvwidth + 30 && draginstance == -1) {
      if(mouseIsPressed) {
      sbaroffset = cnvwidth - mouseX;
      sbardragging = true;
    }
    else if (sbardragging) {
      sbardragging = false;
    }
    }

    if (sbardragging) {
      //console.log('hey')
      //sbarwidth = windowWidth-mouseX-sbaroffset
      cnvwidth = mouseX + sbaroffset;
      sbarwidth = windowWidth - cnvwidth;
      textformat = new RegExp("(.{" + int((sbarwidth/9)-3).toString() + "})", "g");
      this.windowresize();
      windowResized();
    }

    stroke(150);
    line(cnvwidth, 0, cnvwidth, windowHeight-40);
    line(cnvwidth+30, 0, cnvwidth+30, windowHeight-40);
    //line(250, 0, 250, windowHeight);
    noStroke();
    textAlign(LEFT);
    textFont('monospace');


    switch(tab) {
    case 1:
      text(consol.replace(textformat, "$1\n").slice(consol.length-100, consol.length), cnvwidth+50, 10, cnvwidth+50, windowHeight-20)
      break;
    case 2:
      text(code.replace(textformat, "$1\n").slice(code.length-100, code.length), cnvwidth+50, 10, cnvwidth+50, windowHeight-20)
      break;

    }
    textFont("sans-serif");
  }
}

function yourCustomLog(msg) {
  var message = msg.toString();
  var bar = '-'.repeat((sbarwidth/10));
  consol = consol + "\n" + bar + '\n' + message + '\n';
  //consol = consol.slice(consol.length-200, consol.length);
}


window.console = {
  log : function(msg) {yourCustomLog(msg)},
  info : function(msg) {yourCustomLog(msg)},
  warn : function(msg) {yourCustomLog(msg)},
  //...
}
