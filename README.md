# C++ HTTP Server (Windows)

## Overview 

Basic HTTP Server that serves static html files, with basic routing implemented, along with basic multithreading.

## Features
- Multiple Connections
- Static File Serving
- Logging

## Just want to run the program
I have already compiled the executable which is placed in `./src/windows/WindowsServer.exe`. Simply launch the application to get started, and you will be prompted to open `https://localhost:8080`. Simply hold down the <ctrl> key and left click the link to launch your default browser to the specified address. Assuming the server is still open and you've navigated to the correct address, you should be looking at a static HTML page that contains the links. If you click on the links, it sends a request to the server, which is printed both in its raw form, and a parsed format. Once the request is received and printed, the server sends a response containing the HTML page which has been loaded from the local storage on the server and sent in an HTTP response, which is then parsed by the browser, and then responds by displaying the content received. 

## Steps to compile this thing from source
Before attempting to compile, ensure you have the following dependencies:
- Operating System: Windows
- C++ Compiler: Microsoft Visual Studio OR Developer Powershell for VS 2022 (What I use for compiling with VSCode)
- CMake
- Build Tool: NMake

## Steps to compile
### 0. Clone the repo if you wanted
```bash
git clone https://github.com/noah-tah/cpp-http-server.git
```
### 1. Verify Dependencies
```ps
cmake --version
cl
nmake
```
### 2. Open Developer Command Prompt for VS
- Nativigate to root of project directory 
```ps
cd C:\path\to\cpp-http-server`
```
### 3. Make a build directory
```ps
mkdir build
cd build
```
### 4. Generate Build Files with CMake
```ps
cmake .. -G "NMake Makefiles"
```

### 5. Compile project
```ps
nmake
```
### 6. Run the Project
```ps
.\server.exe
```
or 
```ps
explorer.exe .
```
Then click on the executable.

