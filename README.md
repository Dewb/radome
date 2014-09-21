radome
======

Radome is a platform for realtime interactive animation on a dome. It manages both 2D and 3D content
for projection onto a fulldome installation with multiple projectors.

This is a early-stage prototype.  Windows support is planned but right now the build tree is Mac only.

Radome is built with openFrameworks 0073.  The following non-core addons are required:
* ofxCubeMap
* ofxFenster
* ofxUI
* ofxSyphon

Installation instructions:
--------------------------

1. Download [openFrameworks v0073](http://openframeworks.cc/download/older.html)

2. Clone the following oF extensions into the addons folder:
  * ofxSyphon (head revision)
  * ofxCubeMap (head revision)
  * ofxFenster (branch 'ghost')
  * ofxUI (commit 51b2dcb5d6bef17e18c23adf91ae94fce095f42d)
  ```
  cd of_v0.8.1_osx_release/addons
  git clone https://github.com/astellato/ofxSyphon
  git clone https://github.com/andreasmuller/ofxCubeMap
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
