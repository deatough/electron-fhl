// Modules to control application life and create native browser window
const {app, BrowserWindow, ipcMain} = require('electron')
const path = require('path')
const addon = require('./addon/build/Release/addon')

// The C++ addon is called from the host application, to allow the renderer
// to call this method we listen for events sent over the electron ipc
// channel and then call the C++ addon.
ipcMain.on('getDataFromHost', async (event, arg) => {
  event.returnValue = addon.getDataFromHost();
});

// The call to startClock informs the C++ addon that is can start sending
// updates and identifies the JavaScript method that should be called.
ipcMain.on('startClock', async (event, arg) => {
  event.returnValue = addon.startClock('onClockUpdated');
});

var mainWindow = undefined;
var mainWindowLoaded = false;

// The onClockUpdated method will be called by the C++ addon to provide the updated
// time string. It is added to the global object to make it possible to locate the
// function from C++ using the N-Api
global.onClockUpdated = function(timeString) {
  if (mainWindow != undefined)
  {
    if (mainWindowLoaded)
    {
      mainWindow.webContents.send('onClockUpdated', timeString);
    }
  }
};

function createWindow () {
  // Create the browser window.
   mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      nodeIntegration: true // needed to use import statements
    }
  })

  // and load the index.html of the app.
  mainWindow.loadFile('index.html')

  mainWindow.webContents.on('did-finish-load', () => {
    mainWindowLoaded = true;
  });

  // Open the DevTools.
  //mainWindow.webContents.openDevTools()
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(createWindow)

// Quit when all windows are closed.
app.on('window-all-closed', function () {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') app.quit()
})

app.on('activate', function () {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) createWindow()
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.
