radome
======

Radome is a platform for realtime interactive animation on a dome. It manages both 2D and 3D content
for projection onto a fulldome installation with multiple projectors.

This is pre-release software optimized for our particular circumstances and hardware, but it should
theoretically deliver value in other use cases involving rear projection onto domes.

Radome has been tested on OSX 10.8 and 10.9. Windows support (without Syphon) is planned,
but at the moment the build tree is Mac only.

Radome is built with openFrameworks v0.7.4.  The following non-core addons are required:
* ofxCubeMap
* ofxFenster
* ofxUI
* ofxSyphon
* ofxAutoReloadedShader

Installation instructions:
--------------------------

1. Download [openFrameworks v0.7.4](http://openframeworks.cc/download/older.html)

2. Clone the following oF extensions into the addons folder:
  * ofxSyphon (head revision)
  * ofxCubeMap (head revision)
  * ofxAutoReloadedShader (head revision)
  * ofxFenster (branch 'ghost')
  * ofxUI (commit 51b2dcb5d6bef17e18c23adf91ae94fce095f42d)
  ```
  cd of_v0.7.4_osx_release/addons
  git clone https://github.com/astellato/ofxSyphon
  git clone https://github.com/andreasmuller/ofxCubeMap
  git clone https://github.com/andreasmuller/ofxAutoReloadedShader
  git clone https://github.com/underdoeg/ofxFenster.git
  cd ofxFenster
  git checkout ghost
  cd ..
  git clone https://github.com/rezaali/ofxUI
  cd ofxUI
  git checkout 51b2dcb5d6bef17e18c23adf91ae94fce095f42d
  ```

3. Download or clone radome into the same parent folder as openFrameworks. (Do not put it in the apps folder.)
4. Open radome/radome.xcodeproj in Xcode and build.

License
-------

Radome is built on openFrameworks and its many addons, most of which are MIT licensed. The code in this repository is licensed under the GPLv3.
