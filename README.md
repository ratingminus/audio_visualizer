# Audio Visualizer
![Project Cover](res/cover.png)
> An SDL based Audio Visualizer using the fftw3 library

* YouTube Link: https://youtu.be/W0MV3Ehnk0A
* Library Pre-requisites : 
  * ```*nix```:
    * Debian Based Distributions : 
       ```build-essential``` ```libsdl2-dev``` ```libsdl2-image-dev``` ```libsdl2-ttf-dev``` ```libfftw3-dev```
    * Others : Install equaivalent packages of the above mentioned. 
    * ```youtube-dl``` and ```ffmpeg``` required for the download feature or alternatively you can run ```install.sh```
    * ```P.S.```: Make sure you have ```GLIB_C2.29``` or above (comes preinstalled from ```Ubuntu_19.04``` onwards)
  * ```Win64```:
    * run ```install.ps1``` with powershell
    * [```minGW```](https://winlibs.com/) (```GCC 64-bit with 'posix' thread model```)
    * You can install GCC with posix thread model with [```chocolatey```](https://chocolatey.org/) - ```choco install winlibs-llvm-free```
* Compile with ```make``` and run with ```./musico``` 
* Terminal usage:
    * ```./musico -f Path_to_wav_file``` to play a certain song
    * ```./musico -r``` for recording and then playback
    * ```./musico -R``` for real time audio visualization
* Key-Bindings:
    * pause/play/replay/start: ```space```
    * stop/quit: ```q```
    * changemode: ```m```
* Known Bugs:
    * Window resizing doesn't work on Windows due to Microsoft's ```OpenGL``` implementaion issue mentioned [here](https://github.com/libsdl-org/SDL/issues/1059)
    * If the user delays at the youtube link dialogue box the program stops responding as both win32 and posix thread models consider holding the main thread for a long amount of time as being unresponsive. We probably need to work with multithreading to resolve this issue.
