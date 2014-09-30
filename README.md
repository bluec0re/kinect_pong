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
  
build
=====
  - mkdir build
  - cd build
  - cmake ..
  - make

running
=======

- ./build/src/KinectPong
