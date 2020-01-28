function Inlet(x, y, instance, type, color = '#229FD7') {
  this.x = x;
  this.y = y

  var plusbutton = createButton("");
  plusbutton.size(8, 8);
  plusbutton.style("border-radius:100%; border:none; outline:none; font-size:15; color:white; background-color:colorcode ; border: 0 none transparent; padding:0; margin:0;".replace('colorcode', color));
  plusbutton.position(x, y);

  this.setposition = function(x, y) {
    this.x = x;
    this.y = y;
    plusbutton.position(this.x, this.y);
  }
  this.gettype = function() {
      return type;
  }

  this.getposition = function() {
    return [this.x-4, this.y-16];
  }
  this.remove = function() {
    plusbutton.remove();
  }
  plusbutton.mousePressed(function () {
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

function Connection(start, end) {
  this.getconnection = function () {
    return [start, end];
  }
  this.getinlets = function () {
    return [[boxes.indexOf(start[0]), boxes.indexOf(end[0])], [start[1], end[1]]];
  }

  this.remove = function() {
    connections.splice(connections.indexOf(this), 1);
  }
  this.draw = function() {
    var inlet1 = start[0].getinlets()[start[1]].getposition();
    var inlet2 = end[0].getinlets()[end[1]].getposition();

    var line1x = inlet1[0];
    var line1y = inlet1[1];
    var line2x = inlet2[0];
    var line2y = inlet2[1];

    if (pDistance(mouseX, mouseY, line1x, line1y, line2x, line2y) < 10) {
      stroke("#229FD7");
      if(mouseIsPressed) {
        if (mouseButton === RIGHT) {
          this.remove();
        }
    }
  }
    else {
      stroke(65);
    }
    line(line1x, line1y , line2x,line2y);
}
}

// Calculate distance to line
// For highlighing and deleting lines near your cursor


function pDistance(x, y, x1, y1, x2, y2) {

  var A = x - x1;
  var B = y - y1;
  var C = x2 - x1;
  var D = y2 - y1;

  var dot = A * C + B * D;
  var len_sq = C * C + D * D;
  var param = -1;
  if (len_sq != 0) //in case of 0 length line
      param = dot / len_sq;

  var xx, yy;

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

  var dx = x - xx;
  var dy = y - yy;
  return Math.sqrt(dx * dx + dy * dy);
}
