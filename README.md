# electron-fhl

This is a forked version of the [Electron Quick Start](https://github.com/electron/electron-quick-start) project that I created for the Fix Hack Learn week. It uses [N-Api](https://nodejs.org/api/n-api.html) to provide communication between JavaScript and C++.

# Building and running

Install build tools for building the addon

   npm install --global --production windows-build-tools
   npm install -g node-gyp

In the command prompt, navigate  to addon sub directory and run:
   node-gyp configure
   node-gyp rebuild

This should create the ./addon/build/release/addon.node file, which is the C++ dll containing the native code.

Insall and start the electorn app using the following commands
npm install
npm start

How I debugged
    C++, attach visual studio to the electron process with the window title
    main.js (Electron host) run the "Debug Main Process" window from Visual Studio Code
    renderer.js (ReactJS) open the Developer Tools (from the View menu)