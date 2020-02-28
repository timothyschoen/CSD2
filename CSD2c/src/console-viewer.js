/**
 * Displays logs and Javascript errors in an overlay on top of your site. Useful for mobile webdevelopment.
 *
 * <https://github.com/markknol/console-log-viewer>
 * @author Mark Knol <http://blog.stroep.nl>
 */
var ConsoleLogViewer = (function() {
    ConsoleLogViewer.ALIGNMENT = "top"; // top | bottom
    ConsoleLogViewer.IS_MINIMIZED = false; // true | false
    ConsoleLogViewer.LOG_ENABLED = true;
    ConsoleLogViewer.IS_CLOSED = false;
    ConsoleLogViewer.TOTAL = 15;

    var _items = [];

    function ConsoleLogViewer()
    {
        var self = this;
        try
        {
            self.addCSS();
            self.addDivs(self);
            self.overwrite();
            self.applyCustomSettings();
        }
        catch(e)
        {
            setTimeout(function()
            {
                self.addCSS();
                self.addDivs(self);
                self.overwrite();
                self.applyCustomSettings();
            }, 61);
        }
    }

    ConsoleLogViewer.prototype.getFormattedTime = function()
    {
        var date = new Date();
        return this.format(date.getHours(), 2) + ":" + this.format(date.getMinutes(), 2) + ":" + this.format(date.getSeconds(), 2) + ":" + this.format(date.getMilliseconds(), 3);
    }

    ConsoleLogViewer.prototype.format = function(v, x)
    {
        if (x == 2) return  (v < 10) ? "0" + v : "" + v;
        else if (x == 3)
        {
            if  (v < 10) return "00" + v;
            else if  (v < 100) return "0" + v;
            else return "" + v;
        }
    }

    ConsoleLogViewer.prototype.log = function(args, color, splitArgs)
    {
        if (!ConsoleLogViewer.LOG_ENABLED) return;

        var content = args;//(splitArgs ? Array.prototype.slice.call(args).join(",") : args);
        //if (content != null && (content.indexOf("script") > -1)) return; // Want to log script ? No.

        _items.push("<font class='log-date'>" + this.getFormattedTime() + "</font> &nbsp; <font class='" + color + "'>" + content + "<\/font>");
        while (_items.length > ConsoleLogViewer.TOTAL) _items.shift();


        this.updateLog();


        var elem = document.getElementById('debug_console');
        elem.scrollTop = elem.scrollHeight+200;
    }

    ConsoleLogViewer.prototype.updateLog = function()
    {
        if (!ConsoleLogViewer.IS_MINIMIZED)
        {
            document.getElementById('debug_console_messages').innerHTML = _items.join("<br>");
        }
        else
        {
            var minimized = [];
            for(var i = Math.max(0, _items.length-3), leni = _items.length; i < leni ; i++) minimized.push(_items[i]);
            document.getElementById('debug_console_messages').innerHTML = minimized.join("<br>");
        }
    }

    ConsoleLogViewer.prototype.flatten = function(value)
    {
        return value.split("<").join("&lt;").split(">").join("&gt;").split("\"").join("&quot;");
    }
    ConsoleLogViewer.prototype.windowResize = function(width, height)
    {
        var d = document.getElementById("debug_console");
        d.style.width = width.toString() + "px";
        d.scrollTop = d.scrollHeight+200;

    }



    ConsoleLogViewer.prototype.applyCustomSettings = function()
    {
        var url = window.location.href;
        ConsoleLogViewer.TOTAL = 999999;
        var d = document.getElementById("debug_console");
        d.style.pointerEvents = "auto";
        d.style.position = "fixed";
        d.style.left = "auto";
        d.style.top = "55px";
        //d.style.bottom = "13px";
        d.style.right = "13px";
        d.style.maxHeight = "88%";
        d.style.width = "28%";
        d.style.background = "transparent";
        d.style.overflow = "auto";
        var m = document.getElementById("debug_console_messages");
        m.style.font = "11px monospace";
        document.getElementById("debug_console_close_button").style.display="none";
        document.getElementById("debug_console_minimize_button").style.display="none";
        document.getElementById("debug_console_position_button").style.display="none";
        document.getElementById("debug_console_pause_button").style.display="none";

    }

    ConsoleLogViewer.prototype.overwrite = function()
    {
        var self = this;
        // store original functions
        var original =
        {
console:
            {
log:
                console.log,
debug:
                console.debug,
info:
                console.info,
warn:
                console.warn,
error:
                console.error
            },
window:
            {
onerror:
                window.onerror
            }
        }

        // overwrite original functions
        if (original.console.log) console.log = function()
        {
            self.log(self.flatten(Array.prototype.slice.call(arguments).join(",")),"log-normal", true);
            original.console.log.apply(this, arguments);
        }
        if (original.console.debug) console.debug = function()
        {
            self.log(self.flatten(Array.prototype.slice.call(arguments).join(",")),"log-debug", true);
            original.console.debug.apply(this, arguments);
        }
        if (original.console.info) console.info = function()
        {
            self.log(self.flatten(Array.prototype.slice.call(arguments).join(",")),"log-info", true);
            original.console.info.apply(this, arguments);
        }
        if (original.console.warn) console.warn = function()
        {
            self.log(self.flatten(Array.prototype.slice.call(arguments).join(",")),"log-warn", true);
            original.console.warn.apply(this, arguments);
        }
        if (original.console.error) console.error = function()
        {
            self.log(self.flatten(Array.prototype.slice.call(arguments).join(",")),"log-error", true);
            original.console.error.apply(this, arguments);
        }
        window.onerror = function(message, url, lineNumber) {
            self.log([message, "<a target='_blank' href='"+url+"#"+lineNumber+"'>"+url+"</a>", "line:" + lineNumber], "log-error", true);
            if (original.window.onerror) return original.window.onerror(message, url, lineNumber);
            else return false;
        }
    }

    ConsoleLogViewer.prototype.addDivs = function(self)
    {
        var self = self;
        var alignment = window.location.href.indexOf("console_at_bottom=true") > -1 || window.location.href.indexOf("console_at_bottom=1") > -1 ? "bottom-aligned" : "top-aligned";
        var scripts = window.document.getElementsByTagName('script');
        for (var i=0; i<scripts.length; i++)
        {
            var script = scripts[i];
            if(typeof script !== 'undefined' && typeof script.src !== 'undefined')
            {
                if (script.src.indexOf('console-log-viewer.js') !== -1)
                {
                    if (script.src.indexOf('align=bottom') !== -1)
                    {
                        ConsoleLogViewer.ALIGNMENT = "bottom";
                    }
                    if (script.src.indexOf('minimized=true') !== -1)
                    {
                        ConsoleLogViewer.IS_MINIMIZED = true;
                    }
                    if (script.src.indexOf('closed=true') !== -1)
                    {
                        ConsoleLogViewer.IS_CLOSED = true;
                    }
                    if (script.src.indexOf('log_enabled=false') !== -1)
                    {
                        ConsoleLogViewer.LOG_ENABLED = false;
                    }
                }
            }
        }


        var div = document.createElement('div');
        div.id = "debug_console";
        var html = '<a href="#close" id="debug_console_close_button" class="log-button"></a>';
        html += '<a href="#minimize" id="debug_console_minimize_button" class="log-button"></a>';
        html += '<a href="#position" id="debug_console_position_button" class="log-button"></a>';
        html += '<a href="#pause" id="debug_console_pause_button" class="log-button"></a>';
        html += '<div id="debug_console_messages"></div>';
        div.innerHTML = (html);
        document.getElementsByTagName('body')[0].appendChild(div);

        document.getElementById("debug_console_close_button").addEventListener("click", function(e) {
            //div.style.display = "none";
            ConsoleLogViewer.IS_CLOSED = !ConsoleLogViewer.IS_CLOSED;
            self.setClosed(ConsoleLogViewer.IS_CLOSED);
            e.preventDefault();
        }, false);

        window.addEventListener("keydown", function(e) {
            if (e.keyCode == 192) {
                ConsoleLogViewer.IS_CLOSED = !ConsoleLogViewer.IS_CLOSED;
                self.setClosed(ConsoleLogViewer.IS_CLOSED);
            }
        }, false);

        document.getElementById("debug_console_minimize_button").addEventListener("click", function(e) {
            ConsoleLogViewer.IS_MINIMIZED = !ConsoleLogViewer.IS_MINIMIZED;
            self.setMinimized(ConsoleLogViewer.IS_MINIMIZED);
            e.preventDefault();
        }, false);

        document.getElementById("debug_console_position_button").addEventListener("click", function(e) {
            ConsoleLogViewer.ALIGNMENT = ConsoleLogViewer.ALIGNMENT == "top" ? "bottom" : "top";
            self.alignTo(ConsoleLogViewer.ALIGNMENT);
            e.preventDefault();
        }, false);

        document.getElementById("debug_console_pause_button").addEventListener("click", function(e) {
            ConsoleLogViewer.LOG_ENABLED = !ConsoleLogViewer.LOG_ENABLED;
            self.setLogEnabled(ConsoleLogViewer.LOG_ENABLED);
            e.preventDefault();
        }, false);

        self.setClosed(ConsoleLogViewer.IS_CLOSED);
        self.setMinimized(ConsoleLogViewer.IS_MINIMIZED);
        self.setLogEnabled(ConsoleLogViewer.LOG_ENABLED);
        self.alignTo(ConsoleLogViewer.ALIGNMENT);
    }

    ConsoleLogViewer.prototype.setClosed = function(value) { // true | false
        ConsoleLogViewer.IS_CLOSED = value;

        document.getElementById("debug_console_close_button").innerHTML = !value ? "x" : "&Xi;";
        document.getElementById("debug_console").style.background = !value ? "rgba(0, 0, 0, .75)" : "transparent";
        document.getElementById("debug_console_messages").style.display = !value ? "block" : "none";
        document.getElementById("debug_console_minimize_button").style.display = !value ? "inline" : "none";
        document.getElementById("debug_console_position_button").style.display = !value ? "inline" : "none";
        document.getElementById("debug_console_pause_button").style.display = !value ? "inline" : "none";
    }
    ConsoleLogViewer.prototype.show = function()
    {
        ConsoleLogViewer.IS_CLOSED = false;
        document.getElementById("debug_console").style.background = "rgba(0, 0, 0, 0)";
        document.getElementById("debug_console_messages").style.display = ""
    }

    ConsoleLogViewer.prototype.hide = function()
    {
        ConsoleLogViewer.IS_CLOSED = true;
        document.getElementById("debug_console").style.background = "transparent";
        document.getElementById("debug_console_messages").style.display = "none";
    }

    ConsoleLogViewer.prototype.setLogEnabled = function(value) { // true | false
        ConsoleLogViewer.LOG_ENABLED = value;
        document.getElementById("debug_console_pause_button").innerHTML = (!value ? "||" : "&#9658;");
    }

    ConsoleLogViewer.prototype.setMinimized = function(value) { // true | false
        ConsoleLogViewer.IS_MINIMIZED = value;
        document.getElementById("debug_console_minimize_button").innerHTML = value ? "+" : "-";
        this.updateLog();
    }

    ConsoleLogViewer.prototype.alignTo = function(value) { // top | bottom
        ConsoleLogViewer.ALIGNMENT = value;
        document.getElementById("debug_console").className = value == "bottom" ? "bottom-aligned" : "top-aligned";
        document.getElementById("debug_console_position_button").innerHTML = value == "bottom" ? "&uarr;" : "&darr;";
    }

    ConsoleLogViewer.prototype.addCSS = function()
    {
        var css = '#debug_console { background: rgba(0,0,0,.75); position:fixed; padding:0; margin:0; z-index:12834567; box-sizing:border-box; pointer-events:none; text-align:left; text-transform:none; }';
        css += '#debug_console, #debug_console * { font: 11px monospace, sans-serif!important;  }';
        css += '#debug_console_messages { background:transparent;pointer-events:none; }'
               css += '#debug_console_button { border:1px solid #fff; position:absolute; z-index:2;  }';
        css += '#debug_console.top-aligned {left:0; right:0; top:0;}';
        css += '#debug_console.minimized {left:0; right:0; top:0;}';
        css += '#debug_console.bottom-aligned {left:0; right:0; bottom:0;}';
        css += '#debug_console a.log-button {font: bold 12px monospace, sans-serif!important; pointer-events:all; text-align:center; text-decoration:none; border:1px solid #999; background:#333; color:#fff; width:16px; height:16px; padding:5px; margin:1px; display:block; float:right; box-sizing: content-box; }';
        css += '#debug_console font.log-error a {pointer-events:all;color:red;}';
        css += '#debug_console font.log-date {color:gray;}';
        css += '#debug_console font.log-info {color:yellow;}';
        css += '#debug_console font.log-warn {color:orange;}';
        css += '#debug_console font.log-debug {color:lightblue;}';
        css += '#debug_console font.log-error {color:red;}';
        css += '#debug_console font.log-normal {color:#DCDCDC;}';

        var style = document.createElement('style');
        style.type = 'text/css';
        if (style.styleSheet) style.styleSheet.cssText = css;
        else style.appendChild(document.createTextNode(css));

        document.getElementsByTagName('head')[0].appendChild(style);
    }

    return ConsoleLogViewer;
})();

var logviewer = new ConsoleLogViewer();
