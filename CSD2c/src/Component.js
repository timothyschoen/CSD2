// COMPONENT FUNCTION
// Represents 1 electrical component



function Component(name, xin = mouseX,  yin = mouseY-100) {
      let x, y, w, h;          // Location and size
      let inp;                 // letiable for input field
      let inputting = false;  // Check if we have an active input field
      let inlets = [];        // Array to store inlets
      let valid = true;       // Check if we entered a valid component name/arguments
      let args = [];          // Arguments of our component ('200' in 'resistor 200')
      let type = name;        // Name of our component ('resistor' in 'resistor 200')
      let selected = false;   // Is our component selected?

      let instance = this;
      let offsetX, offsetY;
      let dragging = false;         // Is the Component being dragged?
      let multipledragging = false; // Is another Component being dragged while we are selected?
      let rollover = false;         // color change on mouseOver


      x = xin;
      y = yin;

      // Dimensions
      w = 70;
      h = 20;

      // Open input field
    this.inputname = function(text) {
      inp = createInput(text);
      inputting = true;
      typing = true;
      inp.size(w-6, h-7);
      inp.position(x+10, y+23);
      inp.style('background-color: #efefef; font-size:11px; font:sans-serif; text-align: center; border:none; outline:none;');
    }

    //Bunch of getters and setters to interact and get info from these boxes

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
    this.getsize = function(){
      return [w, h];
    }
    this.select = function(){
      selected = true;
    }
    this.deselect = function(){
      selected = false;
    }


    // Select the box if there is a click on it
    this.mouseClicked = function(){
      if (intersect(mouseX, mouseX+1, mouseY, mouseY+1, x, x+w, y, y+h)) {
        selected = true;
      }
      // Only deselect when clicking outside the box if shift is not down
      // This allows us to select multiple objects when holding down shift!
      else if(!keyIsDown(SHIFT) && !selecting)  {
        selected = false;
      }
    }

    // Change positions of inlets and outlets
    this.updateInlets = function() {
        let offset = 0;
        for(let i = 0; i < inlets.length; i++) {
          if(inlets[i].gettype() == 'inlet') {
            inlets[i].setposition(x+15+(10*i), y+17)
            offset++;
          }
          else {
            inlets[i].setposition(x+15+(10*(i-offset)), y+36);
          }
          if(inlets[i].getposition()[0] > cnvwidth) {
            inlets[i].show(false);
          }
          else {
            inlets[i].show(true);
          }
        }
      }

  this.draw = function() {
    stroke(0);

    // Dragging functionality (TODO: make more efficient)
    // Is the mouse over the box?
    if(intersect(mouseX, mouseX+1, mouseY, mouseY+1, x, x+w, y, y+h)) {
      rollover = true;

      // Only start dragging when we are not inputting or making a selection box
      if(mouseIsPressed && !inputting && !dragging && !selecting) {
        if(!keyIsDown(SHIFT) && !selected) {
        for (let i = 0; i < boxes.length; i++) {
          boxes[i].deselect();
        }
      }
        dragging = true;
        // Make it public that this component is being dragged
        draginstance = boxes.indexOf(this)
        // Right-click delets the object
        if (mouseButton === RIGHT) {
          draginstance = -1;
          this.delete();
        }
        // Keep track of the relative location of the mouse from the point where we clicked
        offsetX = mouseX-x;
        offsetY = mouseY-y;
      }
      // If the mouse isn't pressed, we're not dragigng anymore
      else if (!mouseIsPressed && dragging) {
        dragging = false;
        draginstance = -1;
      }
    }

    // If we are not being dragging but some other component is and we are selected, move it along with that box!
    else if (draginstance != -1 && !dragging && selected && !inputting && !multipledragging) {
      offsetX = mouseX-x;
      offsetY = mouseY-y;
      multipledragging = true;
    }
    // If the other box is no longer being dragged, so are we
    else if (draginstance == -1 && multipledragging) {
      multipledragging = false;
    }
    // Close input when clicking out of the box, also make sure we don't accidentally open an input while dragging
    else if (mouseIsPressed && (inputting || selected)) {
      this.closeinput();
    }
    else {
      rollover = false;
    }


    // Finally, change colors and positions now we know what state it's in
    if (dragging || multipledragging) {
      x = mouseX - offsetX;
      y = mouseY - offsetY;
      fill (85);
    }
    else if(selected) {
      fill(110);
    }
    else if (rollover) {
      fill(135);
    }
    else {
      fill('#afafaf');
    }

    // If the object goes over the edge, resize our canvas
    if (x+w >= width) {
      resizeCanvas(x+w+w, height);

    }
    if (y >= height) {
      resizeCanvas(width, y+h+h);
    }
    // If we do not have a proper name/arguments, color the box red
    if (!valid) {
      fill('red');
    }
    // this could be better if we did it less often
    this.updateInlets();

    // Draw the box!
    rect(x, y, w, h);

    // Write text in it
    fill (0);
    noStroke();
    textAlign(CENTER);
    textSize(12);
    text(name, x+(w/2), y+(h/1.5));

    textAlign(LEFT);

    stroke(1)

  }

  // Input name in double click
  this.doubleclick = function() {
    if ((mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h) && !inputting){
    this.inputname(name);
      }
    }

    // When the name changes or the object is being created, add the inlets
  this.changetype = function() {
    for(let i = 0; i < inlets.length; i++) {
      inlets[i].remove();
    }
    inlets = [];

    for(let i = 0; i < int(types[type]['inlets']); i++) {
      inlets.push(new Inlet(x+15+(10*i), y+17, [this, i], 'inlet', types[type]['colors'][i]));
    }
    for(let i = 0; i < int(types[type]['outlets']); i++) {
      inlets.push(new Inlet(x+15+(10*i), y+36, [this, types[type]['inlets']+i], 'outlet', types[type]['colors'][i+types[type]['inlets']]));
    }
  }

  // Close the input and parse the name
  this.closeinput = function() {
    if (inputting == true) {
      inputting = false;
      typing = false;
      name = inp.value();
      inp.remove();
    }
    // Arguments
    args = name.split(" ");
    // type of object
    type = args.shift();
    // Check if type is existing
      if (type in types && types[type]['args'] == args.length) {
        // Make sure ground is at 0
        if(type == 'ground' && boxes.indexOf(this) != 0) {
          boxes.move(boxes.indexOf(this), 0);
        }
        valid = true;
        this.changetype();
        /*
        if(!initializing) {
        changed() // Update our code
      } */
      }
      else {
        valid = false;
      }
      // Resize width according to length of name
      w = ((name.length)*6)+15

  }

  // Delete an inlet
  this.removeinlet = function(inlet) {
    inlets.splice(inlet, 1);
  }

  // Delete a whole box
  this.delete = function() {
    // First remove connections
    for(let i = connections.length-1; i >= 0 ; i--) {
      // count backwards to avoid messing up the order when removing
      if(connections[i].getinlets()[0].includes(boxes.indexOf(this))) {
        connections[i].remove();
      }
    }
    // Then remove inlets
    for(let i = 0; i < inlets.length; i++) {
      inlets[i].remove();
    }
    inlets = [];

    // Then remove the box
    boxes.splice(boxes.indexOf(this), 1);

  }

  // function to run on backspace or delete
  // If this box is selected, delete it
  this.deleteIfSelected = function() {
    if (selected) {
      this.delete();
    }
  }


// If this object has no name, open an input!
  if (name  == undefined) {
    this.inputname("New Object");
  }
  else {
    this.closeinput();
  }
}
