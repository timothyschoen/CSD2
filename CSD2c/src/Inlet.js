let mouseX = 0;
let mouseY = 0;

let connectingline;

// Object for individual inlets
// Also manages the creation of connections
function Inlet(x, y, instance, type, datatype, color = '#229FD7') {
  //make this public
  this.x = x;
  this.y = y


  // This is the actual inlet
  var inletbutton = document.createElement("BUTTON");
  if (datatype == 'digital') {
    color = '#fcba03';
  }

  inletbutton.style.position = "absolute";
  inletbutton.style.height = "9px";
  inletbutton.style.width = "9px";
  inletbutton.style.padding = "0px";
  inletbutton.style.margin = "0px";
  inletbutton.style.borderRadius = '100%'
  inletbutton.style.border = '0px'
  inletbutton.style.zIndex = "-1";

  inletbutton.style.background = color;
  inletbutton.style.top = y + "px";
  inletbutton.style.left = x + "px";


  instance[0].getdiv().appendChild(inletbutton);

  // getters and setters
  this.setposition = function(x, y) {
    this.x = x;
    this.y = y;
  }
  this.gettype = function() {
    return type;
  }

  this.getdiv = function() {
    return inletbutton;
  }

  this.settooltip = function(tooltip) {
    inletbutton.title = tooltip;
  }


  this.getdatatype = function() {
    return datatype;
  }

  this.getposition = function() {
    return [this.x, this.y];
  }

  // Delete inlet
  this.remove = function() {
    inletbutton.remove();
  }


  inletbutton.onclick = function() {
    if (connectingline == undefined && blocked == false) {
      connecting = instance; // let everyone know who's connecting
      // Create a temporary line from the clicked inlet to the mouse
      let htmlLine = document.createElement("div");
      var off1 = getOffset(inletbutton); // inletbutton position
      var eventhandler = function(e) {
        dragToMouse(e, off1, htmlLine)
      } // we need to set this up so we have a non-anonymous function that can be passed parameters
      document.addEventListener('mousemove', eventhandler); // Track the mouse position whenever it moves
      connectingline = function() // Give everyone access to a function that will end the connection process
      {
        document.removeEventListener('mousemove', eventhandler); // stop tracking mouse
        htmlLine.parentNode.removeChild(htmlLine);
        connectingline = undefined;
      }
      document.body.appendChild(htmlLine);

    } else if (connecting == instance) {
      connectingline();
      connectingline = undefined;
      connecting = -1; // no one is connecting now
    } else if (blocked == false) {
      let sametype = instance[0].getinlets()[instance[1]].getdatatype() == connecting[0].getinlets()[connecting[1]].getdatatype();
      if (connecting[0] != instance[0] && sametype) {
        connectingline()
        connections.push(new Connection([instance[0], instance[1]], [connecting[0], connecting[1]], datatype))
      } else if (!sametype) {
        connectingline()
        console.warn("Cannot connect a digital inlet to an analog inlet!!");
      }
      connectingline = undefined;
      connecting = -1;
    }
  };

}

// Get position of html element, compensated for scrolling
function getOffset(el) {
  var rect = el.getBoundingClientRect();
  return {
    left: rect.left + window.pageXOffset,
    top: rect.top + window.pageYOffset,
    width: rect.width || el.offsetWidth,
    height: rect.height || el.offsetHeight
  };
}

// Get position of mouse during event, compensated for scrolling
function dragToMouse(e, off1, htmlLine) {
  updateLine(off1, {
    left: e.clientX + 5 + window.pageXOffset,
    top: e.clientY + 4 + window.pageYOffset,
    width: -1,
    height: -1
  }, htmlLine);
}


// Update position of HtmlLine when we drag boxes or move our mouse
function updateLine(off1, off2, htmlLine) {

  // bottom right
  var x1 = off1.left + off1.width - 2;
  var y1 = off1.top + off1.height;
  // top right
  var x2 = off2.left + off2.width - 2;
  var y2 = off2.top + off2.height;
  // distance
  var length = Math.sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
  // center
  var cx = ((x1 + x2) / 2) - (length / 2) - 4;
  var cy = ((y1 + y2) / 2) - (1 / 2) - 4;
  // angle
  var angle = Math.atan2((y1 - y2), (x1 - x2)) * (180 / Math.PI);


  htmlLine.style.position = "absolute";
  htmlLine.style.height = "1px";
  htmlLine.style.width = length + "px";
  htmlLine.style.padding = "0px";
  htmlLine.style.margin = "0px";
  htmlLine.style.borderStyle = "solid";
  htmlLine.style.borderWidth = "2px";
  htmlLine.style.color = 'transparent';
  htmlLine.style.borderColor = "#505050";
  htmlLine.style.borderRadius = "10%";
  htmlLine.style.lineHeight = '2px'
  htmlLine.style.transform = 'rotate(' + angle + 'deg)' // set the angle to our mouse
  htmlLine.style.zIndex = '-3'

  htmlLine.style.background = '#DCDCDC';
  htmlLine.style.top = cy + "px";
  htmlLine.style.left = cx + "px";
}

// Object for connections between components
function Connection(start, end, datatype = 'analog') // Optional argument 'datatype' provides legacy support for all the patches we made before adding digital signals
{

  // Create a new line once the connection is established
  let htmlLine = document.createElement("div");
  let off1 = getOffset(start[0].getinlets()[start[1]].getdiv());
  let off2 = getOffset(end[0].getinlets()[end[1]].getdiv());
  updateLine(off1, off2, htmlLine);

  // highlight color
  htmlLine.addEventListener('mouseenter', function() {
    htmlLine.style.backgroundColor = "#229FD7";
  });

  // remove on right-click
  htmlLine.addEventListener('contextmenu', ((ev) => {
    ev.preventDefault();
    this.remove();
    return false;
  }), false);

  // un-highlight
  htmlLine.addEventListener('mouseleave', function() {
    htmlLine.style.backgroundColor = "#DCDCDC";
  });

  document.body.appendChild(htmlLine);


  // Getters and setters
  this.getconnection = function() {
    return [start, end];
  }
  this.getinlets = function() {
    return [
      [boxes.indexOf(start[0]), boxes.indexOf(end[0])],
      [start[1], end[1]]
    ];
  }

  this.remove = function() {
    connections.splice(connections.indexOf(this), 1);
    htmlLine.parentNode.removeChild(htmlLine);
  }

  this.gettype = function() {
    return datatype;
  }

  this.update = function() {
    off1 = getOffset(start[0].getinlets()[start[1]].getdiv());
    off2 = getOffset(end[0].getinlets()[end[1]].getdiv());
    updateLine(off1, off2, htmlLine);
  }

}
