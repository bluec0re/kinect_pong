kinect_pong
===========


A Pong implementation controlled by the Microsoft Kinect with

- OpenNi2
- Nite2
- libfreenect
- Ogre3D

install
=======

- requirements
  - cmake
  - Ogre3D 1.9
  - Boost (tested with 1.56)
  - OIS (tested with 1.3)
  - CEGUI 0.8.x
  - OgreOggSound (tested with r420)
  - (optional; but without fun when missing) OpenNi2 (tested with 2.2beta2)
  - (optional; but without fun when missing) libfreenect driver (tested with 0.4.2)
  - (optional; but without fun when missing) NiTE2 (tested with 2.2)
  - (optional)opencv [for recording feature]

- get openni2 + libfreenect from your package manager or from github
  - there are several tutorials in the WWW
- extract the corresponding NiTE 2.2 archive for your system into libs/Nite2.2
  - or use the bash script from the lib folder

Arch Linux specific:
- pacman -S ogre ois cegui cmake boost
- from AUR: openni2-libfreenect ogreoggsound-svn

Ubuntu 14.4:
- sudo apt-get install libogre-1.9-dev libois-dev cmake libusb-1.0-0-dev libudev-dev
- wget https://github.com/OpenKinect/libfreenect/archive/v0.4.2.tar.gz -O libfreenect-0.4.2.tar.gz
    - sudo apt-get install libusb-1.0-0-dev libudev-dev
    - tar xvf libfreenect-0.4.2.tar.gz
    - cd libfreenect-0.4.2
    - cmake -DBUILD_AUDIO=OFF -DBUILD_OPENNI2_DRIVER=ON -DLIB_SUFFIX="" -DBUILD_EXAMPLES=OFF
    - make && sudo make install
- wget https://github.com/OpenNI/OpenNI2/archive/2.2-beta2.tar.gz -O openni2.2-beta2.tar.gz
    - sudo apt-get install freeglut3-dev default-jdk   # for java wrapper
    - tar xvf openni2.2-beta2.tar.gz
    - cd OpenNI2-2.2-beta2
    - sed -i s%-mfloat-abi=softfp%% ThirdParty/PSCommon/BuildSystem/Platform.Arm
    - LDFLAGS+=" -pthread" make CFG=Release ALLOW_WARNINGS=1
    - As root:
        - echo /usr/lib/OpenNI2/Drivers > "/etc/ld.so.conf.d/openni2.conf"
        - sh Packaging/Linux/install.sh
        - mkdir -p /etc/OpenNI2 /usr/lib/OpenNI2/Drivers
        - cp -r Config/* "/etc/OpenNI2"
        - cp -r Include/* "/usr/include"
        - cp Bin/x64-Release/*.so /usr/lib
        - cp Bin/x64-Release/OpenNI2/Drivers/*.so /usr/lib/OpenNI2/Drivers
- wget http://prdownloads.sourceforge.net/crayzedsgui/cegui-0.8.4.tar.bz2?download -O cegui-0.8.4.tar.bz2
    - tar xvf cegui-0.8.4.tar.bz2
    - cd cegui-0.8.4
    - cmake .
    - make && sudo make install
- wget http://sourceforge.net/code-snapshots/svn/o/og/ogreoggsound/code/ogreoggsound-code-442-trunk.zip
    - sudo apt-get install libopenal-dev libvorbis-dev
    - unzip ogreoggsound-code-442-trunk.zip
    - cd ogreoggsound-code-442-trunk
    - cmake .
    - make && sudo make install
  
build
=====
  - mkdir build
  - cd build
  - cmake ..
  - make

running
=======

- ./build/src/KinectPong
