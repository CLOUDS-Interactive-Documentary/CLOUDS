CLOUDS
======

UPDATE September 15th 2024
Reviving CLOUDS for permanent storage, including source code complilation on Windows and Mac.

Note that several of these steps require private media and source code. These steps are for archiving purposes only, and those attempting to build CLOUDS from source should get in touch directly with James George (@obviousjim)

Steps to get Windows 10 compiling
1. Download [Visual Studio 2012 Ultimate from Internet Archive](https://archive.org/details/vs-2012-rc-ult-enu)
2. Clone the `master` branch of openFrameworks from [here](https://github.com/CLOUDS-Interactive-Documentary/openframeworks/). 
3. Clone this `master` branch of this repository into the `openFrameworks/apps` directory. 
4. Run the `cloneaddons.sh` script to pull all required addons in the appropriate location. Run the following commands in your Command Line:

    ```
    cd openFrameworks/apps/CLOUDS
    ./cloneaddons.sh
    ```
5. WINDOWS ONLY: Change the ofxAnimationPrimitives branch to `windows-old`. Run the following commands in your Command Line:

    ```
    cd openFrameworks/addons/ofxAnimationPrimitives
    git checkout windows-old
    ```
WINDOWS ONLY: Change the ofxOculusDK2 branch to `oculus-1.3.0`. Run the following commands in your Command Line:

    ```
    cd openFrameworks/addons/ofxOculusDK2
    git checkout oculus-1.3.0
    ```

6. PRIVATE STEP: Clone Private Visual Systems https://github.com/obviousjim/VisualSystemsPrivate into `CloudsLibrary/src/VisualSystemsPrivate` - This private repository for Casey Reas and LIA's artowrk that is not authorized for public distribution
7. Download `CloudsDataIgnored/` and place in `openFrameworks/apps/CLOUDS/`
8. Download `CloudsDataMedia/` and place in `openFrameworks/apps/CLOUDS/`
9. Download `CloudsCrypto.h` and place in `openFrameworks/app/CLOUDS/CloudsLibrary/src/`
10. Open the Visual Studio 2012 solution at `openFrameworks/apps/CLOUDS/CLOUDS/CLOUDS.sln`
11. Set CLOUDS build target as Startup Project and configure to `Release` and 
12. Build CLOUDS. You should have no compile errors
13. Copy the `.dll` files from `openFrameworks/export/vs64/` into `openFrameworks/apps/CLOUDS/CLOUDS/bin/` next to the `CLOUDS.exe` replacing any existing executables there
14. Launch CLOUDS and emjoy!


June 12 2014

We're opening up the CLOUDS repo as the beginning of a conversation, as a learning tool. We've all learned a lot from working on this project and had a blast doing it, and we feel like sharing the work is the next step. We hope you enjoy it and find it useful, and would love to hear what you do with it.

Not everything is going to work out of the box, but we'll be adding components and how-tos as we move forward. If you have any questions or problems, open an issue and we'll be happy to help.

### Set Up

1. Download the `CLOUDS` branch of openFrameworks from [here](https://github.com/CLOUDS-Interactive-Documentary/openframeworks/tree/CLOUDS).
1. Clone CLOUDS into the `OF/apps` directory.
1. Run the `cloneaddons.sh` script to pull all required addons in the appropriate location. Run the following commands in your Terminal:

    ```
    cd OF/apps/CLOUDS
    cloneaddons.sh
    ```
1. While we fix a few things with the open source version, you'll also need to follow the few extra fixes listed here: https://github.com/CLOUDS-Interactive-Documentary/CLOUDS/issues/41

### Structure

* A **Visual System** is a code module that can be configured to present dynamic generative and interactive graphics. The Visual Systems are artworks and algorithmic examples contributed by artists featured in the documentary, as well as collaborators on the project. 
  * Some Visual Systems are original commissioned works by artists represented in the film.
  * Others are canonical examples of algorithms and libraries commonly used in creative coding. Topics explored include computer vision, generative graphics, audio-reactive feedback, physics simulation, and more.
* A **Standalone** is a project that runs a single Visual System. Standalones can compile for up to three different targets: desktop, Oculus Rift, and Kinect v2 (through OSC). You can find Standalones in the `OF/apps/CLOUDS/VSStandaloneName` directories.
* The main **CLOUDS** project is located in the `OF/apps/CLOUDS/CLOUDS` directory. It currently won't run as it's missing dependencies, but we're working on it. Stay tuned.

### Navigation

* `f` to toggle fullscreen mode
* `h` to toggle the GUI
* `e` `r` `t` `y` to re-arrange the GUI
