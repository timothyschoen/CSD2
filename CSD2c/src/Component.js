// COMPONENT CLASS
// Represents 1 electrical component

let connections = []; // Where we store our components
let boxes = []; // Where we store our connections
let connecting = -1; // Is any inlet currently in the connecting state?

// Change array order (used to make sure ground is at 0)
Array.prototype.move = function(from, to) {
  this.splice(to, 0, this.splice(from, 1)[0]);
};


function Component(name, xin = mouseX, yin = mouseY - 100) {
  let x, y, w, h; // Location and size
  let inp; // letiable for input field
  let inputting = false; // Check if we have an active input field
  let inlets = []; // Array to store inlets
  let valid = true; // Check if we entered a valid component name/arguments
  let args = []; // Arguments of our component ('200' in 'resistor 200')
  let optargs = []; // Optional arguments
  let type = name; // Name of our component ('resistor' in 'resistor 200')
  let selected = false; // Is our component selected?
  let _this = this;

  let instance = this;
  let offsetX, offsetY;
  let dragging = false; // Is the Component being dragged?
  let multipledragging = false; // Is another Component being dragged while we are selected?
  let rollover = false; // color change on mouseOver


  x = xin;
  y = yin;

  // Dimensions
  w = 70;
  h = 20;

  var divComponent = document.createElement("div");

  // Make a nicely styled box with padding around the text
  divComponent.className = "component";
  divComponent.style.position = "absolute";
  divComponent.style.height = "17px";
  divComponent.style.paddingLeft = "12px";
  divComponent.style.paddingTop = "2px";
  divComponent.style.paddingBottom = "0px";
  divComponent.style.paddingRight = "12px";
  divComponent.style.background = "#424242";
  divComponent.style.top = y + h + "px";
  divComponent.style.left = x + "px";
  divComponent.style.color = "#DCDCDC";
  divComponent.style.border = "1px solid #DCDCDC"
  divComponent.style.fontFamily = "sans-serif";
  divComponent.style.fontSize = "12px";
  divComponent.style.fontAlign = "center";
  divComponent.style.zIndex = '-2'
  divComponent.x = x
  divComponent.y = y
  divComponent.innerHTML = name;

  // Delete on right click
  divComponent.addEventListener('contextmenu', ((ev) => {
    ev.preventDefault();
    this.delete();
    return false;
  }), false);

  // Input name after doubleclick
  divComponent.addEventListener('dblclick', ((ev) => {
    ev.preventDefault();
    this.inputname(name);
    return false;
  }), false);

  document.body.appendChild(divComponent);

  // Make it draggable and selectable
  dragElement(divComponent);
  ds.addSelectables(divComponent);



  // Open input field
  this.inputname = function(text) {
    inp = document.createElement("INPUT");
    inp.value = text;
    inputting = true;
    typing = true;
    // Display it over the bounds of the component
    var bound = divComponent.getBoundingClientRect();
    inp.style.cssText = 'background-color: #efefef; font-size:11px; font:sans-serif; text-align: center; border:none; outline:none;';
    inp.style.height = divComponent.clientHeight + 'px';
    inp.style.width = divComponent.clientWidth + 'px';
    inp.style.top = bound.top + window.scrollY + 'px';
    inp.style.left = bound.left + window.scrollX + 'px';
    inp.style.position = 'absolute';
    inp.addEventListener("focusout", this.closeinput);
    document.body.appendChild(inp);
  }

  //Bunch of getters and setters to interact and get info from these boxes
  this.getinlets = function() {
    return inlets;
  }

  this.getindex = function() {
    return boxes.indexOf(this);
  }

  this.gettype = function() {
    return type;
  }

  this.getdiv = function() {
    return divComponent;
  }

  this.getargs = function() {
    return args;
  }

  this.getoptargs = function() {
    return optargs;
  }

  this.getname = function() {
    return name;
  }

  this.getposition = function() {
    return [divComponent.x, divComponent.y];
  }
  this.getsize = function() {
    return [w, h];
  }
  this.select = function() {
    selected = true;
  }
  this.deselect = function() {
    selected = false;
  }


  // Select the box if there is a click on it
  this.mouseClicked = function() {
    if (intersect(mouseX, mouseX + 1, mouseY, mouseY + 1, x, x + w, y, y + h)) {
      selected = true;
    }
    // Only deselect when clicking outside the box if shift is not down
    // This allows us to select multiple objects when holding down shift!
    else if (!keyIsDown(SHIFT) && !selecting) {
      selected = false;
    }
  }

  // Change positions of inlets and outlets
  this.updateInlets = function() {
    let offset = 0;
    for (let i = 0; i < inlets.length; i++) {
      if (inlets[i].gettype() == 'inlet') {
        inlets[i].setposition(x + 15 + (10 * i), y + 17)
        offset++;
      } else {
        inlets[i].setposition(x + 15 + (10 * (i - offset)), y + 36);
      }
      if (inlets[i].getposition()[0] > window.innerWidth - sbarwidth) {
        inlets[i].show(false);
      } else {
        inlets[i].show(true);
      }
    }
  }


  // When the name changes or the object is being created, add the inlets
  this.changetype = function() {
    divComponent.innerHTML = name;
    for (let i = 0; i < inlets.length; i++) {
      inlets[i].remove();
    }
    inlets = [];

    for (let i = 0; i < parseInt(types[type]['inlets']); i++) {
      let datatype = 'analog';
      if (types[type]['datatypes'] !== undefined) {
        datatype = types[type]['datatypes'][i]
      }
      inlets.push(new Inlet((11 * i) + 6, -4, [this, i], 'inlet', datatype, types[type]['colors'][i]));
    }
    for (let i = 0; i < parseInt(types[type]['outlets']); i++) {
      let datatype = 'analog';
      if (types[type]['datatypes'] !== undefined) {
        datatype = types[type]['datatypes'][i + types[type]['inlets']]
      }
      inlets.push(new Inlet((11 * i) + 6, 16, [this, types[type]['inlets'] + i], 'outlet', datatype, types[type]['colors'][i + types[type]['inlets']]));
    }
  }

  // Close the input and parse the name
  this.closeinput = function() {
    if (inputting == true) {
      inputting = false;
      typing = false;
      name = inp.value;
      inp.remove();
    }
    // Arguments
    args = name.replace("  ", " ").split(" ");
    console.log(args);
    // type of object
    type = args.shift();
    // Check if type is existing
    if (type in types && types[type]['args'] <= args.length) {
      // Make sure ground is at 0
      if (types[type]['args'] != args.length) {
        optargs = args.splice(types[type]['args'], args.length)
      }
      for (let i = 0; i < optargs.length; i++) {
        optargs[i] = parseFloat(optargs[i])
      }
      if (type == 'ground' && boxes.indexOf(this) != 0) {
        boxes.move(boxes.indexOf(this), 0);
      }
      valid = true;
      divComponent.style.backgroundColor = '#424242'
      _this.changetype();
    } else {
      divComponent.innerHTML = name;
      divComponent.style.backgroundColor = '#ff6961'
      valid = false;
    }
    // Resize width according to length of name

  }

  // Delete an inlet
  this.removeinlet = function(inlet) {
    inlets.splice(inlet, 1);
  }

  // Delete a whole box
  this.delete = function() {
    // First remove connections
    for (let i = connections.length - 1; i >= 0; i--) {
      // count backwards to avoid messing up the order when removing
      if (connections[i].getinlets()[0].includes(boxes.indexOf(this))) {
        connections[i].remove();
      }
    }
    // Then remove inlets
    for (let i = 0; i < inlets.length; i++) {
      inlets[i].remove();
    }
    inlets = [];
    divComponent.parentNode.removeChild(divComponent);

    // Then remove the box
    boxes.splice(boxes.indexOf(this), 1);

  }

  // Check if box is currently selected
  this.isSelected = function() {
    let selection = ds.getSelection();
    let selected = false;
    for (var i = 0; i < selection.length; i++) {
      if (selection[i].isEqualNode(divComponent)) {
        selected = true;
        break
      }
    }
    return selected
  }


  // function to run on backspace or delete
  // If this box is selected, delete it
  this.deleteIfSelected = function() {
    if (this.isSelected()) {
      this.delete();
    }
  }

  // If this object has no name, open an input!
  if (name == undefined) {
    this.inputname("New Object");
  } else {
    this.closeinput();
  }
}
