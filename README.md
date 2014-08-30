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
  - Ogre3D
  - Boost
  - OIS
  - CEGUI
  - OpenNi 2
  - libfreenect driver
  - NiTE 2

- get openni2 + libfreenect from your package manager or from github
  - there are several tutorials in the WWW
- extract the corresponding NiTE 2.2 archive for your system into libs/Nite2.2
  - or use the bash script from the lib folder

Arch Linux specific:
- pacman -S ogre ois cegui cmake boost
- from AUR: openni2 openni2-libfreenect
  
build
=====
  - mkdir build
  - cd build
  - cmake ..
  - make

running
=======

- ./build/KinectPong
