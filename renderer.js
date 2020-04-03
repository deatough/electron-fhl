// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// No Node.js APIs are available in this process because
// `nodeIntegration` is turned off. Use `preload.js` to
// selectively enable features needed in the rendering
// process.
const { ipcRenderer } = require('electron')

// In electron there are two separate JavaScript contexts, the main host application which
// runs in Node and the render which runs within the context of the browser. The call to the
// C++ addon will be made in the main host application so we communicate using the ipcRenderer class.
async function getDataFromHost() {
  const res = await ipcRenderer.sendSync('getDataFromHost', null);
  return res;
}

// The call from the C++ application will happen in the host process, we setup a receiver to get
// messages from the host and then call the startClock method to tell the host that we are ready
// to start receiving updates on the time.
async function startClock(callback) {
  ipcRenderer.on('onClockUpdated', async (event, arg) => {
    callback(arg)
  });

  const res = await ipcRenderer.sendSync('startClock', null);
  return res;
}

// React component for the timer field, this is implemented as a sperate component because
// it will receive many calls to update the state.
class TimerField extends React.Component {
  constructor () {
    super();
    this.state = {};
  }

  componentDidMount() {
    var pThis = this;
    startClock(timeString => this.setState({timeString}));
  }

  render () {
    var currentTimeString = '<loading ...>';
    if (this.state.timeString != undefined)
    {
      currentTimeString = this.state.timeString;
    }

    return (
      <div className='CurrentTime'>
        C++ calls a JavaScript method to update the following value:<br/>
        <section className='MainScreen-native'>{currentTimeString}</section>
      </div>
    );
  }
}

// Main react component of the application
class MainScreen extends React.Component {
  constructor () {
    super();
    this.state = {};
  }

  componentDidMount() {
    getDataFromHost()
      .then(message => this.setState({message}));
  }

  render () {
    var fromNative = '<loading ...>';
    if (this.state.message != undefined)
    {
      fromNative = this.state.message;
    }

    return (
      <div className='MainScreen'>
        <header className='MainScreen-header'>
          <img src={'./logo.svg'} className='MainScreen-logo' alt='logo' />
          <p>
            This FHL project provided learnings about communication between ReactJS and native C++ code. It was done using <a href='https://www.electronjs.org/' className='MainScreen-link'>Electron</a> as the host for ReactJS and <a href='https://nodejs.org/api/n-api.html' className='MainScreen-link'>N-Api</a> to communicate between the JavaScript and C++ layers.
          </p>
          <p>
            JavaScript calls a C++ method to get the following value:<br/>
            <section className='MainScreen-native'>{fromNative}</section>
          </p>
          <TimerField />
          <p>
            See <a href='https://github.com/deatough/electron-fhl' className='MainScreen-link'>https://github.com/deatough/electron-fhl</a> for the source code and the readme.md for instructions on building the project.
          </p>
        </header>
      </div>
    );
  }
}

ReactDOM.render(
  <React.StrictMode>
    <MainScreen />
  </React.StrictMode>,
  document.getElementById('react_contents')
);
