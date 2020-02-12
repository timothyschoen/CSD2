// Object for individual inlets
// Also manages the creation of connections
function Inlet(x, y, instance, type, color = '#229FD7') {
  //make this public
  this.x = x;
  this.y = y

  // This is the actual inlet
  let inletbutton = createButton("");
  inletbutton.size(8, 8);
  inletbutton.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:colorcode ; border: 0 none transparent; padding:0; margin:0;".replace('colorcode', color));
  inletbutton.position(x, y);


  // basic getters and setters
  this.setposition = function(x, y) {
    this.x = x;
    this.y = y;
    inletbutton.position(this.x, this.y);
  }
  this.gettype = function() {
      return type;
  }

  this.getposition = function() {
    return [this.x-4, this.y-16];
  }

  // HTML buttons overlap any p5js draws, so we need to hide it manually when it's under the sidebar
  this.show = function(boolean) {
  if(boolean) {
    inletbutton.show();
  }
  else {
    inletbutton.hide();
  }
}

  // Delete inlet
  this.remove = function() {
    inletbutton.remove();
  }

  inletbutton.mousePressed(function () {
    if (connecting == -1) {
      connecting = instance;
    }
    else {
      if(connecting[0] != instance[0]) {
        connections.push(new Connection([instance[0] ,instance[1]], [connecting[0] ,connecting[1]]))
      }
      connecting = -1;
    }
  });
}

// Object for connections
function Connection(start, end) {

  // Getters and setters
  this.getconnection = function () {
    return [start, end];
  }
  this.getinlets = function () {
    return [[boxes.indexOf(start[0]), boxes.indexOf(end[0])], [start[1], end[1]]];
  }

  this.remove = function() {
    connections.splice(connections.indexOf(this), 1);
  }

  // Draw the line
  this.draw = function() {
    // Get inlet positions (TODO: just use index.x and index.y)
    let inlet1 = start[0].getinlets()[start[1]].getposition();
    let inlet2 = end[0].getinlets()[end[1]].getposition();

    // Get x,y of start and end of line
    let line1x = inlet1[0];
    let line1y = inlet1[1];
    let line2x = inlet2[0];
    let line2y = inlet2[1];

    // Distance of mouse to line, so we can highlight it when our mouse is close
    if (pDistance(mouseX, mouseY, line1x, line1y, line2x, line2y) < 10) {
      stroke("#229FD7"); // set selected color
      // If our mouse is close and we right click, delete this connection
      if(mouseIsPressed) {
        if (mouseButton === RIGHT) {
          this.remove();
        }
    }
  }
    else {
      stroke(65);
    }
    // Draw it!
    line(line1x, line1y , line2x,line2y);
}
}




// Calculate distance to line
// For highlighing and deleting lines near your cursor

function pDistance(x, y, x1, y1, x2, y2) {

  let A = x - x1;
  let B = y - y1;
  let C = x2 - x1;
  let D = y2 - y1;

  let dot = A * C + B * D;
  let len_sq = C * C + D * D;
  let param = -1;
  if (len_sq != 0) //in case of 0 length line
      param = dot / len_sq;

  let xx, yy;

  if (param < 0) {
    xx = x1;
    yy = y1;
  }
  else if (param > 1) {
    xx = x2;
    yy = y2;
  }
  else {
    xx = x1 + param * C;
    yy = y1 + param * D;
  }

  let dx = x - xx;
  let dy = y - yy;
  return Math.sqrt(dx * dx + dy * dy);
}
