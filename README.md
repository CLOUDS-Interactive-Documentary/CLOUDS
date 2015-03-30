CLOUDS
======

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
