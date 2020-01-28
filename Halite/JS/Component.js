// COMPONENT FUNCTION
// Represents 1 electrical component

function Component(name, xin = mouseX,  yin = mouseY-100) {
      var x, y, w, h;          // Location and size
      var inp;
      var inputting = false;
      let inlets = [];
      let valid = true;
      let args = [];
      let type = name;

      var instance = this;
      var offsetX, offsetY;
      var dragging = false; // Is the Component being dragged?
      var rollover = false;


      x = xin;
      y = yin;
      // Dimensions
      w = 70;
      h = 20;


      this.inputname = function(text) {
        inp = createInput(text);
        inputting = true;
        inp.size(w-6, h-7);
        inp.position(x+10, y+23);
        inp.style('background-color: #efefef; font-size:11px; font:sans-serif; text-align: center; border:none; outline:none;');
      }


    this.getinlets = function(){
      return inlets;
    }

    this.getindex = function(){
      return boxes.indexOf(this);
    }

    this.gettype = function(){
      return type;
    }

    this.getargs = function(){
      return args;
    }
    this.getname = function(){
      return name;
    }

    this.getposition = function(){
      return [x, y];
    }

  this.draw = function() {
    stroke(0);


    // Basic dragging functionality
    if (mouseX > x+(w*0.3) && mouseX < x + w - (w*0.3) && mouseX < cnvwidth && mouseY > y && mouseY < y + h) {
      rollover = true;

      if(mouseIsPressed && !inputting && (draginstance == -1 || draginstance == boxes.indexOf(this))) {
        dragging = true;
        draginstance = boxes.indexOf(this)
        if (mouseButton === RIGHT) {
          draginstance = -1;
          this.delete();
        }
        // If so, keep track of relative location of click to corner of rectangle
        offsetX = x-mouseX;
        offsetY = y-mouseY;
      }
      else if (dragging) {
        dragging = false;
        draginstance = -1;
      }
    }
    else {
      rollover = false;
    }

    if (dragging) {
      x = mouseX + offsetX;
      y = mouseY + offsetY;
      var offset = 0;
      for(var i = 0; i < inlets.length; i++) {
        if(inlets[i].gettype() == 'inlet') {
        inlets[i].setposition(x+15+(10*i), y+17)
        offset++;
      }
      else {
        inlets[i].setposition(x+15+(10*(i-offset)), y+36);
      }
      }
      fill (50);
    } else if (rollover) {
      fill(100);
    } else {
      fill('#afafaf');
    }

    if (x+w >= cnvwidth || x < 0 || y > windowHeight-h-h || y < 0) {
      x = Math.max(x, 0);
      x = Math.min(x, cnvwidth-w);
      y = Math.max(y, 0);
      y = Math.min(y, windowHeight-h-h);
      for(var i = 0; i < inlets.length; i++) {
        if(inlets[i].gettype() == 'inlet') {
        inlets[i].setposition(x+15+(10*i), y+17)
        offset++;
      }
      else {
        inlets[i].setposition(x+15+(10*(i-offset)), y+36);
      }
    }
  }

    if (!valid) {
      fill('red');
    }

    rect(x, y, w, h);


    fill (0);
    noStroke();
    textAlign(CENTER);
    textSize(12);
    text(name, x+(w/2), y+(h/1.5));

    textAlign(LEFT);

    stroke(1)

  }



  this.doubleclick = function() {
    if ((mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h) && !inputting){
    this.inputname(name);
      }
    }

    this.changetype = function() {

      for(var i = 0; i < inlets.length; i++) {
        inlets[i].remove();
      }
      inlets = [];

      for(var i = 0; i < int(types[type]['inlets']); i++) {
        inlets.push(new Inlet(x+15+(10*i), y+17, [this, i], 'inlet', types[type]['colors'][i]));
      }
      for(var i = 0; i < int(types[type]['outlets']); i++) {
        inlets.push(new Inlet(x+15+(10*i), y+36, [this, types[type]['inlets']+i], 'outlet', types[type]['colors'][i+types[type]['inlets']]));
      }
    }

  this.closeinput = function() {
    if (inputting == true) {
      inputting = false;
      name = inp.value();
      inp.remove();
    }
    args = name.split(" ");
    type = args.shift();
      if (type in types && types[type]['args'] == args.length) {
        if(type == 'ground' && boxes.indexOf(this) != 0) {
          boxes.move(boxes.indexOf(this), 0);
        }
        valid = true;
        this.changetype();
      }
      else {
        valid = false;
      }
      w = ((name.length)*6)+15

  }

  this.removeinlet = function(inlet) {
    inlets.splice(inlet, 1);
  }

  this.delete = function() {
    for(var i = connections.length-1; i >= 0 ; i--) {
      // count backwards to avoid messing up the order when removing
      if(connections[i].getinlets()[0].includes(boxes.indexOf(this))) {
        connections[i].remove();
      }
    }
    for(var i = 0; i < inlets.length; i++) {
      inlets[i].remove();
    }
    inlets = [];

    boxes.splice(boxes.indexOf(this), 1);

  }

  if (name  == undefined) {
    this.inputname("New Object");
  }
  else {
    this.closeinput();
  }
}
