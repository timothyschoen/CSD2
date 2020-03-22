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
