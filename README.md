CLOUDS
======

UPDATE September 15th 2024
Reviving CLOUDS for permanent storage, including source code complilation on Windows and Mac.

Note that several of these steps require private media and source code. These steps are for archiving purposes only, and those attempting to build CLOUDS from source should get in touch directly with James George (@obviousjim)


# Setup Build Environment
## Windows
* Requires Windows 11 or earlier
* Download [Visual Studio 2012 Ultimate from Internet Archive](https://archive.org/details/vs-2012-rc-ult-enu)
## macOS
* Requires a Mac Intel machine (does not work on MacARM / Apple Silicon)
* Requires El Capitan (10.11)
* Xcode 7.3.1

# Prepare
1. Clone the `master` branch of openFrameworks from [here](https://github.com/CLOUDS-Interactive-Documentary/openframeworks/). 
2. Clone this `master` branch of this repository into the `openFrameworks/apps` directory. 
3. Run the `cloneaddons.sh` script to pull all required addons in the appropriate location. Run the following commands in your Command Line:

    ```
    cd openFrameworks/apps/CLOUDS
    ./cloneaddons.sh
    ```
4. WINDOWS ONLY: Change the ofxAnimationPrimitives branch to `windows-old`. Run the following commands in your Command Line:

    ```
    cd openFrameworks/addons/ofxAnimationPrimitives
    git checkout windows-old
    ```
5. WINDOWS ONLY: Change the ofxOculusDK2 branch to `oculus-1.3.0`. Run the following commands in your Command Line:

    ```
    cd openFrameworks/addons/ofxOculusDK2
    git checkout oculus-1.3.0
    ```
# Private Steps
1. Clone Private Visual Systems https://github.com/obviousjim/VisualSystemsPrivate into `CloudsLibrary/src/VisualSystemsPrivate` - This private repository for Casey Reas and LIA's artowrk that is not authorized for public distribution
2. Download `CloudsDataIgnored/` and place in `openFrameworks/apps/CLOUDS/`
3. Download `CloudsDataMedia/` and place in `openFrameworks/apps/CLOUDS/`
4. Download `CloudsCrypto.h` and place in `openFrameworks/app/CLOUDS/CloudsLibrary/src/`

# Build
## Windows
1. Open the Visual Studio 2012 solution at `openFrameworks/apps/CLOUDS/CLOUDS/CLOUDS.sln`
2. Set CLOUDS build target as Startup Project.
3. Configure to `Release`
4. Build CLOUDS.
5. Copy the `.dll` files from `openFrameworks/export/vs64/` into `openFrameworks/apps/CLOUDS/CLOUDS/bin/` next to the `CLOUDS.exe` replacing any existing executables there
## Mac
1. Open the Xcode 7.3.1 project at `openFrameworks/apps/CLOUDS/CLOUDS/CLOUDS.xcodeproj`
2. Select the CLOUDS build target.
3. Select Edit Scheme > Build Configuration: Release
4. Build.

Launch CLOUDS and emjoy!

June 12 2014

We're opening up the CLOUDS repo as the beginning of a conversation, as a learning tool. We've all learned a lot from working on this project and had a blast doing it, and we feel like sharing the work is the next step. We hope you enjoy it and find it useful, and would love to hear what you do with it.

Not everything is going to work out of the box, but we'll be adding components and how-tos as we move forward. If you have any questions or problems, open an issue and we'll be happy to help.

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
