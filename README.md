Radome
======

Radome is a platform for realtime interactive animation on a dome. It manages both 2D and 3D content
for projection onto a fulldome installation with multiple projectors.

This is pre-release software optimized for our particular circumstances and hardware, but it should
theoretically be useful to other people doing rear projection onto skinned domes (or, with small modifications, other geometry.)

Radome has been tested on OSX 10.8 through 10.10, with both Xcode 5 and 6. Windows support is experimental, with key features missing, 
but it is known to build with Visual Studio 2012 on Windows 7. (Video input with Spout instead of Syphon is planned.)

Radome is built with openFrameworks v0.8.4.  The following non-core addons are required:
* ofxCubeMap
* ofxUI
* ofxSyphon (OSX only)
* ofxAutoReloadedShader
* ofxAppGLFWWindowMulti

Installation instructions:
--------------------------

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
4. Open radome/radome.xcodeproj (Xcode) or radome/radome.sln (Visual Studio 2012) and build.

License
-------

Radome is built on openFrameworks and its many addons, most of which are MIT licensed. The code in this repository is licensed under the GPLv3.
