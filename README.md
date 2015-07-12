Radome
======

Radome is a platform for realtime interactive visualization on a dome. It manages both 2D and 3D content
for projection onto a fulldome installation with multiple projectors.

This is pre-release software optimized for our particular circumstances and hardware, but it should
theoretically be useful to other people doing projection onto skinned domes (or, with small modifications, other geometry.)

Radome has been tested on OSX 10.8 through 10.10, built with both Xcode 5 and 6, and uses [Syphon](http://syphon.v002.info/)
to exchange video with other software.

Windows support is newer but has so far tested well on Windows 7 and the Windows 10 beta, built with
Visual Studio 2012 Express. Inter-app video uses the [Spout 2](http://spout.zeal.co/) library.

Key features
-------
* Handles projector transformation of up to 4 projectors
* Supports Matrox Triplehead output, individual outputs, and mixed scenarios 
* Displays animated 3D content onto dome surface in real time, for game worlds or interactive visualization
* Warps 2D content to the dome surface using customizable shader algorithms; plays fulldome masters
* Support Syphon (OSX) and Spout (Windows) for receiving video input from other video software & devices
* Simple C++ plugin interface for interactive content

Known Issues
-------

* Physical model is not yet fully correct, some fudging of measured distances and angles is required
* Built-in support for 3D content interactivity is limited (but plugins can implement custom interaction with OSC)
* No feathering of overlapping projectors when using front projection
* Only supports domes (arbitrary geometry coming soonish)
* User experience somewhat harmed by limited openFrameworks application tooling (future version may switch to JUCE)

Building Radome
---------------

Radome is built with openFrameworks v0.8.4.  The following non-core addons are required:
* ofxCubeMap
* ofxUI
* ofxSyphon (OSX only)
* ofxAutoReloadedShader
* ofxAppGLFWWindowMulti

### Detailed instructions:

1. Download [openFrameworks v0.8.4](http://openframeworks.cc/versions/v0.8.4/)

2. Clone the following oF extensions into the addons folder:
  * ofxSyphon (head revision) (OSX only)
  * ofxCubeMap (head revision)
  * ofxAutoReloadedShader (head revision)
  * ofxAppGLFWWindowMulti (fork)
  * ofxUI (fork)
  ```
  cd of_v0.8.4_osx_release/addons
  git clone https://github.com/astellato/ofxSyphon
  git clone https://github.com/andreasmuller/ofxCubeMap
  git clone https://github.com/andreasmuller/ofxAutoReloadedShader
  git clone https://github.com/Dewb/ofxAppGLFWWindowMulti
  cd ofxApplGLFWWindowMulti
  git checkout focus
  cd ..
  git clone https://github.com/Dewb/ofxUI
  cd ofxUI
  git checkout radome
  ```

3. Download or clone radome into the same parent folder as openFrameworks. (Do not put it in the apps folder.)
4. Open osx/radome.xcodeproj (OSX/Xcode) or windows/radome.sln (Windows/Visual Studio 2012) and build.


License
-------

Radome is built on openFrameworks and its many addons, most of which are MIT licensed. 
SpoutSDK is licenced under the Simplified BSD licence.
The code in this repository is licensed under the GPLv3. Contact @dewb if you are interested in discussing other licensing options.
