let fs = require("fs");
const os = require('os');

let sbarwidth;

let home = os.homedir() + "/Documents/Circuitry";

if (!fs.existsSync(home)) {
    fs.mkdirSync(home);
    fs.mkdirSync(home + "/Saves");
    fs.mkdirSync(home + "/Saves/Examples");
    fs.mkdirSync(home + "/Outputs");
    fs.mkdirSync(home + "/Media");


    fs.copyFile(__dirname + '/../media/Solo.wav', home + '/Media/Solo.wav', (err) => {
  if (err) throw err;
  });

  fs.copyFile(__dirname + '/../media/sample-44k.wav', home + '/Media/sample-44k.wav', (err) => {
  if (err) throw err;
  });

  fs.copyFile(__dirname + '/../saves/examples/Midisynth.ncl', home + '/Saves/Examples/Midisynth.ncl', (err) => {
  if (err) throw err;
  });


}


let scrollitem = document.createElement("div");
scrollitem.innerHTML = '!!';
scrollitem.style.position = "absolute";
scrollitem.style.height = "8px";
scrollitem.style.width = "8px";
document.body.appendChild(scrollitem);


let ds = new DragSelect({});


function dragElement(elmnt, sidebar)
{
    var pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;

    elmnt.onmousedown = dragMouseDown;

    var adjustBodyWidth = function(nPixels) {
      scrollitem.style.left = nPixels + 'px';
    };

    function dragMouseDown(e)
    {
        e = e || window.event;
        e.preventDefault();
        // get the mouse cursor position at startup:
        pos3 = e.clientX;
        pos4 = e.clientY;
        document.onmouseup = closeDragElement;
        // call a function whenever the cursor moves:
        document.onmousemove = elementDrag;
    }

    function elementDrag(e)
    {
        e = e || window.event;
        e.preventDefault();
        // calculate the new cursor position:
        pos1 = pos3 - e.clientX;
        pos2 = pos4 - e.clientY;
        pos3 = e.clientX;
        pos4 = e.clientY;


        // set the element's new position:
        //elmnt.style.right = "0px";

        if(sidebar == undefined)
        {
            ds.addSelection(elmnt);
            let elements = ds.getSelection();
            for (var i = 0; i < elements.length; i++) {
              elements[i].style.left = (elements[i].offsetLeft - pos1) + "px";
              elements[i].style.top = (elements[i].offsetTop - pos2) + "px";
              elements[i].x = (elements[i].offsetLeft - pos1);
              elements[i].y = (elements[i].offsetTop - pos2);
              if(elements[i].x > window.innerWidth-sbarwidth-30) {
                adjustBodyWidth(elmnt.x + sbarwidth + 100);
            }

          }
            for (var i = 0; i < connections.length; i++)
            {
                connections[i].update();
            }
        }
        else
        {
            sbarwidth = window.innerWidth - (elmnt.offsetLeft - pos1);
            elmnt.style.right = sbarwidth + "px";

            sidebar.windowresize();
        }
        ds.break();

    }

    function closeDragElement()
    {
        // stop moving when mouse button is released:
        document.onmouseup = null;
        document.onmousemove = null;
    }
}





let mouseX = 0;
let mouseY = 0;

let connectingline;

onmousemove = function(e)
{
    mouseX = e.clientX;
    mouseY = e.clientY;
    if(connectingline != undefined)
    {
        connectingline(false);
    }
}

// Object for individual inlets
// Also manages the creation of connections
function Inlet(x, y, instance, type, datatype, color = '#229FD7')
{
    //make this public
    this.x = x;
    this.y = y


             // This is the actual inlet

             var inletbutton = document.createElement("BUTTON");
    if(datatype == 'digital')
    {
        color = '#fcba03';
    }

    inletbutton.style.position = "absolute";
    inletbutton.style.height = "8px";
    inletbutton.style.width = "8px";
    inletbutton.style.padding = "0";
    inletbutton.style.margin = "0";
    inletbutton.style.borderRadius = '100%'
    inletbutton.style.border = '0px'
    inletbutton.style.zIndex = "-1";

    inletbutton.style.background = color;
    inletbutton.style.top =  y + "px";
    inletbutton.style.left = x + "px";

    //inletbutton.style("border-radius:100%; border:none; position:absolute; outline:none; font-size:15; color:white; background-color:colorcode ; border: 0 none transparent; padding:0; margin:0;".replace('colorcode', color));

    //inletbutton.position(x, y);

    instance[0].getdiv().appendChild(inletbutton);

    // basic getters and setters
    this.setposition = function(x, y)
    {
        this.x = x;
        this.y = y;
        //inletbutton.style.top =  y + "px";
        //inletbutton.style.left = x + "px";
    }
    this.gettype = function()
    {
        return type;
    }

    this.getdiv = function()
    {
        return inletbutton;
    }

    this.getdatatype = function()
    {
        return datatype;
    }

    this.getposition = function()
    {
        return [this.x, this.y];
    }

    // HTML buttons overlap any p5js draws, so we need to hide it manually when it's under the sidebar
    this.show = function(boolean)
    {
        if(boolean)
        {
            //inletbutton.show();
        }
        else
        {
            //inletbutton.hide();
        }
    }
    // Delete inlet
    this.remove = function()
    {
        inletbutton.remove();
    }


    inletbutton.onclick = function ()
    {
        if (connectingline == undefined)
        {
            connecting = instance;
            let htmlLine = document.createElement("div");
            connectingline = function(del)
            {
                if(!del)
                {
                    var off1 = getOffset(inletbutton);
                    var off2 = getmousePos();
                    updateLine(off1, off2, htmlLine);
                }
                else
                {
                    htmlLine.parentNode.removeChild(htmlLine);
                    connectingline = undefined;
                }
            }
            document.body.appendChild(htmlLine);

        }
        else if (connecting == instance)
        {
            connectingline(true);
            connectingline = undefined;
            connecting = -1;
        }
        else
        {
            let sametype = instance[0].getinlets()[instance[1]].getdatatype() == connecting[0].getinlets()[connecting[1]].getdatatype();
            if(connecting[0] != instance[0] && sametype)
            {
                connectingline(true)
                connections.push(new Connection([instance[0],instance[1]], [connecting[0],connecting[1]], datatype))
            }
            else if(!sametype)
            {
                connectingline(true)
                console.warn("Cannot connect a digital inlet to an analog inlet!!");
            }
            connectingline = undefined;
            connecting = -1;
        }
    };

}

function getOffset( el )
{
    var rect = el.getBoundingClientRect();
    return {
left:
        rect.left + window.pageXOffset,
top:
        rect.top + window.pageYOffset,
width:
        rect.width || el.offsetWidth,
height:
        rect.height || el.offsetHeight
    };
}

function getmousePos()
{
    return {
left:
        mouseX+5 + window.pageXOffset,
top:
        mouseY+4 + window.pageYOffset,
width:
        -1,
    height:
            -1
        };
}

function updateLine(off1, off2, htmlLine)
{

    // bottom right
    var x1 = off1.left + off1.width - 2;
    var y1 = off1.top + off1.height;
    // top right
    var x2 = off2.left + off2.width - 2;
    var y2 = off2.top + off2.height;
    // distance
    var length = Math.sqrt(((x2-x1) * (x2-x1)) + ((y2-y1) * (y2-y1)));
    // center
    var cx = ((x1 + x2) / 2) - (length / 2)-4;
    var cy = ((y1 + y2) / 2) - (1 / 2)-4;
    // angle
    var angle = Math.atan2((y1-y2),(x1-x2))*(180/Math.PI);
    // make hr
    //var htmlLine = "<div style='padding:0px; margin:0px; height:" + 1 + "px; background-color:" + color + "; line-height:1px; position:absolute; left:" + cx + "px; top:" + cy + "px; width:" + length + "px; -moz-transform:rotate(" + angle + "deg); -webkit-transform:rotate(" + angle + "deg); -o-transform:rotate(" + angle + "deg); -ms-transform:rotate(" + angle + "deg); transform:rotate(" + angle + "deg);' />";

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
                                htmlLine.style.transform = 'rotate(' + angle + 'deg)'
                                        htmlLine.style.zIndex = '-3'

                                                //htmlline.style += '-webkit-transform:rotate(" + angle + "deg)';

                                                htmlLine.style.background = '#DCDCDC';
    htmlLine.style.top =  cy + "px";
    htmlLine.style.left = cx + "px";
}

// Object for connections
function Connection(start, end, datatype = 'analog')   // Optional argument 'datatype' provides legacy support for all the patches we made before adding digital signals
{

    let htmlLine = document.createElement("div");
    let off1 = getOffset(start[0].getinlets()[start[1]].getdiv());
    let off2 = getOffset(end[0].getinlets()[end[1]].getdiv());
    updateLine(off1, off2, htmlLine);

    htmlLine.addEventListener('mouseenter', function()
    {
        htmlLine.style.backgroundColor = "#229FD7";
    });

    htmlLine.addEventListener('contextmenu', ((ev) =>
    {
        ev.preventDefault();
        this.remove();
        return false;
    }), false);

    htmlLine.addEventListener('mouseleave', function()
    {
        htmlLine.style.backgroundColor = "#DCDCDC";
    });

    document.body.appendChild(htmlLine);


    // Getters and setters
    this.getconnection = function ()
    {
        return [start, end];
    }
    this.getinlets = function ()
    {
        return [[boxes.indexOf(start[0]), boxes.indexOf(end[0])], [start[1], end[1]]];
    }

    this.remove = function()
    {
        connections.splice(connections.indexOf(this), 1);
        htmlLine.parentNode.removeChild(htmlLine);
    }

    this.gettype = function()
    {
        return datatype;
    }

    this.update = function()
    {
        off1 = getOffset(start[0].getinlets()[start[1]].getdiv());
        off2 = getOffset(end[0].getinlets()[end[1]].getdiv());
        updateLine(off1, off2, htmlLine);
    }

}
