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
