const {
  app,
  BrowserWindow,
  systemPreferences
} = require('electron')
const path = require('path')
const url = require('url')

let window = null


//exec('node '.concat(path.join(__dirname, 'oscServer.js')));


// Wait until the app is ready
app.once('ready', () => {
  // Create a new window
  window = new BrowserWindow({
    // Set the initial width to 500px
    width: 1100,
    // Set the initial height to 400px
    height: 500,
    // minimum width
    minWidth: 750,
    // minimum height
    minHeight: 450,
    // set the title bar style
    //titleBarStyle: 'hiddenInset',
    darkTheme: true,
    // set the background color to black
    backgroundColor: "#FFFFF",
    // Don't show the window until it's ready, this prevents any white flickering
    show: false,
    resizable: true

  })

  window.on('closed', function() {
    app.quit();
  });
    // No menu bar! We handle all settings in the sidebar
  window.setMenu(null);
  window.setMenuBarVisibility(false)

  window.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }))

  window.once('ready-to-show', () => {
    window.show()
  })
})
