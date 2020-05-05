// Utility functions used everywhere


function dragElement(elmnt, sidebar) {
  var pos1 = 0,
    pos2 = 0,
    pos3 = 0,
    pos4 = 0;

  elmnt.onmousedown = dragMouseDown;

  var adjustBodyWidth = function(nPixels) {
    scrollitem.style.left = nPixels + 'px';
  };

  function dragMouseDown(e) {
    e = e || window.event;
    e.preventDefault();
    // get the mouse cursor position at startup:
    pos3 = e.clientX;
    pos4 = e.clientY;
    document.onmouseup = closeDragElement;
    // call a function whenever the cursor moves:
    document.addEventListener("mousemove", elementDrag);
  }

  function elementDrag(e) {
    e = e || window.event;
    e.preventDefault();
    // calculate the new cursor position:
    pos1 = pos3 - e.clientX;
    pos2 = pos4 - e.clientY;
    pos3 = e.clientX;
    pos4 = e.clientY;

    if (sidebar == undefined && blocked == false) {
      // Make sure the element stays selected
      ds.addSelection(elmnt);
      // Get all selected elements (for dragging multiple components at once with shift/ctrl)
      let elements = ds.getSelection();
      // Set the new position for all elements
      for (var i = 0; i < elements.length; i++) {
        elements[i].style.left = (elements[i].offsetLeft - pos1) + "px";
        elements[i].style.top = (elements[i].offsetTop - pos2) + "px";
        elements[i].x = (elements[i].offsetLeft - pos1);
        elements[i].y = (elements[i].offsetTop - pos2);
        // Make sure they don't fall off the screen !! MIGHT BE OPTIONAL NOW WE USE THAT INVISIBLE MARKER
        if (elements[i].x > window.innerWidth - sbarwidth - 30) {
          adjustBodyWidth(elmnt.x + sbarwidth + 100);
        }

      }
      for (var i = 0; i < connections.length; i++) {
        connections[i].update();
      }
    } else if (blocked == false) {
      // If we're dragging the sidebar, just set the horizontal position
      sbarwidth = window.innerWidth - (elmnt.offsetLeft - pos1);
      elmnt.style.right = sbarwidth - 30 + "px";
      sidebar.windowresize();
    }
    ds.break();

  }

  function closeDragElement() {
    // stop moving when mouse button is released:
    document.onmouseup = null;
    document.removeEventListener("mousemove", elementDrag);
  }
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
