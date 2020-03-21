// COMPONENT CLASS
// Represents 1 electrical component

// Change array order (used to make sure ground is at 0)
Array.prototype.move = function(from, to) {
  this.splice(to, 0, this.splice(from, 1)[0]);
};


function Component(name, xin = mouseX, yin = mouseY - 200) {
  let x, y, w, h; // Location and size
  let inp; // letiable for input field
  let inputting = false; // Check if we have an active input field
  let inlets = []; // Array to store inlets
  let valid = true; // Check if we entered a valid component name/arguments
  let args = []; // Arguments of our component ('200' in 'resistor 200')
  let optargs = []; // Optional arguments
  let type = name; // Name of our component ('resistor' in 'resistor 200')
  let _this = this;

  let uiChild;

  let instance = this;


  x = xin;
  y = yin;


  var divComponent = document.createElement("div");

  // Make a nicely styled box with padding around the text
  divComponent.className = "component";
  divComponent.style.position = "absolute";
  divComponent.style.height = "17px";
  divComponent.style.paddingLeft = "12px";
  divComponent.style.paddingTop = "3px";
  divComponent.style.paddingBottom = "1px";
  divComponent.style.paddingRight = "12px";
  divComponent.style.background = "#424242";
  divComponent.style.top = y + "px";
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
    if (connectingline !== undefined) {
      connectingline();
    }
    blocked = true;
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
    inp.focus(); // set focus to the input so the user can start typing
  }

  //Bunch of getters and setters to interact and get info from these boxes
  this.getinlets = function() {
    return inlets;
  }

  this.getindex = function() {
    return boxes.indexOf(instance);
  }

  this.gettype = function() {
    return type;
  }

  this.getdiv = function() {
    return divComponent;
  }

  this.isValid = function(val) {
    if(val !== undefined) {
      valid = val;
    }
    return valid;
  }

  this.getargs = function() {
    return args;
  }

  this.setargs = function(a) {
    optargs = a;
  }


  divComponent.setargs = function(a) {
    optargs = a;
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



  // When the name changes or the object is being created, display the new name and update the inlets
  this.changetype = function(oldtype) {
    if(uiChild !== undefined) {
      uiChild.delete();
    }
    if(type === 'comment') {
      divComponent.innerHTML = name.replace(type, "");
    }
    else if (type === 'slider-') {
      divComponent.innerHTML = '';
      sliders.push(divComponent);
      uiChild = new singleSlider(divComponent);
    }
    else {
    divComponent.innerHTML = name;
    }
    let newinlets, newoutlets, oldinlets, oldoutlets;

    newinlets = parseInt(types[type]['inlets']);
    newoutlets = parseInt(types[type]['outlets']);
    if(oldinlets !== undefined) {
    oldinlets = parseInt(types[oldtype]['inlets']);
    oldoutlets = parseInt(types[oldtype]['outlets']);
    }
    else {
      oldinlets = oldoutlets = 0
    }

    let diff = ((divComponent.clientWidth-24) / (newinlets-1 + (newinlets == 1)));
    let start = 8;

    for (let i = 0; i < newinlets; i++) {
      let datatype = 'analog';
      if (types[type]['datatypes'] !== undefined) {
        datatype = types[type]['datatypes'][i];
      }
      inlets[i] = new Inlet(start, -4, [this, i], 'inlet', datatype, types[type]['colors'][i])
      if(types[type]['tooltips'] !== undefined) {
        inlets[i].settooltip("Inlet " + i + ": " + types[type]['tooltips'][i]);
      }
      else {
        inlets[i].settooltip("Inlet " + i);
      }
      start += diff;
    }

    diff = ((divComponent.clientWidth-24) / (newoutlets-1 + (newoutlets == 1)));
    start = 8;

    for (let i = 0; i < newoutlets; i++) {
      let datatype = 'analog';
      if (types[type]['datatypes'] !== undefined) {
        datatype = types[type]['datatypes'][i + newinlets]
      }
        inlets[newinlets + i] = new Inlet(start, divComponent.clientHeight-4, [this, newinlets + i], 'outlet', datatype, types[type]['colors'][i + newinlets]);
        if(types[type]['tooltips'] !== undefined) {
          inlets[newinlets + i].settooltip("Outlet " + i + ": " + types[type]['tooltips'][newinlets + i]);
        }
        else {
          inlets[newinlets + i].settooltip("Outlet " + i);
        }
        start += diff;
    }

  }

  // Close the input and parse the name
  this.closeinput = function() {
    if (inputting == true) {
      inputting = false;
      blocked = false
      name = inp.value;
      inp.remove();
    }
    // Arguments
    args = name.replace("  ", " ").split(" ");
    // type of object
    let oldtype = type;
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
      if (type == 'ground' && boxes.indexOf(instance) != 0) {
        boxes.move(boxes.indexOf(instance), 0); // it is essential that ground is at 0
      }
      valid = true;
      divComponent.style.backgroundColor = '#424242'
      _this.changetype(oldtype);
    } else {
      divComponent.innerHTML = name;
      divComponent.style.backgroundColor = '#ff6961'

      for (let i = connections.length - 1; i >= 0; i--) {
        // count backwards to avoid messing up the order when removing
        if (JSON.stringify(connections[i].getinlets()[0]).includes(boxes.indexOf(instance)) || JSON.stringify(connections[i].getinlets()[1]).includes(boxes.indexOf(instance))) {
          connections[i].remove();
        }
        // Then remove inlets
        for (let i = 0; i < inlets.length; i++) {
          inlets[i].remove();
        }
        inlets = [];
      }
      valid = false;
    }
  }

  // Delete an inlet
  this.removeinlet = function(inlet) {
    inlets.splice(inlet, 1);
  }

  // Delete a whole box
  this.delete = function() {
    if(uiChild !== undefined) {
      uiChild.delete();
    }
    if(blocked == false) {
    // First remove connections
    for (let i = connections.length - 1; i >= 0; i--) {
      // count backwards to avoid messing up the order when removing
      if (connections[i].getinlets()[0].includes(boxes.indexOf(instance))) {
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
    boxes.splice(boxes.indexOf(instance), 1);
  }

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
  }
  else {
    this.closeinput();
  }
}
