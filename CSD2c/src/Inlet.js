// Object for individual inlets
// Also manages the creation of connections
function Inlet(x, y, instance, type, datatype, color = '#229FD7') {
    //make this public
    this.x = x;
    this.y = y


             // This is the actual inlet

    var inletbutton = document.createElement("BUTTON");
             //let inletbutton = createButton("");
    //inletbutton.size(8, 8);


    if(datatype == 'digital') {
        color = '#fcba03';
    }

    inletbutton.style.position = "absolute";
    inletbutton.style.height = "8px";
    inletbutton.style.width = "8px";
    inletbutton.style.padding = "0";
    inletbutton.style.margin = "0";
    inletbutton.style.borderRadius = '100%'
    inletbutton.style.border = '0px'

    inletbutton.style.background = color;
    inletbutton.style.top =  y + "px";
    inletbutton.style.left = x + "px";

    //inletbutton.style("border-radius:100%; border:none; position:absolute; outline:none; font-size:15; color:white; background-color:colorcode ; border: 0 none transparent; padding:0; margin:0;".replace('colorcode', color));

    //inletbutton.position(x, y);

    document.body.appendChild(inletbutton);

    // basic getters and setters
    this.setposition = function(x, y) {
        this.x = x;
        this.y = y;
        inletbutton.style.top =  y + "px";
        inletbutton.style.left = x + "px";
    }
    this.gettype = function() {
        return type;
    }

    this.getdiv = function() {
        return inletbutton;
    }

    this.getdatatype = function() {
        return datatype;
    }

    this.getposition = function() {
        return [this.x-4, this.y-16];
    }

    // HTML buttons overlap any p5js draws, so we need to hide it manually when it's under the sidebar
    this.show = function(boolean) {
        if(boolean) {
            //inletbutton.show();
        }
        else {
            //inletbutton.hide();
        }
    }
    // Delete inlet
    this.remove = function() {
        inletbutton.remove();
    }

    /*

    inletbutton.mousePressed(function () {
        if (connecting == -1) {
            connecting = instance;
        }
        else {
            let sametype = instance[0].getinlets()[instance[1]].getdatatype() == connecting[0].getinlets()[connecting[1]].getdatatype();
            if(connecting[0] != instance[0] && sametype) {
                connections.push(new Connection([instance[0],instance[1]], [connecting[0],connecting[1]], datatype))
            }
            else if(!sametype) {
                console.warn("Cannot connect a digital inlet to an analog inlet!!");
            }
            connecting = -1;
        }
    }); */
}

// Object for connections
function Connection(start, end, datatype = 'analog') { // Optional argument 'datatype' provides legacy support for all the patches we made before adding digital signals

  function getOffset( el ) {
      var rect = el.getBoundingClientRect();
      return {
          left: rect.left + window.pageXOffset,
          top: rect.top + window.pageYOffset,
          width: rect.width || el.offsetWidth,
          height: rect.height || el.offsetHeight
      };
  }



      var off1 = getOffset(start[0].getinlets()[start[1]].getdiv());
      var off2 = getOffset(end[0].getinlets()[end[1]].getdiv());

      // bottom right
      var x1 = off1.left + off1.width;
      var y1 = off1.top + off1.height;
      // top right
      var x2 = off2.left + off2.width;
      var y2 = off2.top;
      // distance
      var length = Math.sqrt(((x2-x1) * (x2-x1)) + ((y2-y1) * (y2-y1)));
      // center
      var cx = ((x1 + x2) / 2) - (length / 2);
      var cy = ((y1 + y2) / 2) - (1 / 2);
      // angle
      var angle = Math.atan2((y1-y2),(x1-x2))*(180/Math.PI);
      // make hr
      //var htmlLine = "<div style='padding:0px; margin:0px; height:" + 1 + "px; background-color:" + color + "; line-height:1px; position:absolute; left:" + cx + "px; top:" + cy + "px; width:" + length + "px; -moz-transform:rotate(" + angle + "deg); -webkit-transform:rotate(" + angle + "deg); -o-transform:rotate(" + angle + "deg); -ms-transform:rotate(" + angle + "deg); transform:rotate(" + angle + "deg);' />";
      var htmlLine = document.createElement("div");

      htmlLine.style.position = "absolute";
      htmlLine.style.height = "1px";
      htmlLine.style.width = length + "px";
      htmlLine.style.padding = "0px";
      htmlLine.style.margin = "0px";
      htmlLine.style.border = '1px'
      htmlLine.style.lineHeight = '1px'
      htmlLine.style.transform = 'rotate(' + angle + 'deg)'

      //htmlline.style += '-webkit-transform:rotate(" + angle + "deg)';

      htmlLine.style.background = 'black';
      htmlLine.style.top =  cy + "px";
      htmlLine.style.left = cx + "px";

      //
      // alert(htmlLine);
      document.body.appendChild(htmlLine);


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

    this.gettype = function() {
        return datatype;
    }

    this.update = function() {
      var off1 = getOffset(start[0].getinlets()[start[1]].getdiv());
      var off2 = getOffset(end[0].getinlets()[end[1]].getdiv());

      // bottom right
      var x1 = off1.left + off1.width;
      var y1 = off1.top + off1.height;
      // top right
      var x2 = off2.left + off2.width;
      var y2 = off2.top;
      // distance
      var length = Math.sqrt(((x2-x1) * (x2-x1)) + ((y2-y1) * (y2-y1)));
      // center
      var cx = ((x1 + x2) / 2) - (length / 2);
      var cy = ((y1 + y2) / 2) - (1 / 2);
      // angle
      var angle = Math.atan2((y1-y2),(x1-x2))*(180/Math.PI);


      htmlLine.style.width = length + "px";
      htmlLine.style.transform = 'rotate(' + angle + 'deg)'
      htmlLine.style.top =  cy + "px";
      htmlLine.style.left = cx + "px";

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
            if(datatype == 'analog') {
                stroke(0);
            }
            else {
                stroke(130);
            }
        }
        // Draw it!
        //line(line1x, line1y, line2x,line2y);
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
